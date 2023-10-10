#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

typedef struct AppData {
    struct nk_context* ctx;
    cute_tiled_map_t* map;

    int x, y;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->map = pntr_load_tiled("resources/desert.json");
    appData->x = 0;
    appData->y = 0;

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);


    if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT)) {
        appData->x--;
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT)) {
        appData->x++;
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_UP)) {
        appData->y--;
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN)) {
        appData->y++;
    }

    pntr_clear_background(screen, PNTR_BLACK);
    pntr_draw_tiled(screen, appData->map, appData->x, appData->y, PNTR_WHITE);

    return true;
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_tiled(appData->map);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 400,
        .height = 225,
        .title = "pntr_tiled: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60
    };
}
