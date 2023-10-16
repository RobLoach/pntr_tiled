#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"

// #define PNTR_ASSETSYS_IMPLEMENTATION
// #include "pntr_assetsys.h"

#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

// Assertion library
#include "pntr_assert.h"

int main() {
    // pntr_load_tiled()
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test.json");
        assert(map != NULL);

        {
            pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
            assert(actual != NULL);
            assert(pntr_save_image(actual, "actual.png"));

            pntr_image* expected = pntr_load_image("resources/expected.png");
            assert(expected != NULL);
            PNTR_ASSERT_IMAGE_EQUALS(actual, expected);

            pntr_unload_image(expected);
            pntr_unload_image(actual);
        }

        pntr_unload_tiled(map);
    }

    // assertsys
    #ifdef PNTR_ASSETSYS_IMPLEMENTATION
    {
        assetsys_t* sys = pntr_load_assetsys("resources", "/res");
        cute_tiled_map_t* map = pntr_load_tiled_from_assetsys(sys, "/res/pntr_tiled_test.json");
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
