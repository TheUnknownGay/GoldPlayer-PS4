// =====================================================================
//  games.cpp - Enumeration des jeux PS4 installes + lancement GoldHEN
// =====================================================================
#include "goldplayer.h"
#include <dirent.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>

std::vector<GameEntry> listGames() {
    std::vector<GameEntry> out;

    DIR* d = opendir("/user/app");
    if (!d) return out;

    struct dirent* e;
    while ((e = readdir(d)) != NULL) {
        std::string id = e->d_name;
        if (id == "." || id == "..") continue;
        // Ne garder que les identifiants de titre (CUSAxxxxx, PCJSxxxxx, ...).
        if (id.size() < 4 || !isalpha((unsigned char)id[0])) continue;

        GameEntry g;
        g.titleId = id;
        g.location = "/user/app";

        // Titre depuis param.sfo (peut etre vide -> on garde l'id).
        std::string title = sfoGetString("/user/appmeta/" + id + "/param.sfo", "TITLE");
        g.name = title.empty() ? id : title;
        out.push_back(g);
    }
    closedir(d);

    std::sort(out.begin(), out.end(), [](const GameEntry& a, const GameEntry& b) {
        return a.name < b.name;
    });
    return out;
}

// Lance un titre via l'API REST de GoldHEN (serveur local 127.0.0.1:2128).
bool launchGame(const std::string& titleId) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2128);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    bool ok = false;
    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) >= 0) {
        char req[256];
        snprintf(req, sizeof(req),
                 "GET /api/launch_title?title_id=%s HTTP/1.1\r\n"
                 "Host: 127.0.0.1:2128\r\nConnection: close\r\n\r\n",
                 titleId.c_str());
        send(sock, req, strlen(req), 0);
        ok = true;
    }
    close(sock);
    return ok;
}
