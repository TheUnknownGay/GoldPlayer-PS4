// =====================================================================
//  screens.cpp - Orchestration des ecrans (accueil, medias, IPTV,
//                jeux, parametres, a propos, lecteur)
// =====================================================================
#include "goldplayer.h"

// Palette locale (identiques a ui.cpp)
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
#define C_BD_R   35
#define C_BD_G   45
#define C_BD_B   65

static std::string parentPath(const std::string& p) {
    if (p == "/mnt/usb0" || p == "/mnt/usb1" || p == "/mnt/usb_ext0" || p == "/data")
        return "";
    size_t slash = p.rfind('/');
    if (slash == std::string::npos || slash == 0) return "";
    return p.substr(0, slash);
}

static void openPlayer(AppState& a, const std::string& title,
                       const std::string& sub, const std::string& info) {
    a.returnScreen = a.screen;
    a.playerTitle = title;
    a.playerSub = sub;
    a.playerInfo = info;
    a.screen = SCR_PLAYER;
    a.cursor = 0;
    a.scroll = 0;
}

static void showBanner(AppState& a, const std::string& msg) {
    a.message = msg;
    a.messageTimer = 160;
}

// ---------------------------------------------------------------------
//  Rafraichissement des listes
// ---------------------------------------------------------------------
void refreshCurrent(AppState& a) {
    a.items.clear();

    switch (a.screen) {
    case SCR_HOME: {
        a.items.push_back({"Médias (USB)", "Parcourir vos disques durs externes", "MEDIA"});
        a.items.push_back({"IPTV — Playlists M3U", "Chaînes en direct et playlists", "IPTV"});
        a.items.push_back({"Jeux installés", "Lister et lancer vos jeux PS4", "GAMES"});
        a.items.push_back({"Paramètres", "Informations du package", "SETTINGS"});
        a.items.push_back({"À propos", "Concepteur et crédits", "ABOUT"});
        break;
    }

    case SCR_MEDIA: {
        if (a.currentPath.empty()) {
            const char* roots[] = { "/mnt/usb0", "/mnt/usb1", "/mnt/usb_ext0", "/data" };
            const char* labels[] = {
                "Disque USB 1 (/mnt/usb0)",
                "Disque USB 2 (/mnt/usb1)",
                "Stockage étendu (/mnt/usb_ext0)",
                "Données de l'application (/data)"
            };
            for (int i = 0; i < 4; i++) {
                if (dirExists(roots[i])) {
                    a.items.push_back({labels[i], "Ouvrir ce stockage", roots[i]});
                }
            }
        } else {
            a.media = browseDir(a.currentPath);
            a.items.push_back({"..", "Revenir en arrière", "UP"});
            for (size_t i = 0; i < a.media.size(); i++) {
                const MediaEntry& m = a.media[i];
                std::string sub = m.isDir ? "Dossier" : ("Fichier " + m.kind);
                a.items.push_back({m.name, sub, m.path});
            }
        }
        break;
    }

    case SCR_IPTV: {
        if (a.iptvView == 0) {
            a.playlists = findPlaylists();
            for (size_t i = 0; i < a.playlists.size(); i++) {
                char sub[96];
                snprintf(sub, sizeof(sub), "%d chaîne(s) — %s",
                         a.playlists[i].channelCount, a.playlists[i].path.c_str());
                a.items.push_back({a.playlists[i].name, sub, a.playlists[i].path});
            }
        } else {
            for (size_t i = 0; i < a.channels.size(); i++) {
                const IptvChannel& c = a.channels[i];
                a.items.push_back({c.name, c.group.empty() ? "Chaîne IPTV" : c.group, c.url});
            }
        }
        break;
    }

    case SCR_GAMES: {
        a.games = listGames();
        for (size_t i = 0; i < a.games.size(); i++) {
            const GameEntry& g = a.games[i];
            a.items.push_back({g.name, g.titleId + " — " + g.location, g.titleId});
        }
        break;
    }

    case SCR_SETTINGS: {
        a.items.push_back({"Title ID", APP_TITLE_ID, ""});
        a.items.push_back({"Content ID", APP_CONTENT_ID, ""});
        a.items.push_back({"Version", APP_VERSION, ""});
        a.items.push_back({"Compatibilité", "PS4 FW 5.05 – 11.00 (GoldHEN 2.4+)", ""});
        a.items.push_back({"Moteur de rendu", "OpenOrbis v0.5.4 — SDL2 1080p", ""});
        a.items.push_back({"À propos du concepteur", "Voir les informations", "ABOUT"});
        break;
    }

    case SCR_ABOUT:
    case SCR_PLAYER:
    default:
        break;
    }
}

