#ifndef MAP_MORTON_HPP
#define MAP_MORTON_HPP

#include <cstdint>

namespace map_server {

inline int32_t decode_morton_3d_x(uint32_t code) {
    int32_t x = 0;
    for (uint32_t i = 0; i < 10; ++i) {
        x |= ((code >> (3 * i)) & 1) << i;
    }
    return x;
}

inline int32_t decode_morton_3d_y(uint32_t code) {
    int32_t y = 0;
    for (uint32_t i = 0; i < 10; ++i) {
        y |= ((code >> (3 * i + 1)) & 1) << i;
    }
    return y;
}

inline int32_t decode_morton_3d_z(uint32_t code) {
    int32_t z = 0;
    for (uint32_t i = 0; i < 10; ++i) {
        z |= ((code >> (3 * i + 2)) & 1) << i;
    }
    return z;
}

} // namespace map_server

#endif // MAP_MORTON_HPP

