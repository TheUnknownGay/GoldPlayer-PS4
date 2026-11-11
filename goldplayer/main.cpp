// =====================================================================
//  main.cpp - Point d'entree, initialisation SDL/FreeType, boucle,
//             gestion des entrees manette.
// =====================================================================
#include "goldplayer.h"

bool appInit(AppState& a) {
    memset(&a, 0, sizeof(a));
    setvbuf(stdout, NULL, _IONBF, 0);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        return false;
    }

    // Module FreeType (optionnel : le texte devient silencieux si absent).
    sceSysmoduleLoadModule(ORBIS_SYSMODULE_FREETYPE_OL);
    textInit();

    a.window = SDL_CreateWindow("GoldPlayer", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, FRAME_W, FRAME_H, 0);
    if (!a.window) return false;

    a.surface = SDL_GetWindowSurface(a.window);
    a.renderer = SDL_CreateSoftwareRenderer(a.surface);
    if (!a.renderer) return false;

    if (SDL_NumJoysticks() > 0) {
        a.pad = SDL_JoystickOpen(0);
    }

    a.screen = SCR_HOME;
    a.returnScreen = SCR_HOME;
    a.iptvView = 0;
    a.cursor = 0;
    a.scroll = 0;
    refreshCurrent(a);
    return true;
}

void appQuit(AppState& a) {
    if (a.pad) SDL_JoystickClose(a.pad);
    if (a.renderer) SDL_DestroyRenderer(a.renderer);
    if (a.window) SDL_DestroyWindow(a.window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    AppState app;
    if (!appInit(app)) {
        // Echec d'initialisation : on bloque (console de debug).
        for (;;) {}
        return -1;
    }

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                int k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE) running = false;
                else if (k == SDLK_UP) { if (app.cursor > 0) app.cursor--; }
                else if (k == SDLK_DOWN) { if (app.cursor < (int)app.items.size() - 1) app.cursor++; }
                else if (k == SDLK_RETURN || k == SDLK_SPACE) onConfirm(app);
                else if (k == SDLK_BACKSPACE) onBack(app);
            } else if (e.type == SDL_JOYBUTTONDOWN) {
                int b = e.jbutton.button;
                if (b == 0) {            // Croix : valider
                    onConfirm(app);
                } else if (b == 1) {     // Rond : retour
                    onBack(app);
                } else if (b == 4) {     // L1 : page precedente
                    app.cursor -= 8;
                    if (app.cursor < 0) app.cursor = 0;
                } else if (b == 5) {     // R1 : page suivante
                    app.cursor += 8;
                    int n = (int)app.items.size();
                    if (app.cursor >= n) app.cursor = (n > 0) ? n - 1 : 0;
                } else if (b == 13) {    // D-pad haut (si boutons)
                    if (app.cursor > 0) app.cursor--;
                } else if (b == 14) {    // D-pad bas (si boutons)
                    if (app.cursor < (int)app.items.size() - 1) app.cursor++;
                }
            } else if (e.type == SDL_JOYHATMOTION) {
                int v = e.jhat.value;
                if (v == SDL_HAT_UP) { if (app.cursor > 0) app.cursor--; }
                else if (v == SDL_HAT_DOWN) { if (app.cursor < (int)app.items.size() - 1) app.cursor++; }
            }
        }

        drawCurrent(app);
        SDL_UpdateWindowSurface(app.window);
        SDL_Delay(16);
    }

    appQuit(app);
    return 0;
}
