// =====================================================================
//  m3u.cpp - Parseur de playlists M3U / M3U8 (IPTV)
//  Gere : BOM UTF-8, #EXTM3U, #EXTINF, #EXTGRP, attributs tvg-*,
//         group-title, et le nom apres la virgule.
// =====================================================================
#include "m3u.h"
#include <cstdio>

namespace {

std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) a++;
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r' || s[b - 1] == '\n')) b--;
    return s.substr(a, b - a);
}

std::string attr(const std::string& line, const std::string& key) {
    std::string pat = key + "=\"";
    size_t p = line.find(pat);
    if (p == std::string::npos) return "";
    p += pat.size();
    size_t e = line.find('"', p);
    if (e == std::string::npos) return "";
    return line.substr(p, e - p);
}

} // namespace

std::vector<IptvChannel> m3uParse(const std::string& textIn) {
    std::vector<IptvChannel> out;

    std::string text = textIn;
    // Supprime le BOM UTF-8 eventuel.
    if (text.size() >= 3 &&
        (unsigned char)text[0] == 0xEF &&
        (unsigned char)text[1] == 0xBB &&
        (unsigned char)text[2] == 0xBF) {
        text.erase(0, 3);
    }

    IptvChannel cur;
    bool pending = false;

    size_t pos = 0;
    while (pos <= text.size()) {
        size_t nl = text.find('\n', pos);
        if (nl == std::string::npos) nl = text.size();
        std::string line = trim(text.substr(pos, nl - pos));
        pos = nl + 1;

        if (line.empty()) continue;

        if (line.compare(0, 8, "#EXTINF:") == 0) {
            cur = IptvChannel();
            pending = true;
            cur.tvgId = attr(line, "tvg-id");
            cur.logo  = attr(line, "tvg-logo");
            cur.group = attr(line, "group-title");
            // Nom : texte apres la derniere virgule.
            size_t q = line.rfind('"');
            size_t c = line.find(',', q == std::string::npos ? 0 : q);
            if (c != std::string::npos) cur.name = trim(line.substr(c + 1));
            if (cur.name.empty()) cur.name = attr(line, "tvg-name");
        } else if (line.compare(0, 8, "#EXTGRP:") == 0) {
            if (pending && cur.group.empty()) cur.group = trim(line.substr(8));
        } else if (line[0] != '#') {
            // Ligne URL (ou chemin de fichier local).
            if (!pending) {
                cur = IptvChannel();
                cur.name = line;
            }
            cur.url = line;
            out.push_back(cur);
            pending = false;
        }
        // Les autres directives (#EXTM3U, #PLAYLIST, ...) sont ignorees.
    }

    return out;
}

std::vector<IptvChannel> m3uParseFile(const std::string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return std::vector<IptvChannel>();

    std::string content;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        content.append(buf, n);
    }
    fclose(f);
    return m3uParse(content);
}
