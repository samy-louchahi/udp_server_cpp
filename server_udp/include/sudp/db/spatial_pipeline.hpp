#pragma once
#include <pqxx/pqxx>
#include <vector>
#include <cstdint>

namespace sudp::db {

struct PointRGB
{
    float   x,y,z;
    uint8_t r,g,b,a;
    int64_t ts;           // epoch‑ms
};

class SpatialPipeline       /* COPY  →  spatial_point */
{
public:
    SpatialPipeline(pqxx::connection& c, std::size_t batch=10'000)
        : conn_(c), batch_(batch) {}

    void push(PointRGB&& p)
    {
        rows_.emplace_back(std::move(p));
        if(rows_.size() >= batch_) flush();
    }
    ~SpatialPipeline() { flush(); }

private:
    void flush()
    {
        if(rows_.empty()) return;

        pqxx::work tx(conn_);
        pqxx::stream_to s(tx,
          "COPY spatial_point (x,y,z,color_r,color_g,color_b,color_a,timestamp,nb_records) FROM STDIN");
        for(const auto& p: rows_)
        s << std::make_tuple(p.x, p.y, p.z,
            int(p.r), int(p.g), int(p.b), int(p.a),
            pqxx::to_string(p.ts), 1);
        s.complete();
        tx.exec("UPDATE spatial_point "
                "SET nb_records = spatial_point.nb_records+EXCLUDED.nb_records,"
                "    color_r = EXCLUDED.color_r,"
                "    color_g = EXCLUDED.color_g,"
                "    color_b = EXCLUDED.color_b,"
                "    color_a = EXCLUDED.color_a,"
                "    timestamp = EXCLUDED.timestamp "
                "FROM EXCLUDED "
                "WHERE spatial_point.x=EXCLUDED.x "
                "  AND spatial_point.y=EXCLUDED.y "
                "  AND spatial_point.z=EXCLUDED.z");
        tx.commit();
        rows_.clear();
    }
    pqxx::connection&        conn_;
    std::size_t              batch_;
    std::vector<PointRGB>    rows_;
};

} // namespace
