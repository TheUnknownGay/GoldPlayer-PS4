// =====================================================================
//  text.cpp - Rendu de texte FreeType (autonome, UTF-8, accents FR)
//  Surface ARGB8888 : Amask=0xFF000000 Rmask=0x00FF0000
//                     Gmask=0x0000FF00 Bmask=0x000000FF
// =====================================================================
#include "goldplayer.h"

static FT_Library g_ft = 0;
static FT_Face    g_face = 0;
static FT_Face    g_faceBold = 0;

bool textInit() {
    if (FT_Init_FreeType(&g_ft) != 0) {
        return false;
    }
    if (FT_New_Face(g_ft, "/app0/assets/fonts/Roboto-Regular.ttf", 0, &g_face) != 0) {
        return false;
    }
    if (FT_New_Face(g_ft, "/app0/assets/fonts/Roboto-Bold.ttf", 0, &g_faceBold) != 0) {
        g_faceBold = g_face; // repli sur la police reguliere
    }
    return true;
}

// Decode le prochain point de code UTF-8 (avec bornes).
static const char* utf8Next(const char* p, const char* end, uint32_t& cp) {
    const unsigned char* s = (const unsigned char*)p;
    unsigned char c = s[0];
    if (c < 0x80) { cp = c; return p + 1; }
    if ((c & 0xE0) == 0xC0 && p + 2 <= end) {
        cp = ((uint32_t)(c & 0x1F) << 6) | (s[1] & 0x3F);
        return p + 2;
    }
    if ((c & 0xF0) == 0xE0 && p + 3 <= end) {
        cp = ((uint32_t)(c & 0x0F) << 12) | ((uint32_t)(s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        return p + 3;
    }
    if ((c & 0xF8) == 0xF0 && p + 4 <= end) {
        cp = ((uint32_t)(c & 0x07) << 18) | ((uint32_t)(s[1] & 0x3F) << 12) |
             ((uint32_t)(s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        return p + 4;
    }
    cp = c;
    return p + 1;
}

static SDL_Surface* renderText(FT_Face face, const char* txt,
                               uint8_t cr, uint8_t cg, uint8_t cb) {
    if (!face || !txt) return NULL;
    const char* end = txt + strlen(txt);

    // Mesure de la largeur totale.
    int w = 0;
    for (const char* p = txt; p < end; ) {
        uint32_t cp = 0;
        p = utf8Next(p, end, cp);
        FT_UInt gi = FT_Get_Char_Index(face, cp);
        if (FT_Load_Glyph(face, gi, FT_LOAD_DEFAULT) == 0) {
            w += face->glyph->advance.x >> 6;
        }
    }

    int asc  = face->size->metrics.ascender >> 6;
    int desc = face->size->metrics.descender >> 6;
    int h = asc - desc + 2;
    if (w < 2) w = 2;
    if (h < 2) h = 2;

    SDL_Surface* s = SDL_CreateRGBSurface(0, w, h, 32,
                                          0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!s) return NULL;
    SDL_memset(s->pixels, 0, (size_t)s->pitch * (size_t)s->h);

    int penX = 0;
    for (const char* p = txt; p < end; ) {
        uint32_t cp = 0;
        p = utf8Next(p, end, cp);
        FT_UInt gi = FT_Get_Char_Index(face, cp);
        if (FT_Load_Glyph(face, gi, FT_LOAD_DEFAULT) != 0) continue;
        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) continue;

        FT_GlyphSlot slot = face->glyph;
        int left = penX + (slot->metrics.horiBearingX >> 6);
        int top  = asc - slot->bitmap_top;

        for (int yy = 0; yy < (int)slot->bitmap.rows; yy++) {
            for (int xx = 0; xx < (int)slot->bitmap.width; xx++) {
                unsigned char px = slot->bitmap.buffer[yy * slot->bitmap.width + xx];
                if (px == 0) continue;
                int X = left + xx;
                int Y = top + yy;
                if (X < 0 || Y < 0 || X >= w || Y >= h) continue;
                uint32_t* row = (uint32_t*)((uint8_t*)s->pixels + (size_t)Y * (size_t)s->pitch);
                row[X] = ((uint32_t)px << 24) | ((uint32_t)cr << 16) |
                         ((uint32_t)cg << 8) | (uint32_t)cb;
            }
        }
        penX += slot->advance.x >> 6;
    }
    return s;
}

void textDraw(SDL_Renderer* r, int x, int y, const char* txt,
              uint8_t cr, uint8_t cg, uint8_t cb, int size, bool bold) {
    if (!r || !txt || !txt[0]) return;
    FT_Face face = (bold && g_faceBold) ? g_faceBold : g_face;
    if (!face) return;
    FT_Set_Pixel_Sizes(face, 0, size);

    SDL_Surface* s = renderText(face, txt, cr, cg, cb);
    if (!s) return;

    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s);
    if (t) {
        SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
        SDL_Rect dst = { x, y, s->w, s->h };
        SDL_RenderCopy(r, t, NULL, &dst);
        SDL_DestroyTexture(t);
    }
    SDL_FreeSurface(s);
}

int textWidth(const char* txt, int size) {
    if (!g_face || !txt) return 0;
    FT_Set_Pixel_Sizes(g_face, 0, size);
    const char* end = txt + strlen(txt);
    int w = 0;
    for (const char* p = txt; p < end; ) {
        uint32_t cp = 0;
        p = utf8Next(p, end, cp);
        FT_UInt gi = FT_Get_Char_Index(g_face, cp);
        if (FT_Load_Glyph(g_face, gi, FT_LOAD_DEFAULT) == 0) {
            w += g_face->glyph->advance.x >> 6;
        }
    }
    return w;
}
