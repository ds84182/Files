#include "texture.hpp"

#include <algorithm>

namespace GFX {

// Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 morton_interleave(u32 x, u32 y) {
	u32 i = (x & 7) | ((y & 7) << 8); // ---- -210
	i = (i ^ (i << 2)) & 0x1313;      // ---2 --10
	i = (i ^ (i << 1)) & 0x1515;      // ---2 -1-0
	i = (i | (i >> 7)) & 0x3F;
	return i;
}

//Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 get_morton_offset(u32 x, u32 y, u32 bytes_per_pixel) {
    u32 i = morton_interleave(x, y);
    unsigned int offset = (x & ~7) * 8;
    return (i + offset) * bytes_per_pixel;
}

void Texture::copyAndTile(u32 *data, u32 width, u32 height) {
    memset(tex->data, 0, tex->size);
    for (u32 x = 0; x<width; x++) {
        for (u32 y = 0; y<height; y++) {
            u32 p2y = (p2Height - 1 - y);
            u32 coarse_y = p2y & ~7;
		    u32 offset = get_morton_offset(x, p2y, 4) + coarse_y * p2Width * 4;
            GFX::Color pixel = data[x + y * width];
            std::swap(pixel.r, pixel.a);
            std::swap(pixel.b, pixel.g);
            *reinterpret_cast<u32*>(reinterpret_cast<u8*>(tex->data) + offset) = pixel.raw;
        }
    }
}

}