// ---------------------------------------------------------------------
//  Dessin des ecrans
// ---------------------------------------------------------------------
static void drawAbout(SDL_Renderer* r, AppState& a) {
    (void)a;
    int cx = 240, cy = 150, w = 1440, h = 640;
    uiPanel(r, cx, cy, w, h, true);

    textDraw(r, cx + 50, cy + 34, "GOLDPLAYER — À PROPOS", C_GD2_R, C_GD2_G, C_GD2_B, 40, true);
    textDraw(r, cx + 50, cy + 86, "Lecteur multimédia, gestionnaire IPTV & lanceur de jeux pour PS4 Jailbreak",
             C_MU_R, C_MU_G, C_MU_B, 22);

    // Colonne gauche : coordonnees du concepteur
    int lx = cx + 50, ly = cy + 160;
    uiPanel(r, lx, ly, 640, 360, false);
    textDraw(r, lx + 30, ly + 24, "COORDONNÉES DU CONCEPTEUR", C_GD_R, C_GD_G, C_GD_B, 24, true);

    struct { const char* k; const char* v; int y; } rows[] = {
        { "Designer",      "DJAFFAR MOURTADHOI",     60 },
        { "Réseau social", "@Mourtadhoi.Art",        110 },
        { "Téléphone",     "0693521495",             160 },
        { "Email",         "djaffar.art@gmail.com",  210 },
    };
    for (int i = 0; i < 4; i++) {
        textDraw(r, lx + 30, ly + rows[i].y, rows[i].k, C_MU_R, C_MU_G, C_MU_B, 26);
        textDraw(r, lx + 240, ly + rows[i].y, rows[i].v, C_TX_R, C_TX_G, C_TX_B, 26, true);
    }
    textDraw(r, lx + 30, ly + 290, "Design & architecture originale créés avec passion.",
             C_MU_R, C_MU_G, C_MU_B, 20);

    // Colonne droite : parametres du package
    int rx = lx + 680, ry = ly;
    uiPanel(r, rx, ry, 640, 360, false);
    textDraw(r, rx + 30, ry + 24, "PARAMÈTRES DU PACKAGE", 40, 180, 90, 24, true);

    struct { const char* k; const char* v; int y; } rows2[] = {
        { "Title ID",     APP_TITLE_ID,     60 },
        { "Content ID",   APP_CONTENT_ID,   110 },
        { "Version",      APP_VERSION,      160 },
        { "Compatibilité","FW 5.05 – 11.00", 210 },
        { "Moteur",       "OpenOrbis v0.5.4 / SDL2", 260 },
    };
    for (int i = 0; i < 5; i++) {
        textDraw(r, rx + 30, ry + rows2[i].y, rows2[i].k, C_MU_R, C_MU_G, C_MU_B, 24);
        textDraw(r, rx + 230, ry + rows2[i].y, rows2[i].v, C_TX_R, C_TX_G, C_TX_B, 24, true);
    }

    // Bandeau bas
    fillRect(r, cx + 50, cy + 550, 1340, 60, 20, 26, 38);
    textDraw(r, cx + 80, cy + 570,
             "GoldPlayer est une application homebrew PS4 légère, rapide et entièrement personnalisable.",
             C_GD_R, C_GD_G, C_GD_B, 24);
}

