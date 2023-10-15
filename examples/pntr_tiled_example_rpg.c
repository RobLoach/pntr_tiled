#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// just some lil utils to see what is in map
#include "debug_map.h"

typedef enum FaceDirection {
    FACE_SOUTH,
    FACE_NORTH,
    FACE_EAST,
    FACE_WEST
} FaceDirection;


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

    bool playerWalking;
    FaceDirection playerDirection;
    int playerX, playerY;

    float gameTime;
} AppData;

// unload current map and switch to another one
void map_portal(char* name, AppData* appData) {
    // reset so it will be pulled from initial position/direction in map
    appData->playerX = 0;
    appData->playerY = 0;

    appData->x = 0;
    appData->y = 0;
    appData->sign_text = NULL;

    pntr_unload_tiled(appData->map);

    char fname[80];
    sprintf(fname, "resources/rpg/%s.tmj", name);
    appData->map = pntr_load_tiled(fname);
}

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
        // printf("LAYER: %s\n", layer->name.ptr);

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

                // printf("  OBJECT (%dx%d) - %d: %s (%s)\n", tileX, tileY, gid, o->name.ptr, o->type.ptr);

                // draw the representative tile
                if (gid) {
                    // gid represents player
                    if (gid < 13) {
                        // set player position if it has not been set (for initial state)
                        if (appData->playerX == 0 && appData->playerY == 0) {
                            appData->playerX = posX;
                            appData->playerY = posY;
                            appData->playerDirection = gid/3;
                        }
                        
                        // handle walking animation
                        int frame = 0;

                        if (appData->playerWalking) {
                            frame = ((int)(appData->gameTime * appData->speed/30) % 2) + 1;
                        }

                        // draw still player
                        pntr_draw_image_rec(appData->objects, appData->sprites, get_tile_rec((appData->playerDirection * 3) + frame, appData->sprites), appData->playerX, appData->playerY-16);
                    }
                    // generic: draw whatever tile
                    else {
                        pntr_draw_image_rec(appData->objects, appData->sprites, get_tile_rec(gid-1, appData->sprites), posX, posY-16);
                    }
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
    
    appData->speed = 200;

    map_portal("welcome", appData);

    // print_map(appData->map);

    // create layer for objects to be drawn on
    appData->objects = pntr_new_image(appData->map->width * appData->map->tilewidth, appData->map->height * appData->map->tileheight);

    // setup all the spritesheet for objects
    // TODO: are these already loaded/quaded by map?
    appData->sprites = pntr_load_image("resources/rpg/sprites.png");

    appData->gameTime = 0;

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    appData->gameTime += pntr_app_delta_time(app);

    // this is "camera" behavior, so you can move the camera around, seperate from the player

    // Mouse
    if (pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
        appData->x -= pntr_app_mouse_delta_x(app);// * pntr_app_delta_time(app) * app->fps;
        appData->y -= pntr_app_mouse_delta_y(app);// * pntr_app_delta_time(app) * app->fps;
    } else {
        appData->x =  screen->width - appData->playerX - (screen->width/2) + 8;
        appData->y =  screen->height - appData->playerY - (screen->height/2) + 8;
    }

    // Clear the screen
    pntr_clear_background(screen, PNTR_BLACK);

    // Draw the map
    pntr_draw_tiled(screen, appData->map, appData->x, appData->y, PNTR_WHITE);

    // update all map objects
    pntr_clear_background(appData->objects, PNTR_BLANK);
    update_map_objects(appData);

    // Keyboard/Gamepad
    // TODO: check collisions
    bool anyDirectionKey = false;
    if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
        appData->playerX -= appData->speed * pntr_app_delta_time(app);
        anyDirectionKey = true;
        appData->playerDirection = FACE_WEST;
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
        appData->playerX += appData->speed * pntr_app_delta_time(app);
        anyDirectionKey = true;
        appData->playerDirection = FACE_EAST;
    }
    
    if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
        appData->playerY -= appData->speed * pntr_app_delta_time(app);
        anyDirectionKey = true;
        appData->playerDirection = FACE_NORTH;
    }
    else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
        appData->playerY += appData->speed * pntr_app_delta_time(app);
        anyDirectionKey = true;
        appData->playerDirection = FACE_SOUTH;
    }
    
    appData->playerWalking = anyDirectionKey;

    // draw all map objects
    pntr_draw_image(screen, appData->objects, appData->x, appData->y);

    // TODO: handle keys to update playerX/playerY

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
