#include "sudp/db/pg_pipeline.hpp"
#include "sudp/net/db_queue.hpp"

#include <pqxx/pqxx>
#include <thread>

namespace sudp::db {

void db_thread(DbQueue& q, const std::string& uri)
{
    pqxx::connection conn(uri);
    PgPipeline pipe(conn);

    while(true)
    {
        auto item = q.pop();       // bloquant ou wait‑free
        if(!item) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; }

        pipe.push(item->drone_id,
                  item->seq,
                  item->flags,
                  item->payload);
    }
}

} // namespace
