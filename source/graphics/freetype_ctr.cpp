#include "freetype_ctr.hpp"

#include "attribute.hpp"

namespace GFX {

static unsigned int next_pow2(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v+1;
}

Glyph::Glyph(FT_Face face, char32_t c, float baseline) {
    FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_DEFAULT);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    this->baseline = baseline;

    unsigned int initialWidth, initialHeight;
    initialWidth = width = face->glyph->bitmap.width;
    initialHeight = height = face->glyph->bitmap.rows;

    width = next_pow2(width);
    if (width < 64) width = 64;
    height = next_pow2(height);
    if (height < 64) height = 64;

    advanceX = face->glyph->advance.x >> 6;
    bitmapLeft = face->glyph->bitmap_left;
    bitmapTop = face->glyph->bitmap_top;

    size_t size = width*height*4;
    u32* buffer = reinterpret_cast<u32*>(linearAlloc(size));

    u8 *read = face->glyph->bitmap.buffer;
    u32 *write = buffer;

    memset(buffer, 0, size);
    for (unsigned int y=0; y<initialHeight; y++) {
        for (unsigned int x=0; x<initialWidth; x++) {
            u8 v = *read;
            *write = 0xFFFFFF00 | v;
            read++;
            write++;
        }
        write = buffer+(width*(y+1));
    }
    GSPGPU_FlushDataCache(buffer, size);

    memset(&texture, 0, sizeof(C3D_Tex));
    if (!C3D_TexInit(&texture, width, height, GPU_RGBA8)) abort();

    const u32 flags = (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

    C3D_SafeDisplayTransfer(
        buffer,
        GX_BUFFER_DIM(width, height),
        reinterpret_cast<u32*>(texture.data),
        GX_BUFFER_DIM(width, height),
        flags
    );
    gspWaitForPPF();

    linearFree(buffer);
}

Glyph::~Glyph() {
    C3D_TexDelete(&texture);
}

static void *vbodata = nullptr;

static const float vertex_list[] = {
    0.0f, 0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.5f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.5f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
};

static C3D_BufInfo vertexbuffer;
static bool glyphVertexInit = false;

void Glyph::render(int x, int y) const {
    if (!glyphVertexInit) {
        // Create the VBO (vertex buffer object)
        vbodata = linearAlloc(sizeof(vertex_list));
        memcpy(vbodata, vertex_list, sizeof(vertex_list));

        BufInfo_Init(&vertexbuffer);
        BufInfo_Add(&vertexbuffer, vbodata, sizeof(float)*5, 2, 0x20);

        glyphVertexInit = true;
    }

    C3D_Mtx* mtx = GFX::PushMatrix();

    // bitmapLeft and bitmapTop are unsigned, so that makes the ENTIRE expression unsigned
    // it makes no sense because floats are also signed... so...
    Mtx_Translate(mtx, x+(int)bitmapLeft, (y-(int)bitmapTop)+baseline, 0.0f);
    //Mtx_Translate(mtx, x+bitmapLeft, y-bitmapTop, 0);
    Mtx_Scale(mtx, width, height, 1);

    GFX::UpdateMatrix();

    C3D_FVUnifSet(GPU_VERTEX_SHADER, textureScaleUniform, 1, 1, 1, 1);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, textureOffsetUniform, 0, 0, 0, 0);

    // C3D_TexBind does not modify C3D_Tex, but it's not const in the method signature
    C3D_TexBind(0, const_cast<C3D_Tex*>(&texture));
    GFX::Attr::PositionFixedColorTexture.use();
    C3D_SetBufInfo(&vertexbuffer);
    C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);

    GFX::PopMatrix();
}

static FT_Library library;
static bool ftinit = false;

Font::Font(const u8 *data, size_t size, int fontSize) {
    init(data, size, fontSize);
}

Font::~Font() {
    if (face) {
        FT_Done_Face(face);
    }
}

void Font::init(const u8 *data, size_t size, int fontSize) {
    if (!ftinit) {
        FT_Init_FreeType(&library);
        ftinit = true;
    }

    FT_New_Memory_Face(library, data, size, 0, &face);
    //FT_Set_Char_Size(face, 0, fontSize << 6, 96, 96);
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    calcHeight = face->size->metrics.height;
    calcHeight >>= 6;
    // 1.25 is magic line height for true type fonts
    // Thanks Love2D
    baseline = floorf(calcHeight / 1.25f + 0.5f);
}

const Glyph &Font::getGlyph(char32_t c) {
    if (!glyphs.count(c)) {
        glyphs.emplace(std::piecewise_construct,
            std::make_tuple(c),
            std::make_tuple(face, c, baseline));
    }

    return glyphs.at(c);
}

void Font::drawText(const std::wstring &str, int x, int y, const GFX::Color &color) {
    C3D_FixedAttribSet(1, color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0);
    GFX::SetFragMode(GFX::FragMode::ModulateTexture);

    int baseX = x;

    for (unsigned int i=0; i<str.length(); i++) {
        if (str[i] == L'\n') {
            y += baseline;
            x = baseX;
            continue;
        }

        auto &glyph = getGlyph(str[i]);

        glyph.render(x, y);

        x += glyph.advanceX;
    }
}

int Font::width(const std::wstring &str) {
    int width = 0;
    for (unsigned int i=0; i<str.length(); i++) {
        auto &glyph = getGlyph(str[i]);
        width += glyph.advanceX;
    }
    return width;
}

}
