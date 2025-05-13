#include "sudp/net/session.hpp"

#include "sudp/core/udp_header.hpp"
#include "sudp/core/reassembly_buffer.hpp"
#include "sudp/net/db_queue.hpp"

#include <octomap/OcTree.h>          // OctoMap 1.9
#include <asio.hpp>
#include <zlib.h>

#include <sstream>
#include <iostream>
#include <chrono>
#include <cstring>
#include <unordered_map>

using sudp::core::UdpHdr;
using sudp::core::ReassemblyBuffer;
using sudp::db::PointRGB;

namespace sudp::net
{
/* ────────────────────────── ctor ───────────────────────────── */
Session::Session(asio::ip::udp::socket&& sock,
                 DbQueue&               dbq,
                 std::size_t            mtu)
  : socket_(std::move(sock)), db_queue_(dbq), mtu_(mtu)
{
    read_next();
}

/* ───────────────────────── RX loop ─────────────────────────── */
void Session::read_next()
{
    recv_buf_.resize(mtu_ + sizeof(UdpHdr));
    socket_.async_receive_from(
        asio::buffer(recv_buf_), remote_,
        [this](std::error_code ec, std::size_t n)
        {
            if (!ec) handle_packet(n);
            read_next();
        });
}

/* ───────────────────── handle_packet ───────────────────────── */
void Session::handle_packet(std::size_t nbytes)
{
    if (nbytes < sizeof(UdpHdr)) return;

    const auto* hdr  = reinterpret_cast<const UdpHdr*>(recv_buf_.data());
    const auto* body = recv_buf_.data() + sizeof(UdpHdr);

    /* ► re‑assemblage --------------------------------------------------- */
    auto& buf = buffers_[hdr->seq];
    if (!buf) buf = std::make_unique<ReassemblyBuffer>(hdr->tot);
    if (!buf->write(hdr->off, body, hdr->len)) return;
    if (!buf->complete()) return;

    std::vector<uint8_t> blob = std::move(buf->data());
    buffers_.erase(hdr->seq);

    /* ► décompression gzip --------------------------------------------- */
    if (hdr->flags & sudp::core::FLAG_GZIP) {
        if (blob.size() < 4) return;
        uint32_t raw = *reinterpret_cast<uint32_t*>(blob.data());
        std::vector<uint8_t> out(raw);
        uLongf dst = raw;
        if (uncompress(out.data(), &dst,
                       blob.data() + 4, blob.size() - 4) != Z_OK)
            return;
        blob.swap(out);
    }

    /* ► désérialisation OctoMap (API 1.9) ------------------------------ */
    std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
    ss.write(reinterpret_cast<char*>(blob.data()), blob.size());
    ss.seekg(0);

    std::unique_ptr<octomap::AbstractOcTree> at{
        octomap::AbstractOcTree::read(ss)        // ← fonction disponible en 1.9
    };
    auto* tree = dynamic_cast<octomap::OcTree*>(at.get());
    if (!tree) return;

    /* ► parcours des feuilles occupées --------------------------------- */
    const int64_t now_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<PointRGB> pts;
    pts.reserve(tree->size());

    for (auto it = tree->begin_leafs(); it != tree->end_leafs(); ++it) {
        if (!tree->isNodeOccupied(*it)) continue;
        PointRGB p{};
        p.x = it.getX(); p.y = it.getY(); p.z = it.getZ();
        p.r = 128; p.g = 128; p.b = 128; p.a = 255;
        p.ts = now_ms;
        pts.emplace_back(std::move(p));
    }

    if (!pts.empty())
        db_queue_.push_points(std::move(pts));
}

} // namespace sudp::net
