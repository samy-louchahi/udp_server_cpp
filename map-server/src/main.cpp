#include "map/db_client.hpp"
#include "map/insert_buffer.hpp"
#include "map/udp_receiver.hpp" 

int main() {
    map_server::DBClient db("dbname=mydb user=user password=password host=db port=5432");

    map_server::InsertBuffer buffer(db, 1000, 1000);  // 1000 points ou 1 seconde
    map_server::UdpReceiver receiver(9000);
    receiver.start();

    while (true) {
        auto points = receiver.receive_packet();
        for (const auto& p : points) {
            buffer.add(p);
        }
    }

    return 0;
}

