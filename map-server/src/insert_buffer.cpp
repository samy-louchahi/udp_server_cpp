#include "map/insert_buffer.hpp"
#include <chrono>

namespace map_server {

InsertBuffer::InsertBuffer(DBClient& db, size_t batch_size, size_t flush_interval_ms)
    : db_(db),
      batch_size_(batch_size),
      flush_interval_ms_(flush_interval_ms),
      running_(true),
      flush_thread_(&InsertBuffer::flusherThread, this)
{}

InsertBuffer::~InsertBuffer() {
    running_ = false;
    cv_.notify_all();
    if (flush_thread_.joinable()) {
        flush_thread_.join();
    }

    // Final flush
    std::lock_guard<std::mutex> lock(mutex_);
    if (!buffer_.empty()) {
        db_.insertBatch(buffer_);
    }
}

void InsertBuffer::add(const SpatialPoint& point) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(point);

    if (buffer_.size() >= batch_size_) {
        cv_.notify_one(); 
    }
}

void InsertBuffer::flusherThread() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::milliseconds(flush_interval_ms_), [&] {
            return buffer_.size() >= batch_size_ || !running_;
        });

        if (!buffer_.empty()) {
            std::vector<SpatialPoint> to_flush;
            to_flush.swap(buffer_);
            lock.unlock();  
            db_.insertBatch(to_flush);
        }
    }
}

}
