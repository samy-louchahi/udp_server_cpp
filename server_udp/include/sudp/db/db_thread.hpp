#pragma once
#include <string>

#include "sudp/net/db_queue.hpp"   // DbQueue declaration

namespace sudp::db {

/**
 * Boucle consommateur : lit en continu les lots de points (PointBatch)
 * depuis la file `queue` et les insère dans PostgreSQL via SpatialPipeline.
 *
 * @param queue  File partagée « réseau → BD »
 * @param pg_uri Chaîne de connexion, ex. "postgresql://user:pass@host/db"
 */
void db_thread(sudp::net::DbQueue& queue, const std::string& pg_uri);

} // namespace sudp::db
