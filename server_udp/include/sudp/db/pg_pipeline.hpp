#pragma once
#include <pqxx/pqxx>
#include <vector>
#include <string>

namespace sudp::db {

/**
 * Insère les blobs binaires dans la table scans_binary
 *   (drone_id, seq, flags, data  BYTEA)
 */
class PgPipeline
{
public:
    explicit PgPipeline(pqxx::connection& c) : conn_(c) {}

    void push(uint8_t drone_id,
              uint32_t seq,
              uint8_t flags,
              const std::vector<uint8_t>& blob)
    {
        rows_.emplace_back(drone_id, seq, flags,
                           pqxx::binarystring(blob.data(), blob.size()));
        flush_if_needed();
    }

    ~PgPipeline() { flush(true); }

private:
    using row_t = std::tuple<uint8_t, uint32_t, uint8_t, pqxx::binarystring>;

    void flush_if_needed()
    {
        if(rows_.size() >= 2048) flush(false);
    }

    void flush(bool force)
    {
        if(rows_.empty() && !force) return;

        pqxx::work tx(conn_);
        pqxx::stream_to s(tx,
          "COPY scans_binary (drone_id, seq, flags, data) FROM STDIN BINARY");
        for(auto& r : rows_) s << r;
        s.complete();
        tx.commit();
        rows_.clear();
    }

    pqxx::connection&      conn_;
    std::vector<row_t>     rows_;
};

} // namespace sudp::db
