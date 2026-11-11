// =====================================================================
//  files.cpp - Explorateur de fichiers (USB / data) + detection medias
//  Detection de dossier via stat()/S_ISDIR (pas de d_type/DT_*).
// =====================================================================
#include "goldplayer.h"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cctype>

namespace {

bool isDir(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return (char)tolower((int)c); });
    return s;
}

std::string extOf(const std::string& name) {
    size_t dot = name.rfind('.');
    if (dot == std::string::npos) return "";
    return lower(name.substr(dot + 1));
}

void scanPlaylists(const std::string& dir, int depth, std::vector<PlaylistEntry>& out) {
    if (depth < 0) return;
    DIR* d = opendir(dir.c_str());
    if (!d) return;

    struct dirent* e;
    while ((e = readdir(d)) != NULL) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;
        std::string full = dir + "/" + name;

        if (isDir(full)) {
            scanPlaylists(full, depth - 1, out);
        } else {
            std::string ext = extOf(name);
            if (ext == "m3u" || ext == "m3u8") {
                PlaylistEntry p;
                p.name = name;
                p.path = full;
                p.channelCount = (int)m3uParseFile(full).size();
                out.push_back(p);
            }
        }
    }
    closedir(d);
}

} // namespace

std::string mediaKind(const std::string& name) {
    std::string ext = extOf(name);
    if (ext == "mp4" || ext == "mkv" || ext == "avi" || ext == "ts" ||
        ext == "m2ts" || ext == "mts" || ext == "mov" || ext == "m4v" ||
        ext == "webm" || ext == "mpg" || ext == "mpeg" || ext == "wmv" ||
        ext == "flv" || ext == "vob" || ext == "3gp")
        return "video";
    if (ext == "mp3" || ext == "aac" || ext == "m4a" || ext == "flac" ||
        ext == "wav" || ext == "ogg" || ext == "oga" || ext == "wma" ||
        ext == "opus")
        return "audio";
    if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" ||
        ext == "gif" || ext == "webp")
        return "image";
    if (ext == "m3u" || ext == "m3u8")
        return "playlist";
    return "autre";
}

bool dirExists(const std::string& path) {
    return isDir(path);
}

std::vector<MediaEntry> browseDir(const std::string& path) {
    std::vector<MediaEntry> out;
    DIR* d = opendir(path.c_str());
    if (!d) return out;

    struct dirent* e;
    while ((e = readdir(d)) != NULL) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;

        MediaEntry m;
        m.name = name;
        m.path = path + "/" + name;
        m.isDir = isDir(m.path);
        m.kind = m.isDir ? "dossier" : mediaKind(name);
        out.push_back(m);
    }
    closedir(d);

    // Dossiers d'abord, puis fichiers, tri alphabetique.
    std::sort(out.begin(), out.end(), [](const MediaEntry& a, const MediaEntry& b) {
        if (a.isDir != b.isDir) return a.isDir > b.isDir;
        return a.name < b.name;
    });
    return out;
}

std::vector<PlaylistEntry> findPlaylists() {
    std::vector<PlaylistEntry> out;
    const char* roots[] = { "/mnt/usb0", "/mnt/usb1", "/mnt/usb_ext0", "/data", NULL };
    for (int i = 0; roots[i]; i++) {
        scanPlaylists(roots[i], 3, out);
    }
    return out;
}
