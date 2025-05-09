#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>

namespace sudp::net {

/** Structure transmise du thread network au thread DB. */
struct DbItem
{
    uint8_t                 drone_id;
    uint32_t                seq;
    uint8_t                 flags;
    std::vector<uint8_t>    payload;   ///< Octomap binaire (blob)
};

/**
 * File SPSC (ou MPSC simple) thread‑safe basée sur std::queue.
 *  • push ()  : producteur (réseau)
 *  • pop()    : consommateur (DB) – retourne std::optional
 */
class DbQueue
{
public:
    void push(DbItem&& it)
    {
        {
            std::lock_guard<std::mutex> lk(m_);
            q_.emplace(std::move(it));
        }
        cv_.notify_one();
    }

    /** pop bloquant (attend si vide) */
    DbItem pop_blocking()
    {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]{ return !q_.empty(); });
        DbItem it = std::move(q_.front());
        q_.pop();
        return it;
    }

    /** pop non‑bloquant : retourne std::nullopt si vide. */
    std::optional<DbItem> try_pop()
    {
        std::lock_guard<std::mutex> lk(m_);
        if(q_.empty()) return std::nullopt;
        DbItem it = std::move(q_.front());
        q_.pop();
        return it;
    }

private:
    std::queue<DbItem>      q_;
    std::mutex              m_;
    std::condition_variable cv_;
};

} // namespace sudp::net