static void drawPlayer(SDL_Renderer* r, AppState& a) {
    int cx = 240, cy = 170, w = 1440, h = 600;
    uiPanel(r, cx, cy, w, h, true);

    textDraw(r, cx + 50, cy + 40, "LECTURE MULTIMÉDIA", C_GD2_R, C_GD2_G, C_GD2_B, 40, true);

    textDraw(r, cx + 50, cy + 110, a.playerTitle.c_str(), C_TX_R, C_TX_G, C_TX_B, 44, true);
    textDraw(r, cx + 50, cy + 175, a.playerSub.c_str(), C_GD_R, C_GD_G, C_GD_B, 28);

    textDraw(r, cx + 50, cy + 240, "SOURCE :", C_MU_R, C_MU_G, C_MU_B, 24);
    textDraw(r, cx + 50, cy + 280, a.playerInfo.c_str(), C_TX_R, C_TX_G, C_TX_B, 24);

    textDraw(r, cx + 50, cy + 360, "MOTEUR DE DÉCODAGE", C_GD_R, C_GD_G, C_GD_B, 26, true);
    textDraw(r, cx + 50, cy + 400,
             "Le décodage vidéo matériel (libSceAvPlayer) est en cours d'intégration.",
             C_MU_R, C_MU_G, C_MU_B, 24);
    textDraw(r, cx + 50, cy + 440,
             "Cette version pilote gère la navigation, les playlists M3U et le lancement des jeux.",
             C_MU_R, C_MU_G, C_MU_B, 24);

    fillRect(r, cx + 50, cy + 500, 1340, 60, 234, 179, 8);
    textDraw(r, cx + 90, cy + 518, "APPUYEZ SUR O POUR REVENIR", 10, 10, 10, 22, true);
}

void drawCurrent(AppState& a) {
    SDL_Renderer* r = a.renderer;
    if (!r) return;

    fillRect(r, 0, 0, FRAME_W, FRAME_H, 11, 14, 20);

    switch (a.screen) {
    case SCR_HOME:
        uiHeader(r, a, "ACCUEIL");
        textDraw(r, 80, 96, "Bienvenue sur GoldPlayer — choisissez une section :", C_MU_R, C_MU_G, C_MU_B, 24);
        uiList(r, a, "");
        break;

    case SCR_MEDIA:
        uiHeader(r, a, "MÉDIAS (USB)");
        if (a.currentPath.empty()) {
            textDraw(r, 80, 96, "Sélectionnez un stockage à explorer :", C_MU_R, C_MU_G, C_MU_B, 24);
        } else {
            textDraw(r, 80, 96, a.currentPath.c_str(), C_GD2_R, C_GD2_G, C_GD2_B, 24);
        }
        uiList(r, a, "Aucun stockage détecté — branchez un disque USB.");
        break;

    case SCR_IPTV:
        if (a.iptvView == 0) {
            uiHeader(r, a, "IPTV — PLAYLISTS M3U");
            textDraw(r, 80, 96, "Playlists .m3u / .m3u8 détectées sur vos stockages :", C_MU_R, C_MU_G, C_MU_B, 24);
            uiList(r, a, "Aucune playlist .m3u trouvée. Placez-en une à la racine du disque USB.");
        } else {
            uiHeader(r, a, "IPTV — CHAÎNES");
            textDraw(r, 80, 96, "Sélectionnez une chaîne pour la lire :", C_MU_R, C_MU_G, C_MU_B, 24);
            uiList(r, a, "Cette playlist ne contient aucune chaîne.");
        }
        break;

    case SCR_GAMES:
        uiHeader(r, a, "JEUX INSTALLÉS");
        textDraw(r, 80, 96, "Appuyez sur X pour lancer un titre via GoldHEN :", C_MU_R, C_MU_G, C_MU_B, 24);
        uiList(r, a, "Aucun jeu détecté dans /user/app.");
        break;

    case SCR_SETTINGS:
        uiHeader(r, a, "PARAMÈTRES");
        uiList(r, a, "");
        break;

    case SCR_ABOUT:
        uiHeader(r, a, "À PROPOS");
        drawAbout(r, a);
        break;

    case SCR_PLAYER:
        uiHeader(r, a, "LECTURE");
        drawPlayer(r, a);
        break;
    }

    // Banniere transitoire
    if (a.messageTimer > 0) {
        a.messageTimer--;
        int bw = 900, bh = 64;
        int bx = (FRAME_W - bw) / 2, by = 120;
        fillRect(r, bx, by, bw, bh, C_GD_R, C_GD_G, C_GD_B);
        fillRect(r, bx + 3, by + 3, bw - 6, bh - 6, 20, 26, 38);
        textDraw(r, bx + 24, by + 18, a.message.c_str(), C_TX_R, C_TX_G, C_TX_B, 24, true);
    }

    uiFooter(r, a);
}

