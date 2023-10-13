#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"

//#define PNTR_ASSETSYS_IMPLEMENTATION
//#include "pntr_assetsys.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

int main() {
    // pntr_load_tiled()
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/desert.json");
        assert(map != NULL);

        {
            pntr_image* image = pntr_gen_image_tiled(map, PNTR_WHITE);
            assert(image != NULL);
            assert(pntr_save_image(image, "pntr_tiled_test.png"));
            pntr_unload_image(image);
        }

        pntr_unload_tiled(map);
    }

    // assertsys
    #ifdef PNTR_ASSETSYS_IMPLEMENTATION
    {
        assetsys_t* sys = pntr_load_assetsys("resources", "/res");
        cute_tiled_map_t* map = pntr_load_tiled_from_assetsys(sys, "/res/desert.json");
        assert(map != NULL);

        pntr_image* image = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(image != NULL);
        assert(pntr_save_image(image, "pntr_tiled_test-assetsys.png"));

        pntr_unload_tiled(map);
        pntr_unload_image(image);
        pntr_unload_assetsys(sys);
    }
    #endif

    return 0;
}
