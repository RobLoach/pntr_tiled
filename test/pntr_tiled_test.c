#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

int main() {
    cute_tiled_map_t* map = pntr_load_tiled("resources/desert.json");
    assert(map != NULL);

    pntr_image* image = pntr_gen_image_tiled(map, PNTR_WHITE);
    assert(image != NULL);
    assert(pntr_save_image(image, "output.png"));

    pntr_unload_tiled(map);
    pntr_unload_image(image);

    return 0;
}
