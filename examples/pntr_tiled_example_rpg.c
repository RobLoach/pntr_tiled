#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// just some lil utils to see what is in map
#include "debug_map.h"

#define DEBUG_COLLISION 1

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

  cute_tiled_layer_t* layer_objects;
  cute_tiled_layer_t* layer_collisions;
} AppData;

// unload current map and switch to another one
void map_portal(char* name, AppData* appData) {
  // reset so it will be pulled from initial position/direction in map
  appData->playerX = 0;
  appData->playerY = 0;

  appData->x = 0;
  appData->y = 0;
  appData->sign_text = NULL;

  appData->activatableObject = NULL;

  pntr_unload_tiled(appData->map);

  appData->activatableObject = NULL;

  char fname[80];
  sprintf(fname, "resources/rpg/%s.tmj", name);
  appData->map = pntr_load_tiled(fname);

  // find layers for logic-object/collision
  appData->layer_objects = NULL;
  appData->layer_collisions = NULL;

  cute_tiled_layer_t* layer = appData->map->layers;
  while(layer) {
    if (strcmp(layer->name.ptr, "collision") == 0) {
      appData->layer_collisions = layer;
    } else if (strcmp(layer->name.ptr, "logic") == 0) {
      appData->layer_objects = layer;

      // find player and set intial position/direction
      cute_tiled_object_t* o = layer->objects;
      while (o) {
        if (strcmp(o->type.ptr, "player") == 0) {
          appData->playerX = (int) o->x;
          appData->playerY = (int) o->y;
          appData->playerDirection = o->gid/3;
          break;
        }
        o = o->next;
      }
    }
    if (appData->layer_objects != NULL && appData->layer_collisions != NULL) {
      break;
    }
    layer = layer->next;
  }
}

// given a tile-number (0-indexed) return rec of image
pntr_rectangle get_tile_rec(int id, pntr_image* src) {
  int tw = src->width / 16;
  pntr_rectangle r = { .x=(id % tw) * 16, .y=(id / tw) * 16, .width=16, .height=16 };
  return r;
}

// check if 2 rectangles are colliding
bool collision_check(pntr_rectangle a, pntr_rectangle b) {
  bool collision = false;
  do {
    if (a.y > (b.y + b.height)) break;
    if (b.y > (a.y + a.height)) break;
    if (a.x > (b.x + b.width)) break;
    if (b.x > (a.x + a.width)) break;
    collision = true;
  } while (false);
  return collision;
}

// check player collision of all collision objects
bool world_collision_check(AppData* appData, pntr_rectangle playerHitZone) {
  cute_tiled_object_t* o = appData->layer_collisions->objects;

  pntr_rectangle orect = { .x=0, .y=0, .width=0, .height=0 };
  bool collision = false;

  while (o) {
    orect.x = o->x;
    orect.y = o->y;
    orect.width = o->width;
    orect.height = o->height;

    collision = collision_check(playerHitZone, orect);

    if (DEBUG_COLLISION){
      if (collision) {
        pntr_draw_rectangle_fill_rec(appData->objects, orect, PNTR_PINK);
      } else {
        pntr_draw_rectangle_fill_rec(appData->objects, orect, PNTR_BLUE);
      }
    }

    if (collision) {
      return true;
    }
    o = o->next;
  }

  return false;
}

