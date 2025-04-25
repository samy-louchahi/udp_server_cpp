#include "map/db_client.hpp"
#include <iostream>

namespace map_server {

DBClient::DBClient(const std::string& conn_str) {
    try {
        conn_ = new pqxx::connection(conn_str);
        if (conn_->is_open()) {
            std::cout << "[DBClient] Connected to database: " << conn_->dbname() << std::endl;
        } else {
            throw std::runtime_error("Failed to open PostgreSQL connection.");
        }
    } catch (const std::exception& e) {
        std::cerr << "[DBClient] Connection error: " << e.what() << std::endl;
        throw;
    }
}

DBClient::~DBClient() {
    if (conn_) {
        conn_->disconnect();
        delete conn_;
        std::cout << "[DBClient] Disconnected from database." << std::endl;
    }
}

void DBClient::insertBatch(const std::vector<SpatialPoint>& points) {
    if (!conn_ || !conn_->is_open()) return;

    try {
        pqxx::work txn(*conn_);

        std::string query = "INSERT INTO spatial_point "
            "(x, y, z, color_r, color_g, color_b, color_a, timestamp, nb_records) VALUES ";

        std::string values;
        for (size_t i = 0; i < points.size(); ++i) {
            int32_t x, y, z;
            values += "(" +
                      txn.quote(x) + "," +
                      txn.quote(y) + "," +
                      txn.quote(z) + "," +
                      txn.quote(static_cast<int>(points[i].color.r)) + "," +
                      txn.quote(static_cast<int>(points[i].color.g)) + "," +
                      txn.quote(static_cast<int>(points[i].color.b)) + "," +
                      txn.quote(static_cast<int>(points[i].color.a))+ "," +
                      txn.quote(static_cast<int>(points[i].timestamp)) + "," +
                      "1)";

            if (i < points.size() - 1) values += ",";
        }

        query += values + " ON CONFLICT (x, y, z) DO UPDATE SET "
                         "color_r = EXCLUDED.color_r, "
                         "color_g = EXCLUDED.color_g, "
                         "color_b = EXCLUDED.color_b, "
                         "color_a = EXCLUDED.color_a, "
                         "timestamp = EXCLUDED.timestamp, "
                         "nb_records = spatial_point.nb_records + 1;";

        txn.exec(query);
        txn.commit();

        std::cout << "[DBClient] Inserted " << points.size() << " points." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[DBClient] Insert error: " << e.what() << std::endl;
    }
}

}