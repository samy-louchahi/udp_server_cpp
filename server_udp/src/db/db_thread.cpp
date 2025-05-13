#include "sudp/db/spatial_pipeline.hpp"
#include "sudp/net/db_queue.hpp"

#include <pqxx/pqxx>
#include <thread>

namespace sudp::db {

void db_thread(sudp::net::DbQueue& q, const std::string& uri)
{
    pqxx::connection conn(uri);
    SpatialPipeline  pipe(conn);

    while (true)
    {
        if (auto batch = q.try_pop_batch())
            for (auto& p : batch->pts) pipe.push(std::move(p));
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

} // namespace sudp::db
