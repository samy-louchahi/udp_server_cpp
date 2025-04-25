#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "map/spatial_point.hpp"
#include "map/db_client.hpp"

namespace map_server {

class InsertBuffer {
public:
    InsertBuffer(DBClient& db, size_t batch_size = 1000, size_t flush_interval_ms = 1000);
    ~InsertBuffer();

    void add(const SpatialPoint& point);

private:
    void flusherThread();

    std::vector<SpatialPoint> buffer_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread flush_thread_;
    std::atomic<bool> running_;

    DBClient& db_;
    size_t batch_size_;
    size_t flush_interval_ms_;
};

}