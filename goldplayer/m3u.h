#pragma once
// =====================================================================
//  m3u.h - Parseur de playlists M3U / M3U8 (IPTV)
// =====================================================================
#include "types.h"
#include <string>

// Analyse le contenu d'une playlist M3U et retourne la liste des chaines.
std::vector<IptvChannel> m3uParse(const std::string& text);

// Lit un fichier .m3u/.m3u8 et retourne la liste des chaines.
std::vector<IptvChannel> m3uParseFile(const std::string& path);
