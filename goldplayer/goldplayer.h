#pragma once
// =====================================================================
//  goldplayer.h - En-tete principal de l'application GoldPlayer (PS4)
// =====================================================================
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <proto-include.h>
#include <freetype/freetype.h>

#include <orbis/Sysmodule.h>

#include "types.h"
#include "m3u.h"
#include "sfo.h"

// ---------------------------------------------------------------------
//  Constantes
// ---------------------------------------------------------------------
#define FRAME_W  1920
#define FRAME_H  1080

#define APP_VERSION   "1.00"
#define APP_TITLE_ID  "GOLD00001"
#define APP_CONTENT_ID "IV0000-GOLD00001_00-GOLDPLAYER000000"

// ---------------------------------------------------------------------
//  Ecrans
// ---------------------------------------------------------------------
enum ScreenId {
    SCR_HOME = 0,
    SCR_MEDIA,
    SCR_IPTV,
    SCR_GAMES,
    SCR_SETTINGS,
    SCR_ABOUT,
    SCR_PLAYER
};

// ---------------------------------------------------------------------
//  Etat global de l'application
// ---------------------------------------------------------------------
struct AppState {
    // SDL
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Surface*  surface;
    SDL_Joystick* pad;

    // Navigation
    ScreenId screen;
    int cursor;
    int scroll;
    int iptvView;              // 0 = playlists, 1 = chaines
    std::string currentPath;   // explorateur de fichiers (vide = racines)
    ScreenId returnScreen;     // ecran de retour apres le lecteur

    // Ecran "lecteur" (infos)
    std::string playerTitle;
    std::string playerSub;
    std::string playerInfo;

    // Donnees
    std::vector<MenuItem>      items;      // liste generique courante
    std::vector<IptvChannel>   channels;   // chaines de la playlist chargee
    std::vector<PlaylistEntry> playlists;  // playlists trouvees
    std::vector<GameEntry>     games;      // jeux installes
    std::vector<MediaEntry>    media;      // entrees de l'explorateur

    // Banniere transitoire
    std::string message;
    int messageTimer;
};

// ---------------------------------------------------------------------
//  Cycle de vie + boucle (main.cpp)
// ---------------------------------------------------------------------
bool appInit(AppState& a);
void appQuit(AppState& a);

// ---------------------------------------------------------------------
//  Texte (text.cpp) - FreeType
// ---------------------------------------------------------------------
bool textInit();
void textDraw(SDL_Renderer* r, int x, int y, const char* txt,
              uint8_t cr, uint8_t cg, uint8_t cb, int size, bool bold = false);
int  textWidth(const char* txt, int size);

// ---------------------------------------------------------------------
//  UI (ui.cpp)
// ---------------------------------------------------------------------
void fillRect(SDL_Renderer* r, int x, int y, int w, int h,
              uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca = 255);
void drawBorder(SDL_Renderer* r, int x, int y, int w, int h, int t,
                uint8_t cr, uint8_t cg, uint8_t cb);
void uiHeader(SDL_Renderer* r, AppState& a, const char* title);
void uiFooter(SDL_Renderer* r, AppState& a);
void uiList(SDL_Renderer* r, AppState& a, const char* emptyMsg);
void uiPanel(SDL_Renderer* r, int x, int y, int w, int h, bool gold);

// ---------------------------------------------------------------------
//  Explorateur de fichiers (files.cpp)
// ---------------------------------------------------------------------
std::vector<MediaEntry> browseDir(const std::string& path);
std::vector<PlaylistEntry> findPlaylists();
std::string mediaKind(const std::string& name);
bool dirExists(const std::string& path);

// ---------------------------------------------------------------------
//  Jeux (games.cpp)
// ---------------------------------------------------------------------
std::vector<GameEntry> listGames();
bool launchGame(const std::string& titleId);

// ---------------------------------------------------------------------
//  Ecrans (screens.cpp)
// ---------------------------------------------------------------------
void enterScreen(AppState& a, ScreenId s);
void refreshCurrent(AppState& a);
void drawCurrent(AppState& a);
void onConfirm(AppState& a);
void onBack(AppState& a);
