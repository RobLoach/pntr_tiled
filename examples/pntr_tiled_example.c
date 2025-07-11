#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

#define MIN(a,b) (a < b) ? a : b;
#define MAX(a,b) (a > b) ? a : b;

typedef enum {
    DIRECTION_SOUTH,
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_WEST,
} Direction;

typedef struct AppData {
    cute_tiled_map_t* map;
    float speed;
    cute_tiled_layer_t* objects;
    cute_tiled_object_t* player;
    Direction direction;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->speed = 200;

    appData->map = pntr_load_tiled("examples/resources/desert.tmj");

    // if you need more than 1 layer or object, use a single loop
    appData->objects = pntr_tiled_layer(appData->map, "objects");
    appData->player = pntr_tiled_get_object(appData->objects, "player");

    // I added the player in differnt directions, off-screen, to figure out GIDs
    // cute_tiled_object_t* N = pntr_tiled_get_object(appData->objects, "N");
    // cute_tiled_object_t* S = pntr_tiled_get_object(appData->objects, "S");
    // cute_tiled_object_t* E = pntr_tiled_get_object(appData->objects, "E");
    // cute_tiled_object_t* W = pntr_tiled_get_object(appData->objects, "W");
    // printf("Player GIDs: N:%d S:%d E:%d W:%d\n", N->gid, S->gid, E->gid, W->gid);
    
    if (appData->objects == NULL) {
        printf("no objects");
    }

    if (appData->player == NULL) {
        printf("no player");
    }

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    bool walking = false;

    // Keyboard/Gamepad
    if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
        appData->player->x -= appData->speed * pntr_app_delta_time(app);
        walking=true;
        appData->direction = DIRECTION_WEST;
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
        appData->player->x += appData->speed * pntr_app_delta_time(app);
        walking=true;
        appData->direction = DIRECTION_EAST;
    }
    if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
        appData->player->y -= appData->speed * pntr_app_delta_time(app);
        walking=true;
        appData->direction = DIRECTION_NORTH;
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
        appData->player->y += appData->speed * pntr_app_delta_time(app);
        walking=true;
        appData->direction = DIRECTION_SOUTH;
    }

    // choose animated/still in correct direction
    // the layout is still/walkA/walkB for each direction, and walkA is the animation
    // Standing GIDs: N:52 S:49 E:58 W:55
    // Walking GIDs: N:53 S:50 E:59 W:56
    switch(appData->direction) {
        case DIRECTION_SOUTH: appData->player->gid = walking ? 50 : 49; break;
        case DIRECTION_NORTH: appData->player->gid = walking ? 53 : 52; break;
        case DIRECTION_EAST: appData->player->gid = walking ? 59 : 58; break;  
        case DIRECTION_WEST: appData->player->gid = walking ? 56 : 55; break;
    }

    // Update any map data.
    pntr_update_tiled(appData->map, pntr_app_delta_time(app));

    // Clear the screen
    pntr_clear_background(screen, PNTR_BLACK);

    // camera: keep in bounds
    int camera_x = MAX(0, appData->player->x - screen->width / 2);
    int camera_y = MAX(0, appData->player->y - screen->height / 2);
    camera_x = MIN(camera_x, (appData->map->width * appData->map->tilewidth) - screen->width);
    camera_y = MIN(camera_y, (appData->map->height * appData->map->tileheight) - screen->height);

    // Draw the map
    pntr_draw_tiled(screen, appData->map, -camera_x, -camera_y, PNTR_WHITE);

    // once object-layer is setup, this should not be needed
    // pntr_draw_tiled_tile(screen, appData->map, appData->player->gid, appData->player->x-camera_x, appData->player->y- camera_y-appData->map->tilewidth, PNTR_WHITE);

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
