#ifndef MAP_SPATIAL_POINT_HPP
#define MAP_SPATIAL_POINT_HPP

#include <cstdint>
#include <string>
#include "map/morton.hpp"

namespace map_server {

struct ColorRGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct SpatialPoint {
    int32_t x;
    int32_t y;
    int32_t z;
    ColorRGBA color;
    float timestamp;

    SpatialPoint() = default;

    SpatialPoint(int32_t x_, int32_t y_, int32_t z_, ColorRGBA color_, float timestamp_)
        : x(x_), y(y_), z(z_), color(color_), timestamp(timestamp_) {}

    std::string to_string() const {
        return "[X: " + std::to_string(x) + ", Y: " + std::to_string(y) + ", Z: " + std::to_string(z) +
               ", R: " + std::to_string(color.r) + ", G: " + std::to_string(color.g) +
               ", B: " + std::to_string(color.b) + ", A: " + std::to_string(color.a) +
               ", Timestamp: " + std::to_string(timestamp) + "]";
    }

    void decode_coordinate(uint32_t morton_code, int32_t &out_x, int32_t &out_y, int32_t &out_z) const {
        out_x = decode_morton_3d_x(morton_code);
        out_y = decode_morton_3d_y(morton_code);
        out_z = decode_morton_3d_z(morton_code);
    }
};

} // namespace map_server

#endif // MAP_SPATIAL_POINT_HPP