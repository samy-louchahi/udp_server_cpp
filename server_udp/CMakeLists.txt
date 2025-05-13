cmake_minimum_required(VERSION 3.16)
project(map_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ───── dépendances ──────────────────────────────────────────────
find_package(Threads  REQUIRED)
find_package(octomap  REQUIRED)
find_package(ZLIB     REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx) 

# ───── includes ────────────────────────────────────────────────
include_directories(
  include
  ${OCTOMAP_INCLUDE_DIRS}
  ${LIBPQXX_INCLUDE_DIRS}
)

add_library(core
  src/net/session.cpp
  src/net/db_queue.cpp
  src/db/db_thread.cpp
)

target_compile_definitions(core PRIVATE ASIO_STANDALONE)
target_link_libraries(core
  Threads::Threads
  ${LIBPQXX_LIBRARIES}       
  octomap
  ZLIB::ZLIB
)

add_executable(server_udp src/main.cpp)
target_link_libraries(server_udp core)
