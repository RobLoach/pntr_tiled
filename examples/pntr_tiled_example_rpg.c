#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

typedef struct AppData {
    struct nk_context* ctx;
    cute_tiled_map_t* map;
    int speed;
    int x, y;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->map = pntr_load_tiled("resources/rpg/welcome.tmj");
    appData->x = 0;
    appData->y = 0;
    appData->speed = 200;

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    // Keyboard/Gamepad
    if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
        appData->x += appData->speed * pntr_app_delta_time(app);
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
        appData->x -= appData->speed * pntr_app_delta_time(app);
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
        appData->y += appData->speed * pntr_app_delta_time(app);
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
        appData->y -= appData->speed * pntr_app_delta_time(app);
    }

    // Mouse
    if (pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
        appData->x -= pntr_app_mouse_delta_x(app);// * pntr_app_delta_time(app) * app->fps;
        appData->y -= pntr_app_mouse_delta_y(app);// * pntr_app_delta_time(app) * app->fps;
    }

    // Keep the map within screen bounds
    if (appData->x > 0) {
        appData->x = 0;
    }
    if (appData->y > 0) {
        appData->y = 0;
    }
    if (appData->x - screen->width < -appData->map->width * appData->map->tilewidth) {
        appData->x = -appData->map->width * appData->map->tilewidth + screen->width;
    }
    if (appData->y - screen->height < -appData->map->height * appData->map->tileheight) {
        appData->y = -appData->map->height * appData->map->tileheight + screen->height;
    }

    // Clear the screen
    pntr_clear_background(screen, PNTR_BLACK);

    // Draw the map
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
        .title = "pntr_tiled: RPG",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60
    };
}
