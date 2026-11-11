# 🎮 GoldPlayer — Lecteur multimédia & IPTV pour PS4 (GoldHEN)

Application homebrew **native PS4** (OpenOrbis + SDL2), légère et rapide, compilée en `.pkg` via **GitHub Actions** pour une installation simple depuis le menu système d'une PS4 jailbreakée (GoldHEN).

> Designer : **DJAFFAR MOURTADHOI** · @Mourtadhoi.Art · 0693521495 · djaffar.art@gmail.com

---

## ✨ Fonctionnalités

| Écran | Description |
|---|---|
| 🏠 **Accueil** | Menu principal : Médias, IPTV, Jeux, Paramètres, À propos. |
| 💽 **Médias (USB)** | Parcours **réel** de vos disques externes (`/mnt/usb0`, `/mnt/usb1`, stockage étendu, `/data`) — dossiers et fichiers classés (vidéo / audio / image / playlist). |
| 📺 **IPTV — M3U** | Détection des playlists `.m3u` / `.m3u8` sur vos stockages, parsing des chaînes (`#EXTINF`, `tvg-name`, `tvg-logo`, `group-title`), regroupement intuitif. |
| 🕹️ **Jeux installés** | **Énumération réelle** des jeux de `/user/app` (titre lu dans `param.sfo`) et **lancement direct via l'API REST de GoldHEN** (`127.0.0.1:2128`). |
| ⚙️ **Paramètres** | Informations du package (Title ID, Content ID, version, compatibilité). |
| ℹ️ **À propos** | Coordonnées complètes du concepteur + crédits. |

Le rendu utilise **FreeType** (police Roboto intégrée) avec support des **accents français** (é, à, ç…).

---

## 🧭 Commandes manette

| Bouton | Action |
|---|---|
| **D-Pad Haut / Bas** | Naviguer dans les listes |
| **✕ (Croix)** | Valider / Lancer / Ouvrir |
| **◯ (Rond)** | Retour |
| **L1 / R1** | Page précédente / suivante |

---

## 📦 Compilation en `.pkg` (GitHub Actions)

1. Créez un dépôt GitHub et poussez ces fichiers :
   ```bash
   git init
   git add -A
   git commit -m "GoldPlayer"
   git branch -M main
   git remote add origin https://github.com/VOTRE_COMPTE/GoldPlayer.git
   git push -u origin main
   ```
2. Allez dans l'onglet **Actions** → le workflow `Build GoldPlayer PS4 PKG` démarre automatiquement.
3. Une fois terminé, téléchargez l'artefact **`GoldPlayer-PS4-PKG`** → il contient `IV0000-GOLD00001_00-GOLDPLAYER000000.pkg`.

Le workflow (`.github/workflows/build.yml`) :
- installe `clang` / `lld` + `libssl1.1` (requis par `PkgTool.Core`) ;
- télécharge le **toolchain OpenOrbis v0.5.4** ;
- exécute `make` → génère `param.sfo`, `eboot.bin`, `pkg.gp4`, puis le `.pkg`.

> ⚙️ **Compilation locale (optionnelle)** : installez le [toolchain OpenOrbis](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain) (clang + lld requis), définissez la variable `OO_PS4_TOOLCHAIN`, puis lancez `make`. Le plus simple reste GitHub Actions.

---

## 💾 Installation sur PS4

1. Copiez le `.pkg` à la racine d'une clé USB (formatée **exFAT** ou **FAT32**).
2. Sur la PS4 (GoldHEN actif) : **Paramètres → GoldHEN → Debug Settings → Package Installer** (ou l'équivalent de votre firmware).
3. Sélectionnez le fichier → installer. L'app apparaît dans le menu système, prête à lancer.

---

## 📁 Structure du projet

```
.
├── .github/workflows/build.yml   # Compilation CI → PKG
├── Makefile                      # Build OpenOrbis autonome (v0.5.4)
├── goldplayer/                   # Code source C++ (SDL2 + FreeType)
│   ├── main.cpp                  # Initialisation + boucle + manette
│   ├── goldplayer.h              # Types + prototypes communs
│   ├── types.h                   # Structures pures
│   ├── text.cpp                  # Rendu texte FreeType (UTF-8)
│   ├── ui.cpp                    # Dessin, listes, header/footer
│   ├── screens.cpp               # Orchestration des écrans
│   ├── files.cpp                 # Explorateur USB + détection média
│   ├── games.cpp                 # Jeux installés + lancement GoldHEN
│   ├── m3u.{h,cpp}               # Parseur de playlists M3U
│   └── sfo.{h,cpp}               # Lecture de param.sfo (titre des jeux)
├── assets/fonts/                 # Roboto (Regular + Bold)
├── sce_module/                   # libc.prx, libSceFios2.prx
└── sce_sys/                      # icon0.png, pic0.png, pic1.png, about/right.sprx
```

---

## 🔧 Notes techniques

- **Énumération des jeux** : `listGames()` lit `/user/app/` et le titre depuis `/user/appmeta/<TITLE_ID>/param.sfo` (parseur SFO maison). Si le titre est illisible, l'ID est affiché.
- **Lancement des jeux** : `launchGame()` envoie `GET /api/launch_title?title_id=<id>` au serveur REST de GoldHEN (`127.0.0.1:2128`).
- **Playlists** : `findPlaylists()` scanne `/mnt/usb0`, `/mnt/usb1`, `/mnt/usb_ext0` et `/data` (profondeur 3) à la recherche de `.m3u`/`.m3u8`.
- **Décodage vidéo matériel** : la lecture des pixels vidéo via `libSceAvPlayer` (dont OpenOrbis n'expose que des stubs vides) est **la prochaine étape**. Cette version pilote gère entièrement la navigation, les playlists et le lancement des jeux ; l'écran « Lecture » affiche la source sélectionnée en attendant l'intégration du moteur de décodage.

---

## 📜 Licences tierces

- **OpenOrbis** (toolchain, `right.sprx`, `libc.prx`, `libSceFios2.prx`) — [GPLv3](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
- **SDL2** (port PS4) — licence zlib
- **FreeType** — FreeType License
- **Roboto** — [Apache 2.0](https://github.com/googlefonts/roboto)

Le code source de GoldPlayer est sous licence **MIT** (voir `LICENSE`).