// this will update/draw all the map-objects, including player, based on state of things
void update_map_objects(AppData* appData) {
  if (appData->layer_objects != NULL) {
    cute_tiled_object_t* o = appData->layer_objects->objects;
    pntr_rectangle orect = { .x=0, .y=0, .width=16*3, .height=16*3 };
    pntr_rectangle playerHitZone = { .x=appData->playerX + 4, .y=appData->playerY-4, .height=4, .width=8 };
    bool collision = false;
    cute_tiled_property_t* p;
    bool touchObject;

    appData->activatableObject = NULL;

    while (o) {
      if (strcmp(o->type.ptr, "player") == 0) {
        // draw moved to anotehr operation, so nothing to do here
      }

      else {
        orect.x = o->x - 16;
        orect.y = o->y - 32;
        orect.width = 16*3;
        orect.height = 16*3;

        touchObject = false;
        for (int i = 0; i < o->property_count; ++i) {
          p = o->properties + i;
          if (strcmp(p->name.ptr, "touch") == 0 && p->data.boolean) {
            touchObject = true;
            orect.x = o->x;
            orect.y = o->y
            orect.width = 16;
            orect.height = 16;
            break;
          }
        }

        collision = collision_check(playerHitZone, orect);

        if (DEBUG_COLLISION){
          if (collision) {
            pntr_draw_rectangle_fill_rec(appData->objects, orect, PNTR_GREEN);
          } else {
            pntr_draw_rectangle_fill_rec(appData->objects, orect, PNTR_YELLOW);
          }
        }

        // handle player activation (touch/action-button)
        if (collision) {
          if (touchObject || pntr_app_key_pressed(app, PNTR_APP_KEY_X) || pntr_app_gamepad_button_pressed(app, 0, PNTR_APP_GAMEPAD_BUTTON_A)) {
            activate_current(o);
          }
        }

        // generic: draw whatever tile
        pntr_draw_image_rec(appData->objects, appData->sprites, get_tile_rec(o->gid-1, appData->sprites), (int) o->x, (int) o->y-16);
      }

      o = o->next;
    }
  }
}

// called on activation
void activate_current(AppData* appData, cute_tiled_object_t* activatableObject){
  if (activatableObject!=NULL) {
    printf("ACTION: %s (%s)\n", activatableObject->name.ptr, activatableObject->type.ptr);
    if (strcmp(activatableObject->type.ptr, "portal") == 0) {
      map_portal(activatableObject->name.ptr, appData);
    }
  }
}

bool Init(pntr_app* app) {
  AppData* appData = pntr_load_memory(sizeof(AppData));
  pntr_app_set_userdata(app, appData);

  appData->speed = 100;

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

  // handle walking animation
  int frame = 0;

  if (appData->playerWalking) {
    frame = ((int)(appData->gameTime * appData->speed/30) % 2) + 1;
  }

  // draw player
  pntr_draw_image_rec(appData->objects, appData->sprites, get_tile_rec((appData->playerDirection * 3) + frame, appData->sprites), appData->playerX, appData->playerY-16);

  pntr_rectangle playerHitZone = { .x=appData->playerX + 4, .y=appData->playerY-4, .height=4, .width=8 };

  if (DEBUG_COLLISION) {
    pntr_draw_rectangle_fill_rec(appData->objects, playerHitZone, PNTR_RED);
  }

  // Keyboard/Gamepad
  bool anyDirectionKey = false;
  if (pntr_app_key_down(app, PNTR_APP_KEY_LEFT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_LEFT)) {
    playerHitZone.x -= appData->speed * pntr_app_delta_time(app);
    if (!world_collision_check(appData, playerHitZone)) {
      appData->playerX = playerHitZone.x - 4;
    }
    anyDirectionKey = true;
    appData->playerDirection = FACE_WEST;
  }
  else if (pntr_app_key_down(app, PNTR_APP_KEY_RIGHT) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_RIGHT)) {
    playerHitZone.x += appData->speed * pntr_app_delta_time(app);
    if (!world_collision_check(appData, playerHitZone)) {
      appData->playerX = playerHitZone.x - 4;
    }
    anyDirectionKey = true;
    appData->playerDirection = FACE_EAST;
  }

  if (pntr_app_key_down(app, PNTR_APP_KEY_UP) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_UP)) {
    playerHitZone.y -= appData->speed * pntr_app_delta_time(app);
    if (!world_collision_check(appData, playerHitZone)) {
      appData->playerY = playerHitZone.y + 4;
    }
    anyDirectionKey = true;
    appData->playerDirection = FACE_NORTH;
  }
  else if (pntr_app_key_down(app, PNTR_APP_KEY_DOWN) || pntr_app_gamepad_button_down(app, 0, PNTR_APP_GAMEPAD_BUTTON_DOWN)) {
    playerHitZone.y += appData->speed * pntr_app_delta_time(app);
    if (!world_collision_check(appData, playerHitZone)) {
      appData->playerY = playerHitZone.y + 4;
    }
    anyDirectionKey = true;
    appData->playerDirection = FACE_SOUTH;
  }
  appData->playerWalking = anyDirectionKey;

  // draw all map objects
  pntr_draw_image(screen, appData->objects, appData->x, appData->y);

  pntr_update_tiled(appData->map, pntr_app_delta_time(app));

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
