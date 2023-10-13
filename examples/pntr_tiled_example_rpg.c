#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// just some lil utils to see what is in map
#include "debug_map.h"

// Even though these are in map, they are not drawn.
// I find it easier to use tile-objects to represent
// different objects/orientations. So, I setup a tileset
// that is just the stuff, then put it on "logic" layer.
// This can also be done with any object, with properties,
// if you need something more advanced, but you can encode
// a lot of the same info in type/name/sprite-id, and I
// like that I can see it in map-editor, more quickly.
typedef enum LogicObject {
    LOGIC_PLAYER_E = 133,
    LOGIC_PLAYER_N = 139,
    LOGIC_PLAYER_S = 121,
    LOGIC_PLAYER_W = 127,

    LOGIC_LOOT = 161,
    LOGIC_POT = 125,
    LOGIC_SIGN = 168,

    LOGIC_TORCH_BLUE = 137,
    LOGIC_TORCH_GREEN = 149,
    LOGIC_TORCH_ORANGE = 155,
    LOGIC_TORCH_RED = 143,
    LOGIC_TORCH_YELLOW = 131,

    LOGIC_ANIM_FIREPLACE = 167,
    LOGIC_ANIM_WATER = 148,

    LOGIC_ENEMY_BAT_E = 159,
    LOGIC_ENEMY_BAT_N = 165,
    LOGIC_ENEMY_BAT_S = 147,
    LOGIC_ENEMY_BAT_W = 153,
    LOGIC_ENEMY_BLOB_E = 135,
    LOGIC_ENEMY_BLOB_N = 141,
    LOGIC_ENEMY_BLOB_S = 123,
    LOGIC_ENEMY_BLOB_W = 129,
    LOGIC_ENEMY_GHOST_E = 136,
    LOGIC_ENEMY_GHOST_N = 142,
    LOGIC_ENEMY_GHOST_S = 124,
    LOGIC_ENEMY_GHOST_W = 130,
    LOGIC_ENEMY_SKELETON_E = 134,
    LOGIC_ENEMY_SKELETON_N = 140,
    LOGIC_ENEMY_SKELETON_S = 122,
    LOGIC_ENEMY_SKELETON_W = 128,
    LOGIC_ENEMY_SPIDER_E = 158,
    LOGIC_ENEMY_SPIDER_N = 164,
    LOGIC_ENEMY_SPIDER_S = 146,
    LOGIC_ENEMY_SPIDER_W = 152,

    LOGIC_PORTAL_DOOR = 132,
    LOGIC_PORTAL_GATE = 126,
    LOGIC_PORTAL_STAIRS_DOWN_E = 150,
    LOGIC_PORTAL_STAIRS_DOWN_W = 138,
    LOGIC_PORTAL_STAIRS_UP_E = 144,
    LOGIC_PORTAL_STAIRS_UP_W = 156,
    LOGIC_PORTAL_WELL = 162,

    LOGIC_SWITCH_BLUE = 151,
    LOGIC_SWITCH_GREEN = 163,
    LOGIC_SWITCH_ORANGE = 157,
    LOGIC_SWITCH_RED = 145,

    LOGIC_TRAP_FIRE = 166,
    LOGIC_TRAP_LAVA = 154,
    LOGIC_TRAP_SPIKES = 160
} LogicObject;



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
    
    // these are layers that are drawn over map by update_map_objects
    pntr_image* layer_player;
    pntr_image* layer_enemies;
    pntr_image* layer_things;

    // these are spritesheets that will be used to draw objects
    // generally it;s a good idea to put them all togther on one
    // but this is how I had the map setup
    pntr_image* sprites_basictiles;
    pntr_image* sprites_characters;
    pntr_image* sprites_things;
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

    // Keyboard/Gamepad
    // if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
    //     appData->x += appData->speed * pntr_app_delta_time(app);
    // }
    // else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
    //     appData->x -= appData->speed * pntr_app_delta_time(app);
    // }
    // if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
    //     appData->y += appData->speed * pntr_app_delta_time(app);
    // }
    // else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
    //     appData->y -= appData->speed * pntr_app_delta_time(app);
    // }

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

                // this will be used to draw things
                pntr_rectangle rect;

                printf("  OBJECT (%dx%d) - %d: %s (%s)\n", tileX, tileY, gid, o->name.ptr, o->type.ptr);

                if (strcmp(o->type.ptr, "player") == 0){
                    // TODO: do animation and update position
                    if (gid == LOGIC_PLAYER_N) {
                        // 39, 40, 41
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_characters, get_tile_rec(40, appData->sprites_characters), posX, posY);
                    } else if (gid == LOGIC_PLAYER_S) {
                        // 3, 4, 5
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_characters, get_tile_rec(4, appData->sprites_characters), posX, posY);
                    } if (gid == LOGIC_PLAYER_E) {
                        // 27, 28, 29
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_characters, get_tile_rec(28, appData->sprites_characters), posX, posY);
                    } if (gid == LOGIC_PLAYER_W) {
                        // 15, 16, 17
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_characters, get_tile_rec(16, appData->sprites_characters), posX, posY);
                    }
                }

                else if (strcmp(o->type.ptr, "enemy") == 0){
                    int tile_id = 0;

                    if (gid == LOGIC_ENEMY_BAT_E || gid == LOGIC_ENEMY_BAT_N || gid == LOGIC_ENEMY_BAT_S || gid == LOGIC_ENEMY_BAT_W) {

                    }
                    else if (gid == LOGIC_ENEMY_BLOB_E || gid == LOGIC_ENEMY_BLOB_N || gid == LOGIC_ENEMY_BLOB_S || gid == LOGIC_ENEMY_BLOB_W) {

                    }
                    else if (gid == LOGIC_ENEMY_GHOST_E || gid == LOGIC_ENEMY_GHOST_N || gid == LOGIC_ENEMY_GHOST_S || gid == LOGIC_ENEMY_GHOST_W) {

                    }
                    else if (gid == LOGIC_ENEMY_SKELETON_E || gid == LOGIC_ENEMY_SKELETON_N || gid == LOGIC_ENEMY_SKELETON_S || gid == LOGIC_ENEMY_SKELETON_W) {

                    }
                    else if (gid == LOGIC_ENEMY_SPIDER_E || gid == LOGIC_ENEMY_SPIDER_N || gid == LOGIC_ENEMY_SPIDER_S || gid == LOGIC_ENEMY_SPIDER_W) {

                    }

                    if (tile_id) {
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_basictiles, get_tile_rec(tile_id, appData->sprites_basictiles), posX, posY);
                        // TODO: check for near & action button
                    }

                }

                else if (strcmp(o->type.ptr, "portal") == 0){
                    int tile_id = 0;

                    if (gid == LOGIC_PORTAL_DOOR) {
                        // TODO: this is animated on activation
                        tile_id = 48;
                    }
                    else if (gid == LOGIC_PORTAL_GATE) {
                        // TODO: this is animated on activation
                        tile_id = 49;
                    }
                    else if (gid == LOGIC_PORTAL_STAIRS_DOWN_E) {
                        tile_id = 105;
                    }
                    else if (gid == LOGIC_PORTAL_STAIRS_DOWN_W) {
                        tile_id = 104;
                    }
                    else if (gid == LOGIC_PORTAL_STAIRS_UP_E) {
                        tile_id = 113;
                    }
                    else if (gid == LOGIC_PORTAL_STAIRS_UP_W) {
                        tile_id = 112;
                    }
                    else if (gid == LOGIC_PORTAL_WELL) {
                        tile_id = 31;
                    }

                    if (tile_id) {
                        pntr_draw_image_rec(appData->layer_player, appData->sprites_basictiles, get_tile_rec(tile_id, appData->sprites_basictiles), posX, posY);
                        // TODO: check for near & action button
                    }
                }

                else if (strcmp(o->type.ptr, "trap") == 0){
                    // TODO: you can figure out the trap tiles from the gid.
                }

                else if (strcmp(o->type.ptr, "loot") == 0){
                    // TODO: you can figure out the loot prize from the name.
                }

                else if (strcmp(o->type.ptr, "switch") == 0){
                    // TODO: you can figure out the switch tiles from the gid, the variable it toggles is the name.
                }

                else if (strcmp(o->type.ptr, "animaton") == 0){
                    // TODO: you can figure out the animation tiles from the gid.
                }

                else if (strcmp(o->type.ptr, "sign") == 0){
                    char* text;
                    for (int i=0; i < o->property_count; i++) {
                        cute_tiled_property_t* p = o->properties + i;
                        if (strcmp(p->name.ptr, "text") == 0 && p->type == CUTE_TILED_PROPERTY_STRING){
                            printf("%s\n---\n", p->data.string.ptr);
                            pntr_draw_image_rec(appData->layer_things, appData->sprites_basictiles, get_tile_rec(67, appData->sprites_basictiles), posX, posY);
                            // TODO: & handle trigger & show text
                        }
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

    appData->map = pntr_load_tiled("resources/rpg/welcome.tmj");
    appData->x = 0;
    appData->y = 0;
    appData->speed = 200;

    appData->sign_text = NULL;

    // print_map(appData->map);

    int w = appData->map->width * appData->map->tilewidth;
    int h = appData->map->height * appData->map->tileheight;

    // create layers for objects to be drawn on
    appData->layer_player = pntr_new_image(w, h);
    appData->layer_enemies = pntr_new_image(w, h);
    appData->layer_things = pntr_new_image(w, h);

    // setup all the spritesheets for objects
    // TODO: are these already loaded/quaded by map?
    appData->sprites_basictiles = pntr_load_image("resources/rpg/basictiles.png");
    appData->sprites_characters = pntr_load_image("resources/rpg/characters.png");
    appData->sprites_things = pntr_load_image("resources/rpg/things.png");

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
    pntr_draw_image(screen, appData->layer_things, appData->x, appData->y);
    pntr_draw_image(screen, appData->layer_enemies, appData->x, appData->y);
    pntr_draw_image(screen, appData->layer_player, appData->x, appData->y);

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
