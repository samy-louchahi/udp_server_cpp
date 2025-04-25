cmake_minimum_required(VERSION 3.10)
project(udp-server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES src/*.cpp)
add_executable(udp_server ${SOURCES})
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

target_include_directories(udp_server PRIVATE ${PQXX_INCLUDE_DIRS})
target_link_libraries(udp_server ${PQXX_LIBRARIES})