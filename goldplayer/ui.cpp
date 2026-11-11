// =====================================================================
//  ui.cpp - Primitives de dessin, en-tete, pied de page et listes
// =====================================================================
#include "goldplayer.h"

// Palette
#define C_BG_R   11
#define C_BG_G   14
#define C_BG_B   20
#define C_HD_R   18
#define C_HD_G   24
#define C_HD_B   34
#define C_PN_R   20
#define C_PN_G   27
#define C_PN_B   40
#define C_IT_R   22
#define C_IT_G   29
#define C_IT_B   42
#define C_BD_R   35
#define C_BD_G   45
#define C_BD_B   65
#define C_GD_R   234
#define C_GD_G   179
#define C_GD_B   8
#define C_GD2_R  245
#define C_GD2_G  197
#define C_GD2_B  66
#define C_TX_R   255
#define C_TX_G   255
#define C_TX_B   255
#define C_MU_R   160
#define C_MU_G   175
#define C_MU_B   195

#define HEADER_H    80
#define FOOTER_Y    1010
#define CONTENT_TOP 116
#define LIST_X      80
#define LIST_W      1760
#define ITEM_H      84
#define ITEM_GAP    12

void fillRect(SDL_Renderer* r, int x, int y, int w, int h,
              uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca) {
    SDL_SetRenderDrawColor(r, cr, cg, cb, ca);
    SDL_Rect q = { x, y, w, h };
    SDL_RenderFillRect(r, &q);
}

void drawBorder(SDL_Renderer* r, int x, int y, int w, int h, int t,
                uint8_t cr, uint8_t cg, uint8_t cb) {
    fillRect(r, x, y, w, t, cr, cg, cb);
    fillRect(r, x, y + h - t, w, t, cr, cg, cb);
    fillRect(r, x, y, t, h, cr, cg, cb);
    fillRect(r, x + w - t, y, t, h, cr, cg, cb);
}

void uiPanel(SDL_Renderer* r, int x, int y, int w, int h, bool gold) {
    fillRect(r, x, y, w, h, C_PN_R, C_PN_G, C_PN_B);
    if (gold) {
        drawBorder(r, x, y, w, h, 2, C_GD_R, C_GD_G, C_GD_B);
    } else {
        drawBorder(r, x, y, w, h, 1, C_BD_R, C_BD_G, C_BD_B);
    }
}

void uiHeader(SDL_Renderer* r, AppState& a, const char* title) {
    (void)a;
    fillRect(r, 0, 0, FRAME_W, HEADER_H, C_HD_R, C_HD_G, C_HD_B);
    drawBorder(r, 0, HEADER_H - 2, FRAME_W, 2, 2, C_BD_R, C_BD_G, C_BD_B);

    // Logo "G" dore
    fillRect(r, 60, 20, 40, 40, C_GD_R, C_GD_G, C_GD_B);
    textDraw(r, 72, 28, "G", 12, 12, 14, 26, true);

    textDraw(r, 118, 14, "GOLDPLAYER", C_GD2_R, C_GD2_G, C_GD2_B, 34, true);
    textDraw(r, 118, 52, "MEDIA & IPTV SUITE - PS4", C_MU_R, C_MU_G, C_MU_B, 16);

    // Titre de l'ecran courant (centre)
    if (title && title[0]) {
        int tw = textWidth(title, 34);
        textDraw(r, (FRAME_W - tw) / 2, 24, title, C_TX_R, C_TX_G, C_TX_B, 34, true);
    }

    // Badge statut a droite
    fillRect(r, 1680, 24, 180, 32, 20, 27, 40);
    drawBorder(r, 1680, 24, 180, 32, 1, 40, 180, 90);
    textDraw(r, 1694, 32, "GOLDHEN READY", 40, 220, 100, 16);
}

