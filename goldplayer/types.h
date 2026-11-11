#pragma once
// =====================================================================
//  types.h - Structures de donnees pures (aucune dependance plateforme)
// =====================================================================
#include <string>
#include <vector>
#include <cstdint>

// Entree de l'explorateur de fichiers (USB / data / app0)
struct MediaEntry {
    std::string name;   // nom affiche
    std::string path;   // chemin complet
    bool        isDir;
    std::string kind;   // "dossier", "video", "audio", "playlist", "image", "autre"
};

// Chaine IPTV issue d'une playlist M3U/M3U8
struct IptvChannel {
    std::string name;
    std::string url;
    std::string group;   // group-title
    std::string tvgId;   // tvg-id
    std::string logo;    // tvg-logo
};

// Playlist M3U trouvee sur le stockage
struct PlaylistEntry {
    std::string name;
    std::string path;
    int         channelCount;
};

// Jeu PS4 installe
struct GameEntry {
    std::string titleId;   // ex: CUSAxxxxx
    std::string name;      // titre lu depuis param.sfo
    std::string location;  // /user/app ou /mnt/usb0
};

// Element generique de menu/liste
struct MenuItem {
    std::string label;
    std::string sub;
    std::string value;     // charge utile (chemin / titleId / ...)
};
