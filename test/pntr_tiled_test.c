#include <assert.h>
#include <string.h>

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
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test.tmj");
        assert(map != NULL);

        // pntr_gen_image_tiled()
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

        // pntr_tiled_layer()
        {
            cute_tiled_layer_t* layer = pntr_tiled_layer(map, "Structure");
            assert(layer != NULL);

            assert(PNTR_STRCMP(layer->name.ptr, "Structure") == 0);
            assert(pntr_tiled_layer(map, "Non-Existant Layer") == NULL);
            assert(pntr_tiled_layer(map, NULL) == NULL);
        }

        // pntr_layer_tile()
        {
            cute_tiled_layer_t* layer = pntr_tiled_layer(map, "Plants");
            assert(layer != NULL);
            int gid = pntr_layer_tile(layer, 8, 2);
            assert(gid == 39);
            gid = pntr_layer_tile(layer, 2, 6);
            assert(gid == 38);
        }

        // pntr_set_layer_tile()
        {
            cute_tiled_layer_t* layer = pntr_tiled_layer(map, "Structure");
            assert(layer != NULL);
            assert(pntr_layer_tile(layer, 2, 2) == 10);
            pntr_set_layer_tile(layer, 2, 2, 34);
            assert(pntr_layer_tile(layer, 2, 2) == 34);
        }

        // pntr_layer_tile_from_position()
        {
            cute_tiled_layer_t* layer = pntr_tiled_layer(map, "Plants");
            assert(layer != NULL);

            pntr_vector result = pntr_layer_tile_from_position(map, layer, 239, 110);
            assert(result.x == 7);
            assert(result.y == 3);

            result = pntr_layer_tile_from_position(map, layer, 140, 53);
            assert(result.x == 4);
            assert(result.y == 1);
        }

        // pntr_tiled_layer_count()
        {
            assert(pntr_tiled_layer_count(map) == 4);
        }

        // pntr_tiled_layer_from_index()
        {
            cute_tiled_layer_t* layer = pntr_tiled_layer_from_index(map, 2);
            assert(strcmp(layer->name.ptr, "Plants") == 0);

            layer = pntr_tiled_layer_from_index(map, -5);
            assert(layer == NULL);

            layer = pntr_tiled_layer_from_index(map, 500);
            assert(layer == NULL);
        }

        pntr_unload_tiled(map);
    }

    // assertsys
    #ifdef PNTR_ASSETSYS_IMPLEMENTATION
    {
        assetsys_t* sys = pntr_load_assetsys("resources", "/res");
        assert(sys != NULL);

        cute_tiled_map_t* map = pntr_load_tiled_from_assetsys(sys, "/res/pntr_tiled_test.tmj");
        assert(map != NULL);

        pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(actual != NULL);
        assert(pntr_save_image(actual, "pntr_tiled_test-assetsys.png"));

        pntr_image* expected = pntr_load_image("resources/expected.png");
        assert(expected != NULL);
        PNTR_ASSERT_IMAGE_EQUALS(actual, expected);
        pntr_unload_image(expected);

        pntr_unload_tiled(map);
        pntr_unload_image(actual);
        pntr_unload_assetsys(sys);
    }
    #endif

    return 0;
}