void uiFooter(SDL_Renderer* r, AppState& a) {
    (void)a;
    fillRect(r, 0, FOOTER_Y, FRAME_W, FRAME_H - FOOTER_Y, 14, 18, 26);
    drawBorder(r, 0, FOOTER_Y, FRAME_W, 2, 2, C_BD_R, C_BD_G, C_BD_B);

    // Croix = valider
    fillRect(r, 80, 1030, 34, 34, 40, 120, 230);
    textDraw(r, 90, 1037, "X", 255, 255, 255, 20, true);
    textDraw(r, 128, 1038, "Valider / Lancer", C_MU_R, C_MU_G, C_MU_B, 20);

    // Rond = retour
    fillRect(r, 360, 1030, 34, 34, 230, 50, 50);
    textDraw(r, 370, 1037, "O", 255, 255, 255, 20, true);
    textDraw(r, 408, 1038, "Retour", C_MU_R, C_MU_G, C_MU_B, 20);

    // D-PAD = naviguer
    fillRect(r, 560, 1030, 96, 34, 50, 65, 85);
    textDraw(r, 572, 1037, "D-PAD", C_GD2_R, C_GD2_G, C_GD2_B, 18, true);
    textDraw(r, 672, 1038, "Naviguer", C_MU_R, C_MU_G, C_MU_B, 20);

    // L1 / R1 = page
    fillRect(r, 820, 1030, 44, 34, 50, 65, 85);
    textDraw(r, 830, 1037, "L1", C_GD2_R, C_GD2_G, C_GD2_B, 16, true);
    fillRect(r, 872, 1030, 44, 34, 50, 65, 85);
    textDraw(r, 882, 1037, "R1", C_GD2_R, C_GD2_G, C_GD2_B, 16, true);
    textDraw(r, 930, 1038, "Page +/-", C_MU_R, C_MU_G, C_MU_B, 20);

    textDraw(r, 1440, 1038, "Designer : DJAFFAR MOURTADHOI", C_GD_R, C_GD_G, C_GD_B, 20);
}

// Rendu d'une liste generique (a.items) avec curseur + defilement.
void uiList(SDL_Renderer* r, AppState& a, const char* emptyMsg) {
    int n = (int)a.items.size();
    if (n == 0) {
        if (emptyMsg && emptyMsg[0]) {
            int tw = textWidth(emptyMsg, 30);
            textDraw(r, (FRAME_W - tw) / 2, 480, emptyMsg, C_MU_R, C_MU_G, C_MU_B, 30);
        }
        return;
    }

    int step = ITEM_H + ITEM_GAP;
    int visible = (FOOTER_Y - CONTENT_TOP) / step;

    if (a.cursor < 0) a.cursor = 0;
    if (a.cursor >= n) a.cursor = n - 1;
    if (a.scroll < 0) a.scroll = 0;
    if (a.scroll > a.cursor) a.scroll = a.cursor;
    if (a.scroll + visible - 1 < a.cursor) a.scroll = a.cursor - visible + 1;
    if (a.scroll + visible > n) a.scroll = n - visible;
    if (a.scroll < 0) a.scroll = 0;

    for (int i = 0; i < visible; i++) {
        int idx = a.scroll + i;
        if (idx >= n) break;
        int y = CONTENT_TOP + i * step;
        bool sel = (idx == a.cursor);

        if (sel) {
            fillRect(r, LIST_X, y, LIST_W, ITEM_H, C_GD_R, C_GD_G, C_GD_B);
            fillRect(r, LIST_X + 3, y + 3, LIST_W - 6, ITEM_H - 6, 28, 36, 52);
        } else {
            fillRect(r, LIST_X, y, LIST_W, ITEM_H, C_IT_R, C_IT_G, C_IT_B);
            drawBorder(r, LIST_X, y, LIST_W, ITEM_H, 1, C_BD_R, C_BD_G, C_BD_B);
        }

        int tx = LIST_X + 34;
        if (sel) {
            textDraw(r, tx, y + 12, a.items[idx].label.c_str(), 255, 255, 255, 32, true);
            if (!a.items[idx].sub.empty()) {
                textDraw(r, tx, y + 52, a.items[idx].sub.c_str(), C_GD2_R, C_GD2_G, C_GD2_B, 22);
            }
        } else {
            textDraw(r, tx, y + 12, a.items[idx].label.c_str(), C_TX_R, C_TX_G, C_TX_B, 30, false);
            if (!a.items[idx].sub.empty()) {
                textDraw(r, tx, y + 52, a.items[idx].sub.c_str(), C_MU_R, C_MU_G, C_MU_B, 22);
            }
        }
    }

    // Indicateur de defilement
    if (n > visible) {
        int total = FOOTER_Y - CONTENT_TOP;
        int th = total * visible / n;
        int ty = CONTENT_TOP + (total - th) * a.scroll / (n - visible);
        fillRect(r, FRAME_W - 18, CONTENT_TOP, 6, total, 14, 18, 26);
        fillRect(r, FRAME_W - 18, ty, 6, th, C_GD_R, C_GD_G, C_GD_B);
    }
}
