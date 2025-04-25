#pragma once

#include <pqxx/pqxx>
#include <vector>
#include "map/spatial_point.hpp"

namespace map_server {

class DBClient {
public:
    DBClient(const std::string& conn_str);
    ~DBClient();

    void insertBatch(const std::vector<SpatialPoint>& points);

private:
    pqxx::connection* conn_;
};

}

