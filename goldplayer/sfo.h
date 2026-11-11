#pragma once
// =====================================================================
//  sfo.h - Lecture de param.sfo (titre des jeux PS4)
// =====================================================================
#include <string>

// Lit la valeur d'une entree UTF-8 (ex: "TITLE") d'un fichier param.sfo.
// Retourne "" en cas d'echec.
std::string sfoGetString(const std::string& path, const std::string& key);
