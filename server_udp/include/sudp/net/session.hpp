#pragma once
#include <asio.hpp>
#include <unordered_map>
#include <memory>
#include <vector>

#include "sudp/core/udp_header.hpp"
#include "sudp/core/reassembly_buffer.hpp"
#include "sudp/net/db_queue.hpp"

namespace sudp::net {

/**  Gère une socket UDP : réception, ré‑assemblage, push DB.  */
class Session
{
public:
    /**
     * @param sock   socket déjà ouverte et bindée.
     * @param dbq    file vers le thread PostgreSQL.
     * @param mtu    charge utile max (par défaut 1300 o).
     */
    Session(asio::ip::udp::socket&& sock,
            DbQueue& dbq,
            std::size_t mtu = 1300);

private:
    /* réception asynchrone */
    void read_next();
    void handle_packet(std::size_t nbytes);

    asio::ip::udp::socket            socket_;
    asio::ip::udp::endpoint          remote_;
    std::vector<uint8_t>             recv_buf_;

    DbQueue&                         db_queue_;
    std::size_t                      mtu_;

    using BufPtr = std::unique_ptr<core::ReassemblyBuffer>;
    std::unordered_map<uint32_t, BufPtr> buffers_;   // seq → buffer
};

} // namespace sudp::net
