cmake_minimum_required(VERSION 3.16)
project(udp_server_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(asio REQUIRED)
find_package(octomap REQUIRED)       
find_package(ZLIB REQUIRED)        
find_package(pqxx REQUIRED)

include_directories(
  include
  ${OCTOMAP_INCLUDE_DIRS}
)

add_library(core
  src/net/session.cpp
  src/db/db_thread.cpp
  # ... autres fichiers
)

target_link_libraries(core
  asio::asio
  pqxx::pqxx
  octomap         
  ZLIB::ZLIB     
)

add_executable(server_udp src/main.cpp)
target_link_libraries(server_udp core)
