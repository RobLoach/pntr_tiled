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

/**
 * Find the bounding box of everything drawn inside the given window of an image.
 *
 * Used to measure where a rotated object actually landed, so that it can be checked against
 * the bounds worked out from Tiled's own rotation rather than against a golden image.
 */
static void find_drawn_bounds(pntr_image* image, int windowX, int windowY, int windowWidth, int windowHeight, int* minX, int* minY, int* maxX, int* maxY) {
    *minX = *minY = 1 << 30;
    *maxX = *maxY = -(1 << 30);

    for (int y = windowY; y < windowY + windowHeight; y++) {
        for (int x = windowX; x < windowX + windowWidth; x++) {
            if (pntr_image_get_color(image, x, y).rgba.a == 0) {
                continue;
            }
            if (x < *minX) *minX = x;
            if (x > *maxX) *maxX = x;
            if (y < *minY) *minY = y;
            if (y > *maxY) *maxY = y;
        }
    }
}

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
            assert(pntr_tiled_layer_count(map) == 5);
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

        // Group layers and object layers
        {
            cute_tiled_layer_t* group = pntr_tiled_layer(map, "Group");
            assert(group != NULL);
            assert(PNTR_STRCMP(group->type.ptr, "group") == 0);
            assert(group->offsetx == 10);
            assert(group->offsety == 20);

            // The group wraps a plain object layer, and one with a "color". Note that
            // cute_tiled leaves a group's child layers in reverse file order, so they are
            // looked up by name here.
            cute_tiled_layer_t* objects = NULL;
            cute_tiled_layer_t* tinted = NULL;
            int childCount = 0;
            for (cute_tiled_layer_t* child = group->layers; child; child = child->next) {
                assert(PNTR_STRCMP(child->type.ptr, "objectgroup") == 0);
                if (PNTR_STRCMP(child->name.ptr, "Objects") == 0) {
                    objects = child;
                }
                else if (PNTR_STRCMP(child->name.ptr, "Tinted Objects") == 0) {
                    tinted = child;
                }
                childCount++;
            }

            assert(childCount == 2);
            assert(objects != NULL);
            assert(tinted != NULL);
            assert(objects->tintcolor == 0);
            assert(tinted->tintcolor == 0xFFFFFB00);

            // pntr_tiled_get_object()
            cute_tiled_object_t* rect = pntr_tiled_get_object(objects, "rect");
            assert(rect != NULL);
            assert(rect->x == 300);
            assert(rect->y == 20);
            assert(rect->width == 40);
            assert(rect->height == 30);
            assert(rect->visible);

            cute_tiled_object_t* ellipse = pntr_tiled_get_object(objects, "ellipse");
            assert(ellipse != NULL);
            assert(ellipse->ellipse);

            cute_tiled_object_t* polygon = pntr_tiled_get_object(objects, "polygon");
            assert(polygon != NULL);
            assert(polygon->vert_type == 1);
            assert(polygon->vert_count == 3);

            cute_tiled_object_t* tile = pntr_tiled_get_object(objects, "tile");
            assert(tile != NULL);
            assert(tile->gid == 10);

            cute_tiled_object_t* hidden = pntr_tiled_get_object(objects, "hidden");
            assert(hidden != NULL);
            assert(!hidden->visible);

            // A full turn, which has to land exactly where an unrotated object would.
            cute_tiled_object_t* rotated = pntr_tiled_get_object(objects, "rotated");
            assert(rotated != NULL);
            assert(rotated->rotation == 360);

            cute_tiled_object_t* rotated45 = pntr_tiled_get_object(objects, "rotated45");
            assert(rotated45 != NULL);
            assert(rotated45->rotation == 45);

            cute_tiled_object_t* rotated90 = pntr_tiled_get_object(objects, "rotated90");
            assert(rotated90 != NULL);
            assert(rotated90->rotation == 90);

            assert(pntr_tiled_get_object(objects, "Non-Existant Object") == NULL);
            assert(pntr_tiled_get_object(NULL, "rect") == NULL);
        }

        // Rotated objects have to land where Tiled puts them, which is a clockwise turn
        // about the object's own x/y. These bounds come from rotating the corners by hand
        // rather than from whatever the renderer happens to produce:
        //
        //   rotated45: 40x20 at (130, 140) after the group's (10, 20) offset. Turning the
        //              corners 45 degrees clockwise gives x from -14.14 to 28.28 and y from
        //              0 to 42.43, so x [115.86..158.28] and y [140..182.43].
        //   rotated90: 40x20 at (230, 140). A quarter turn gives x from -20 to 0 and y from
        //              0 to 40, so x [210..230] and y [140..180].
        //   rotated:   40x30 at (430, 100) turned a full circle, so exactly where an
        //              unrotated object would be: x [430..469] and y [100..129].
        //
        // The group layer is drawn on its own so the objects sit on a blank background and
        // can be measured. A few pixels of slack covers the antialiased leading corner,
        // which is far tighter than the ~14 pixel error this is guarding against.
        {
            cute_tiled_layer_t* group = pntr_tiled_layer(map, "Group");
            assert(group != NULL);

            pntr_image* layerOnly = pntr_gen_image_color(map->width * map->tilewidth, map->height * map->tileheight, PNTR_BLANK);
            assert(layerOnly != NULL);
            pntr_draw_tiled_layer(layerOnly, map, group, 0, 0, PNTR_WHITE);

            int minX, minY, maxX, maxY;
            const int slack = 3;

            find_drawn_bounds(layerOnly, 100, 125, 80, 75, &minX, &minY, &maxX, &maxY);
            assert(abs(minX - 116) <= slack);
            assert(abs(maxX - 158) <= slack);
            assert(abs(minY - 140) <= slack);
            assert(abs(maxY - 182) <= slack);

            find_drawn_bounds(layerOnly, 200, 130, 60, 60, &minX, &minY, &maxX, &maxY);
            assert(abs(minX - 210) <= slack);
            assert(abs(maxX - 230) <= slack);
            assert(abs(minY - 140) <= slack);
            assert(abs(maxY - 180) <= slack);

            // A full turn has to put the object exactly where an unrotated one would, so
            // its top left corner is asserted exactly. The far edge gets a pixel of slack:
            // bilinear sampling at a full turn bleeds a faint column past the 40x30 shape,
            // which is a property of the rotated draw rather than a placement error.
            find_drawn_bounds(layerOnly, 420, 90, 70, 60, &minX, &minY, &maxX, &maxY);
            assert(minX == 430);
            assert(minY == 100);
            assert(abs(maxX - 469) <= 1);
            assert(abs(maxY - 129) <= 1);

            pntr_unload_image(layerOnly);
        }

        // pntr_draw_tiled_layer_objectlayer()
        {
            pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
            assert(actual != NULL);

            // Every shape on the untinted object layer draws in the tint it was given, and
            // not in opaque black. The group layer offsets them all by (10, 20).
            assert(pntr_image_get_color(actual, 320, 50).value == PNTR_WHITE.value);
            assert(pntr_image_get_color(actual, 390, 55).value == PNTR_WHITE.value);
            assert(pntr_image_get_color(actual, 330, 115).value == PNTR_WHITE.value);
            assert(pntr_image_get_color(actual, 450, 115).value == PNTR_WHITE.value);

            // The ellipse and the polygon only cover part of their bounding box, so their
            // corners are left to the layers underneath.
            assert(pntr_image_get_color(actual, 371, 41).value != PNTR_WHITE.value);
            assert(pntr_image_get_color(actual, 311, 118).value != PNTR_WHITE.value);

            // A hidden object is not drawn at all.
            assert(pntr_image_get_color(actual, 440, 50).value != PNTR_WHITE.value);

            // An object layer with a "color" multiplies it into the shape colors.
            pntr_color tinted = pntr_image_get_color(actual, 390, 115);
            assert(tinted.rgba.r == 0xFF);
            assert(tinted.rgba.g == 0xFB);
            assert(tinted.rgba.b == 0x00);

            // ... but not into its tile objects, so that a tile placed as an object looks the
            // same as the same tile placed on a tile layer.
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 32; x++) {
                    pntr_color plain = pntr_image_get_color(actual, 310 + x, 188 + y);
                    pntr_color onTinted = pntr_image_get_color(actual, 430 + x, 188 + y);
                    assert(plain.value == onTinted.value);
                }
            }

            pntr_unload_image(actual);
        }

        // pntr_tiled_tile_image()
        {
            assert(pntr_tiled_tile_image(map, 1) != NULL);
            assert(pntr_tiled_tile_image(map, 0) == NULL);
            assert(pntr_tiled_tile_image(map, -3) == NULL);

            // A gid past the end of the map's tiles must not be read.
            assert(pntr_tiled_tile_image(map, 49) == NULL);
            assert(pntr_tiled_tile_image(map, 100000) == NULL);
            assert(pntr_tiled_tile_image(NULL, 1) == NULL);
        }

        pntr_unload_tiled(map);
    }

    // A "collection of images" tileset reports "columns": 0, which must not divide by zero.
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test_collection.tmj");
        assert(map != NULL);

        // Per-tile images are not supported, so every tile renders blank.
        assert(pntr_tiled_tile_image(map, 1) == NULL);
        assert(pntr_tiled_tile_image(map, 2) == NULL);

        pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(actual != NULL);
        assert(actual->width == 64);
        assert(actual->height == 64);
        assert(pntr_image_get_color(actual, 10, 10).rgba.a == 0);

        pntr_unload_image(actual);
        pntr_unload_tiled(map);
    }

    // A tileset whose image is missing must not copy from a NULL subimage.
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test_missingimage.tmj");
        assert(map != NULL);

        assert(pntr_tiled_tile_image(map, 1) == NULL);

        pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(actual != NULL);
        assert(pntr_image_get_color(actual, 10, 10).rgba.a == 0);

        pntr_unload_image(actual);
        pntr_unload_tiled(map);
    }

    // An infinite map keeps its tiles in chunks instead of a single array.
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test_infinite.tmj");
        assert(map != NULL);
        assert(map->infinite);

        cute_tiled_layer_t* layer = pntr_tiled_layer(map, "Infinite");
        assert(layer != NULL);
        assert(layer->data == NULL);
        assert(layer->chunks != NULL);

        int chunkCount = 0;
        for (cute_tiled_chunk_t* chunk = layer->chunks; chunk; chunk = chunk->next) {
            chunkCount++;
        }
        assert(chunkCount == 2);

        // pntr_gen_image_tiled() sizes an infinite map from the bounding box of its chunks.
        // The chunks span tile columns -16 through 31 and tile rows 0 through 15.
        {
            pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
            assert(actual != NULL);
            assert(actual->width == 48 * 32);
            assert(actual->height == 16 * 32);
            assert(pntr_save_image(actual, "actual_infinite.png"));

            pntr_image* expected = pntr_load_image("resources/expected_infinite.png");
            assert(expected != NULL);
            PNTR_ASSERT_IMAGE_EQUALS(actual, expected);

            pntr_unload_image(expected);
            pntr_unload_image(actual);
        }

        // pntr_layer_tile() reads through to the chunk that holds the tile, including chunks
        // at negative coordinates.
        assert(pntr_layer_tile(layer, -3, 0) == 1);
        assert(pntr_layer_tile(layer, -2, 0) == 30);
        assert(pntr_layer_tile(layer, -1, 2) == 30);
        assert(pntr_layer_tile(layer, 16, 0) == 10);
        assert(pntr_layer_tile(layer, 17, 1) == 30);

        // An empty tile inside a chunk, and anything no chunk covers, is 0.
        assert(pntr_layer_tile(layer, -3, 8) == 0);
        assert(pntr_layer_tile(layer, 0, 0) == 0);
        assert(pntr_layer_tile(layer, -100, 0) == 0);
        assert(pntr_layer_tile(layer, 100, 100) == 0);

        // pntr_set_layer_tile()
        pntr_set_layer_tile(layer, -3, 0, 34);
        assert(pntr_layer_tile(layer, -3, 0) == 34);

        // A tile that no chunk covers cannot be set, since that would grow the map.
        pntr_set_layer_tile(layer, 0, 0, 34);
        assert(pntr_layer_tile(layer, 0, 0) == 0);

        pntr_unload_tiled(map);
    }

    // The map's backgroundcolor is the raw file hex, not 0xRRGGBBAA.
    {
        cute_tiled_map_t* map = pntr_load_tiled("resources/pntr_tiled_test_background.tmj");
        assert(map != NULL);

        // Note that PNTR_RED is not a pure red, so the channels are checked directly.
        pntr_image* actual = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(actual != NULL);

        pntr_color background = pntr_image_get_color(actual, 10, 10);
        assert(background.rgba.r == 0xFF);
        assert(background.rgba.g == 0x00);
        assert(background.rgba.b == 0x00);
        assert(background.rgba.a == 0xFF);

        pntr_unload_image(actual);
        pntr_unload_tiled(map);

        map = pntr_load_tiled("resources/pntr_tiled_test_background_alpha.tmj");
        assert(map != NULL);

        actual = pntr_gen_image_tiled(map, PNTR_WHITE);
        assert(actual != NULL);

        background = pntr_image_get_color(actual, 10, 10);
        assert(background.rgba.a == 0x80);
        assert(background.rgba.r == 0xFF);
        assert(background.rgba.g == 0x00);
        assert(background.rgba.b == 0x00);

        pntr_unload_image(actual);
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
