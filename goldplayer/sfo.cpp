// =====================================================================
//  sfo.cpp - Lecteur minimal de param.sfo (format Sony SFO)
//  Format : magic "\0PSF", version, keyTableOffset, dataTableOffset,
//           numEntries ; puis table de cles + table de donnees.
// =====================================================================
#include "sfo.h"
#include <vector>
#include <cstdint>
#include <cstdio>

namespace {

uint32_t le32(const unsigned char* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

uint16_t le16(const unsigned char* p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

} // namespace

std::string sfoGetString(const std::string& path, const std::string& key) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return "";

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 20) {
        fclose(f);
        return "";
    }

    std::vector<unsigned char> buf((size_t)sz);
    if (fread(buf.data(), 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        return "";
    }
    fclose(f);

    // Magic "\0PSF"
    if (buf[0] != 0x00 || buf[1] != 'P' || buf[2] != 'S' || buf[3] != 'F') {
        return "";
    }

    uint32_t keyOff  = le32(&buf[8]);
    uint32_t dataOff = le32(&buf[12]);
    uint32_t num     = le32(&buf[16]);

    for (uint32_t i = 0; i < num; i++) {
        const unsigned char* e = &buf[dataOff + i * 16];
        uint16_t keyRel  = le16(e);
        uint16_t fmt     = le16(e + 2);
        uint32_t len     = le32(e + 4);
        uint32_t dataRel = le32(e + 12);

        const char* k = (const char*)&buf[keyOff + keyRel];
        if (key == k) {
            // Formats chaine UTF-8 (0x0004 et 0x0204).
            if (fmt == 0x0004 || fmt == 0x0204) {
                const char* v = (const char*)&buf[dataOff + dataRel];
                std::string s(v, v + len);
                size_t z = s.find('\0');
                if (z != std::string::npos) s = s.substr(0, z);
                return s;
            }
            return "";
        }
    }
    return "";
}
