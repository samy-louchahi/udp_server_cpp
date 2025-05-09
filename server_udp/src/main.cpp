#include <asio.hpp>
#include <thread>
#include <iostream>
#include <cstdlib>

#include "sudp/net/session.hpp"
#include "sudp/net/db_queue.hpp"
#include "sudp/db/db_thread.hpp"          // fonction db_thread(queue, uri)

namespace {

/* Lecture helpers */
uint16_t env_uint(const char* key, uint16_t def)
{
    const char* v = std::getenv(key);
    return v ? static_cast<uint16_t>(std::stoi(v)) : def;
}
std::string env_str(const char* key, const char* def)
{
    const char* v = std::getenv(key);
    return v ? v : def;
}

} // anon

int main()
{
    const uint16_t port  = env_uint("UDP_PORT", 48484);
    const uint16_t mtu   = env_uint("UDP_MTU",  1300);
    const std::string pg = env_str("PGURI", "postgresql://postgres:postgres@localhost/maps");

    std::cout << "[main] UDP port " << port << ", MTU " << mtu << "\n";

    /* --------------- Asio ---------------- */
    asio::io_context io;

    asio::ip::udp::socket sock(io,
        asio::ip::udp::endpoint(asio::ip::udp::v4(), port));

    sudp::net::DbQueue queue;

    sudp::net::Session session(std::move(sock), queue, mtu);

    /* --------------- thread DB ----------- */
    std::thread db_thr([&]{
        sudp::db::db_thread(queue, pg);
    });

    io.run();          // boucle réseau
    db_thr.join();     // jamais atteint (Ctrl‑C pour quitter)
    return 0;
}