// ---------------------------------------------------------------------
//  Actions
// ---------------------------------------------------------------------
void enterScreen(AppState& a, ScreenId s) {
    a.screen = s;
    a.cursor = 0;
    a.scroll = 0;
    refreshCurrent(a);
}

void onConfirm(AppState& a) {
    switch (a.screen) {
    case SCR_HOME: {
        if (a.cursor < 0 || a.cursor >= (int)a.items.size()) return;
        const std::string& v = a.items[a.cursor].value;
        if (v == "MEDIA")      enterScreen(a, SCR_MEDIA);
        else if (v == "IPTV")  enterScreen(a, SCR_IPTV);
        else if (v == "GAMES") enterScreen(a, SCR_GAMES);
        else if (v == "SETTINGS") enterScreen(a, SCR_SETTINGS);
        else if (v == "ABOUT") enterScreen(a, SCR_ABOUT);
        break;
    }

    case SCR_MEDIA: {
        if (a.items.empty()) return;
        if (a.currentPath.empty()) {
            if (a.cursor >= 0 && a.cursor < (int)a.items.size()) {
                a.currentPath = a.items[a.cursor].value;
                refreshCurrent(a);
            }
        } else {
            if (a.cursor == 0) {
                a.currentPath = parentPath(a.currentPath);
                refreshCurrent(a);
            } else {
                int mi = a.cursor - 1;
                if (mi >= 0 && mi < (int)a.media.size()) {
                    const MediaEntry& m = a.media[mi];
                    if (m.isDir) {
                        a.currentPath = m.path;
                        refreshCurrent(a);
                    } else {
                        openPlayer(a, m.name, "Fichier " + m.kind, m.path);
                    }
                }
            }
        }
        break;
    }

    case SCR_IPTV: {
        if (a.iptvView == 0) {
            if (a.cursor >= 0 && a.cursor < (int)a.playlists.size()) {
                a.channels = m3uParseFile(a.playlists[a.cursor].path);
                a.iptvView = 1;
                a.cursor = 0;
                a.scroll = 0;
                refreshCurrent(a);
            }
        } else {
            if (a.cursor >= 0 && a.cursor < (int)a.channels.size()) {
                const IptvChannel& c = a.channels[a.cursor];
                openPlayer(a, c.name, c.group.empty() ? "Chaîne IPTV" : c.group, c.url);
            }
        }
        break;
    }

    case SCR_GAMES: {
        if (a.cursor >= 0 && a.cursor < (int)a.games.size()) {
            const GameEntry& g = a.games[a.cursor];
            bool ok = launchGame(g.titleId);
            showBanner(a, ok ? "Requête envoyée à GoldHEN : " + g.name
                             : "Impossible de contacter GoldHEN (127.0.0.1:2128)");
        }
        break;
    }

    case SCR_SETTINGS: {
        if (a.cursor >= 0 && a.cursor < (int)a.items.size() &&
            a.items[a.cursor].value == "ABOUT") {
            enterScreen(a, SCR_ABOUT);
        }
        break;
    }

    case SCR_ABOUT:
    case SCR_PLAYER:
    default:
        break;
    }
}

void onBack(AppState& a) {
    switch (a.screen) {
    case SCR_HOME:
        break; // rien (on reste a l'accueil)

    case SCR_MEDIA:
        if (a.currentPath.empty()) {
            enterScreen(a, SCR_HOME);
        } else {
            a.currentPath = parentPath(a.currentPath);
            refreshCurrent(a);
        }
        break;

    case SCR_IPTV:
        if (a.iptvView == 1) {
            a.iptvView = 0;
            a.cursor = 0;
            a.scroll = 0;
            refreshCurrent(a);
        } else {
            enterScreen(a, SCR_HOME);
        }
        break;

    case SCR_GAMES:
    case SCR_SETTINGS:
    case SCR_ABOUT:
        enterScreen(a, SCR_HOME);
        break;

    case SCR_PLAYER:
        enterScreen(a, a.returnScreen);
        break;

    default:
        enterScreen(a, SCR_HOME);
        break;
    }
}
