#include <stdio.h>
#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

int main() {

    cute_tiled_map_t* map = pntr_load_tiled("resources/desert.json");
    assert(map);

    pntr_image* image = pntr_gen_image_color(map->tilewidth * map->width, map->tileheight * map->height, PNTR_BLACK);
    pntr_draw_tiled(image, map, 0, 0, PNTR_WHITE);
    pntr_save_image(image, "output.png");

    pntr_unload_tiled(map);

    return 0;
}
