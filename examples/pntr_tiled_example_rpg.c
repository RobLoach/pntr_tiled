#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// just some lil utils to see what is in map
#include "debug_map.h"

typedef struct AppData {
    struct nk_context* ctx;
    cute_tiled_map_t* map;
    
    // current player speed
    int speed;
    
    // current camera (map-draw) position
    int x, y;
    
    // this tracks single sign dialog popup
    // set to NULL or current text that should show on screen
    char* sign_text;
    
    // layer that is drawn over map by update_map_objects
    pntr_image* objects;

    // this will be used to draw sprites
    // it's the same spritesheet in the map
    pntr_image* sprites;
} AppData;

// unload current map and switch to another one
void map_portal(char* name) {}

// given a tile-number (0-indexed) return rec of image
pntr_rectangle get_tile_rec(int id, pntr_image* src) {
    int tw = src->width / 16;
    pntr_rectangle r = { .x=(id % tw) * 16, .y=(id / tw) * 16, .width=16, .height=16 };
    return r;
}


// this will update/draw all the map-objects, including player, based on state of things
void update_map_objects(AppData* appData) {
    // TODO: check keys to update "wants to move" (to change sprite-facing direction, check collisions, etc)

    cute_tiled_layer_t* layer = appData->map->layers;

    while(layer) {
        printf("LAYER: %s\n", layer->name.ptr);

        // this is the layer where I keep collision-shapes (to block where the player/enemies can go)
        // this could also be done with proper Tiled collision-stuff, this is just how I set it up
        if (strcmp(layer->name.ptr, "collision") == 0) {
            // TODO: check collisions
        }

        // this is the layer where I keep object-pointers that determine game-logic
        if (strcmp(layer->name.ptr, "logic") == 0) {
            cute_tiled_object_t* o = layer->objects;
            while (o) {
                int gid = o->gid;

                // these are the tile-space coordinates
                int tileX = (int) (o->x/16.0);
                int tileY = (int) (o->y/16.0);

                // these are the image coordinates
                int posX = (int) o->x;
                int posY = (int) o->y;

                printf("  OBJECT (%dx%d) - %d: %s (%s)\n", tileX, tileY, gid, o->name.ptr, o->type.ptr);

                // draw the representative tile
                if (gid) {
                    pntr_draw_image_rec(appData->objects, appData->sprites, get_tile_rec(gid-1, appData->sprites), posX, posY-16);
                }

                // gid represents player
                if (gid < 13) {
                    
                }

                o = o->next;
            }
        }
        layer = layer->next;
    }
}


bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->map = pntr_load_tiled("resources/rpg/welcome.tmj");
    appData->x = 0;
    appData->y = 0;
    appData->speed = 200;

    appData->sign_text = NULL;

    // print_map(appData->map);

    int w = appData->map->width * appData->map->tilewidth;
    int h = appData->map->height * appData->map->tileheight;

    // create layer for objects to be drawn on
    appData->objects = pntr_new_image(w, h);

    // setup all the spritesheets for objects
    // TODO: are these already loaded/quaded by map?
    appData->sprites = pntr_load_image("resources/rpg/sprites.png");

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    // this is "camera" behavior, so you can move the camera around, seperate from the player

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

    // update all map objects, and draw them
    update_map_objects(appData);
    pntr_draw_image(screen, appData->objects, appData->x, appData->y);

    return true;
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_tiled(appData->map);

    // TODO: unload layers & sprites
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
