/**********************************************************************************************
*
*   pntr_tiled - Parse Tiled (http://www.mapeditor.org) files saved as JSON for pntr.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       cute_tiled https://github.com/RandyGaul/cute_headers/blob/master/cute_tiled.h
*
*   DEVELOPER NOTES:
*       There are a few cute_tiled_map_t properties that are used for pntr_tiled use:
*       - tiledversion: Used for an array of pntr_image* subimages representing each tile source in the map.
*       - nextlayerid: Used to track the current animation time in milliseconds.
*       - nextobjectid: Used to track how many tiles are in the tiledversion array.
*
*   LICENSE: zlib/libpng
*
*   pntr_tiled is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef PNTR_TILED_H_
#define PNTR_TILED_H_

// cute_tiled
#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#ifndef PNTR_TILED_CUTE_TILED_H
#define PNTR_TILED_CUTE_TILED_H "cute_tiled.h"
#endif
#include PNTR_TILED_CUTE_TILED_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PNTR_TILED_API
    #define PNTR_TILED_API PNTR_API
#endif

/**
 * Load a Tiled map that is exported as a JSON file.
 *
 * @param fileName The name of the file to load.
 * @return The loaded map data, or NULL on failure.
 */
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled(const char* fileName);
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir);
PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map);
PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint);

/**
 * Draw a tile from the map onto the provided image destination.
 *
 * @param dst The destination of where to draw the tile.
 * @param map The map from which to get the tile.
 * @param gid The global tile ID for the tile. When an animation is applied to the tile, it will get the tile for the active animation frame.
 * @param posX The position to draw the tile along the X coordinate.
 * @param posY The position to draw the tile along the Y coordinate.
 * @param tint The color to tint the tile when drawing.
 */
PNTR_TILED_API void pntr_draw_tiled_tile(pntr_image* dst, cute_tiled_map_t* map, int gid, int posX, int posY, pntr_color tint);
/**
 * Draw the given layer, and every layer that follows it, onto the provided image destination.
 *
 * @details Hidden layers, fully transparent layers and layers with an unknown type are
 * skipped. A group layer draws its child layers, offset by the group's own offset.
 *
 * @param dst The destination of where to draw the layers.
 * @param map The map that the layer belongs to.
 * @param layer The first layer to draw. Every layer linked after it is drawn too.
 * @param posX The position to draw the layers along the X coordinate.
 * @param posY The position to draw the layers along the Y coordinate.
 * @param tint The color to tint the layers when drawing.
 */
PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);

PNTR_TILED_API void pntr_draw_tiled_layer_imagelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer_tilelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);

/**
 * Draw all the visible objects of an object layer onto the provided image destination.
 *
 * @details Rectangle, ellipse and polygon objects have no image of their own, so they are
 * filled with a solid color. That color is the layer's own color multiplied into the given
 * tint. In Tiled an object layer's "color" is the editor display color used to draw the
 * outlines of its shapes, and cute_tiled reads it into the layer's tintcolor, so it is the
 * closest match available to what the editor previews. A layer with no color set is drawn in
 * the unmodified tint.
 *
 * Tile objects are drawn from their tile image through pntr_draw_tiled_tile(), using the
 * unmodified tint. The layer color is not applied to them, so that a tile placed as an object
 * looks the same as the same tile placed on a tile layer.
 *
 * @param dst The destination of where to draw the layer.
 * @param map The map that the layer belongs to.
 * @param layer The object layer to draw.
 * @param posX The position to draw the layer along the X coordinate.
 * @param posY The position to draw the layer along the Y coordinate.
 * @param tint The color to tint the layer when drawing.
 */
PNTR_TILED_API void pntr_draw_tiled_layer_objectlayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);

/**
 * Retrieves an image representing the desired tile from the given global tile ID.
 *
 * @param map The map to get the tile from.
 * @param gid The global tile ID for the tile.
 *
 * @return A subimage from the tileset for the given tile, or NULL when the gid is out of range
 * or the tile has no image. Tiles have no image when their tileset has no tile grid, such as a
 * "collection of images" tileset, or when the tileset image failed to load.
 */
PNTR_TILED_API pntr_image* pntr_tiled_tile_image(cute_tiled_map_t* map, int gid);

/**
 * Generate an image of the given Tiled map.
 *
 * @param map The map to build an image of.
 * @param tint What color to tint the map.
 * @return An image representing the rendered map, or NULL on failure.
 */
PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint);
PNTR_TILED_API pntr_image* pntr_gen_image_tiled_layer(cute_tiled_map_t* map, cute_tiled_layer_t* layer, pntr_color tint);

/**
 * Update the internal animation frame time counter for the map.
 *
 * @param map The map to update.
 * @param deltaTime The amount of time that changed from the last update, in seconds.
 */
PNTR_TILED_API void pntr_update_tiled(cute_tiled_map_t* map, float deltaTime);

/**
 * Get a layer from its name.
 *
 * @param map The map to search through.
 * @param name The name of the layer to return.
 *
 * @return The layer if found, NULL otherwise.
 */
PNTR_TILED_API cute_tiled_layer_t* pntr_tiled_layer(cute_tiled_map_t* map, const char* name);

/**
 * Get the gid of the tile at the given column and row.
 *
 * @param layer The layer to get the tile of.
 * @param column The x coordinate of the tile.
 * @param row The y coordinate of the tile.
 *
 * @return The gid of the given tile, without its flip status.
 */
PNTR_TILED_API int pntr_layer_tile(cute_tiled_layer_t* layer, int column, int row);

/**
 * Set the gid of the tile at the given column and row.
 *
 * @param layer The layer to set the tile of.
 * @param column The x coordinate of the tile.
 * @param row The y coordinate of the tile.
 * @param gid The desired gid to set the tile to.
 */
PNTR_TILED_API void pntr_set_layer_tile(cute_tiled_layer_t* layer, int column, int row, int gid);

/**
 * Retrieve the tile column/row that appears at the given X/Y position.
 *
 * @return The tile's column and row as a vector.
 */
PNTR_TILED_API pntr_vector pntr_layer_tile_from_position(cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY);

PNTR_TILED_API cute_tiled_layer_t* pntr_tiled_layer_from_index(cute_tiled_map_t* map, int i);
PNTR_TILED_API int pntr_tiled_layer_count(cute_tiled_map_t* map);

/**
 * Return the pntr_color for a tiled-color (like what is used in map background property)
 *
 * @return the pntr_color
 */
PNTR_TILED_API pntr_color pntr_tiled_color(uint32_t color);

/**
 * Find an object by name on an object-layer
 *
 * @return the cute_tiled_object
 */
PNTR_TILED_API cute_tiled_object_t* pntr_tiled_get_object(cute_tiled_layer_t* objects_layer, const char* name);

#ifdef PNTR_ASSETSYS_API
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_assetsys(assetsys_t* sys, const char* fileName);
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_H_

#ifdef PNTR_TILED_IMPLEMENTATION
#ifndef PNTR_TILED_IMPLEMENTATION_ONCE
#define PNTR_TILED_IMPLEMENTATION_ONCE

#if !defined(PNTR_UNUSED)
    #define PNTR_UNUSED(x) (void)x
#endif

#ifndef PNTR_STRRCHR
    #include <string.h>
    #define PNTR_STRRCHR strrchr
#endif

/**
 * Log a printf-style message.
 *
 * Does nothing by default so that this header does not pull in stdio.h. Define
 * PNTR_TILED_ENABLE_LOG for a printf-based implementation, or define PNTR_TILED_LOG to route
 * the messages somewhere else.
 */
#ifndef PNTR_TILED_LOG
    #ifdef PNTR_TILED_ENABLE_LOG
        #include <stdio.h>
        #define PNTR_TILED_LOG(...) printf(__VA_ARGS__)
    #else
        #define PNTR_TILED_LOG(...) ((void)0)
    #endif
#endif

#ifndef PNTR_TILED_QSORT
    #include <stdlib.h>
    #define PNTR_TILED_QSORT qsort
#endif

#ifndef PNTR_STRCMP
    #include <string.h>
    #define PNTR_STRCMP strcmp
#endif

#ifndef PNTR_STRCAT
    #include <string.h>
    #define PNTR_STRCAT strcat
#endif

#ifndef PNTR_STRLEN
    #include <string.h>
    #define PNTR_STRLEN strlen
#endif

#ifndef PNTR_PATH_MAX
    #ifdef PATH_MAX
        #define PNTR_PATH_MAX PATH_MAX
    #else
        #define PNTR_PATH_MAX 1024
    #endif
#endif

#ifndef STRPOOL_EMBEDDED_ASSERT
    #ifdef PNTR_ASSERT
        #define STRPOOL_EMBEDDED_ASSERT PNTR_ASSERT
    #endif
#endif

#ifndef STRPOOL_EMBEDDED_MEMSET
    #define STRPOOL_EMBEDDED_MEMSET( ptr, val, cnt ) ( PNTR_MEMSET( ptr, val, cnt ) )
#endif

#ifndef STRPOOL_EMBEDDED_MEMCPY
    #define STRPOOL_EMBEDDED_MEMCPY( dst, src, cnt ) ( PNTR_MEMCPY( dst, src, cnt ) )
#endif

#ifndef STRPOOL_EMBEDDED_MEMCMP
    //#include <string.h>
    //#define STRPOOL_EMBEDDED_MEMCMP( pr1, pr2, cnt ) ( memcmp( pr1, pr2, cnt ) )
#endif

#ifndef STRPOOL_EMBEDDED_STRNICMP
    // #ifdef _WIN32
    //     #include <string.h>
    //     #define STRPOOL_EMBEDDED_STRNICMP( s1, s2, len ) ( _strnicmp( s1, s2, len ) )
    // #else
    //     #include <string.h>
    //     #define STRPOOL_EMBEDDED_STRNICMP( s1, s2, len ) ( strncasecmp( s1, s2, len ) )
    // #endif
#endif

// cute_tiled
#define CUTE_TILED_IMPLEMENTATION
#define CUTE_TILED_ALLOC(size, ctx) pntr_load_memory(size)
#define CUTE_TILED_FREE(mem, ctx) pntr_unload_memory(mem)
#define CUTE_TILED_MEMCPY PNTR_MEMCPY
#define CUTE_TILED_MEMSET PNTR_MEMSET
#define CUTE_TILED_SEEK_SET 0
#define CUTE_TILED_SEEK_END 0
#define CUTE_TILED_FILE void
#define CUTE_TILED_FOPEN(path, state) (void*)path
#define CUTE_TILED_FSEEK(fp, num, seekpart)
#define CUTE_TILED_FREAD(data, sz, num, fp)
#define CUTE_TILED_FTELL(fp) 0
#define CUTE_TILED_FCLOSE(fp) (void)fp

// cute_tiled defines its internal helpers without prototypes, so -Wmissing-prototypes is
// silenced across the vendored implementation rather than patching the vendored header.
#if !defined(__cplusplus) && (defined(__GNUC__) || defined(__clang__))
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-prototypes"
#endif
#include PNTR_TILED_CUTE_TILED_H
#if !defined(__cplusplus) && (defined(__GNUC__) || defined(__clang__))
    #pragma GCC diagnostic pop
#endif

/**
 * Internal pntr_tiled data for tiles within map tilesets.
 *
 * Will be saved into map->tiledversion, and managed internally.
 *
 * @private
 * @internal
 */
typedef struct pntr_tiled_tile {
    pntr_image image;
    cute_tiled_tile_descriptor_t* descriptor;
    int animationDuration;
    cute_tiled_tileset_t* tileset;
} pntr_tiled_tile;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Finds the last slash in the given path.
 *
 * @internal
 * @private
 */
static char* _pntr_tiled_find_last_slash(const char* str) {
    const char *slash     = PNTR_STRRCHR(str, '/');
    const char *backslash = PNTR_STRRCHR(str, '\\');

    if (!slash || (backslash > slash)) {
        return (char*)backslash;
    }

    return (char*)slash;
}

/**
 * Retrieves the base path of the given path.
 *
 * @internal
 * @private
 */
static void _pntr_tiled_path_basedir(char* path) {
    char *last = NULL;
    if (!path || path[0] == '\0' || path[1] == '\0') {
        return;
    }

    if ((last = _pntr_tiled_find_last_slash(path))) {
        last[1] = '\0';
    }
    else {
        path[0] = '\0';
    }
}

/**
 * Perform any internal loading of map data.
 *
 * @internal
 * @private
 */
static void _pntr_load_map_data(cute_tiled_map_t* map) {
    if (map == NULL) {
        return;
    }

    // Prepare the animation counter.
    map->nextlayerid = 0;

    // Count how many tiles there are
    int tileCount = 0;
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        if (tileset->tilecount > 0) {
            tileCount += tileset->tilecount;
        }
        tileset = tileset->next;
    }

    map->tiledversion.ptr = NULL;
    map->nextobjectid = 0;
    if (tileCount <= 0) {
        return;
    }

    // Prepare the entire tiles set
    pntr_tiled_tile* tiles = (pntr_tiled_tile*)pntr_load_memory(sizeof(pntr_tiled_tile) * (size_t)tileCount);
    if (tiles == NULL) {
        return;
    }

    // Start every tile blank. A blank tile has no image data, which makes both
    // pntr_tiled_tile_image() and pntr_draw_tiled_tile() a no-op for it, so any tile that
    // can't be built below is simply never drawn.
    PNTR_MEMSET((void*)tiles, 0, sizeof(pntr_tiled_tile) * (size_t)tileCount);

    // Build all the tiles from each tileset.
    tileset = map->tilesets;
    while (tileset) {
        // A tileset needs a tile grid and a loaded image before any subimages can be cut out
        // of it. A "collection of images" tileset has no shared image and reports
        // "columns": 0, and a tileset whose image failed to load has a NULL image. Both leave
        // their tiles blank rather than dividing by zero or copying from NULL.
        if (tileset->columns <= 0 || tileset->tilewidth <= 0 || tileset->tileheight <= 0 || tileset->image.ptr == NULL) {
            tileset = tileset->next;
            continue;
        }

        for (int i = 0; i < tileset->tilecount; i++) {
            // Calculate the gid, skipping anything that would land outside the tiles array.
            int gid = tileset->firstgid + i - 1;
            if (gid < 0 || gid >= tileCount) {
                continue;
            }

            // Figure out where the tile appears in the tileset.
            int tileX = i % tileset->columns;
            int tileY = i / tileset->columns;

            // Build the source rectangle.
            pntr_rectangle srcRect = {
                .x = tileX * tileset->tilewidth + tileX * tileset->spacing + tileset->margin,
                .y = tileY * tileset->tileheight  + tileY * tileset->spacing + tileset->margin,
                .width = tileset->tilewidth,
                .height = tileset->tileheight
            };

            // Build the subimage as part of the tiles array. This returns NULL when the tile
            // sits outside of the tileset image, which leaves the tile blank.
            pntr_image* temporaryTile = pntr_image_subimage((pntr_image*)tileset->image.ptr, srcRect.x, srcRect.y, srcRect.width, srcRect.height);
            if (temporaryTile == NULL) {
                continue;
            }

            pntr_tiled_tile* tile = tiles + gid;
            tile->tileset = tileset;
            tile->descriptor = NULL;
            tile->animationDuration = 0;

            pntr_image* tileImage = &tile->image;
            pntr_memory_copy((void*)tileImage, (void*)temporaryTile, sizeof(pntr_image));
            pntr_unload_image(temporaryTile);

            // Find any tile descriptors
            cute_tiled_tile_descriptor_t* descriptor = tileset->tiles;
            while (descriptor) {
                // Find the descriptor for the active tile.
                if (descriptor->tile_index == i) {
                    tile->descriptor = descriptor;

                    // Animation: Calculate how long the full animation is.
                    for (int frameNumber = 0; frameNumber < tile->descriptor->frame_count; frameNumber++) {
                        tile->animationDuration += tile->descriptor->animation[frameNumber].duration;
                    }
                    break;
                }
                descriptor = descriptor->next;
            }
        }

        // Onto the next tileset
        tileset = tileset->next;
    }

    map->tiledversion.ptr = (const char*)tiles;
    map->nextobjectid = tileCount;
}

PNTR_TILED_API pntr_image* pntr_tiled_tile_image(cute_tiled_map_t* map, int gid) {
    // The gid is a one-based index into the tiles array, so anything past the tile count would
    // read out of bounds.
    if (map == NULL || map->tiledversion.ptr == NULL || gid <= 0 || gid > map->nextobjectid) {
        return NULL;
    }

    pntr_tiled_tile* tiles = (pntr_tiled_tile*)map->tiledversion.ptr;
    pntr_tiled_tile* tile = tiles + gid - 1;

    // Process any descriptive tile properties.
    if (tile->descriptor != NULL) {
        // Animation
        if (tile->descriptor->frame_count > 0 && tile->animationDuration > 0) {
            // Find the active frame.
            int desiredMilliseconds = map->nextlayerid % tile->animationDuration;
            int desiredFrame = 0;
            int millisecondsCounter = 0;
            for (int i = 0; i < tile->descriptor->frame_count; i++) {
                millisecondsCounter += tile->descriptor->animation[i].duration;
                if (millisecondsCounter < desiredMilliseconds) {
                    break;
                }
                desiredFrame = i;
            }

            // Switch the tile to the animation frame.
            gid = tile->tileset->firstgid + tile->descriptor->animation[desiredFrame].tileid;
            if (gid <= 0 || gid > map->nextobjectid) {
                return NULL;
            }
            tile = tiles + gid - 1;
        }
    }

    // A blank tile has no image to draw.
    if (tile->image.data == NULL) {
        return NULL;
    }

    return &tile->image;
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* data = pntr_load_file(fileName, &bytesRead);
    if (data == NULL) {
        return NULL;
    }

    // Copy the fileName, along with its null terminator to find the basePath.
    char baseDir[PNTR_PATH_MAX];
    size_t fileNameLength = PNTR_STRLEN(fileName);
    pntr_memory_copy((void*)baseDir, (void*)fileName, fileNameLength);
    baseDir[fileNameLength] = '\0';
    _pntr_tiled_path_basedir(baseDir);

    // Load the tiled map.
    cute_tiled_map_t* output = pntr_load_tiled_from_memory(data, bytesRead, baseDir);
    pntr_unload_memory(data);

    return output;
}

/**
 * Replaces the "image" with pntr_image.
 *
 * @param image The string image to replace.
 * @param baseDir The base directory where the map file was loaded.
 */
static void _pntr_load_tiled_string_texture(cute_tiled_string_t* image, const char* baseDir) {
    if (image == NULL || image->ptr == NULL) {
        return;
    }

    // TODO: Allow loading images from a base64 embedded image: https://github.com/RobLoach/TiledExportExtensions
    // Concatenate the baseDir with the image path.
    char fullPath[PNTR_PATH_MAX];
    fullPath[0] = '\0';
    PNTR_STRCAT(fullPath, baseDir);
    PNTR_STRCAT(fullPath, image->ptr);

    // Replace the image string with a pointer to the new image.
    image->ptr = (const char*)pntr_load_image(fullPath);
    if (image->ptr == NULL) {
        PNTR_TILED_LOG("pntr_tiled: Failed to load image: %s\n", fullPath);
    }
}

static void _pntr_load_tiled_layer_images(cute_tiled_layer_t* layer, const char* baseDir) {
    if (layer == NULL) {
        return;
    }

    if (PNTR_STRCMP(layer->type.ptr, "imagelayer") == 0) {
        _pntr_load_tiled_string_texture(&layer->image, baseDir);
    }
    else if (PNTR_STRCMP(layer->type.ptr, "group") == 0) {
        cute_tiled_layer_t* childLayers = layer->layers;
        while (childLayers) {
            _pntr_load_tiled_layer_images(childLayers, baseDir);
            childLayers = childLayers->next;
        }
    }
}

// this grabs external tilesets and injects them as if they are internal
static void _pntr_tiled_load_external_tilesets(cute_tiled_tileset_t* tileset, const char* baseDir) {
    if (tileset->source.ptr != NULL) {
        char fullPath[PNTR_PATH_MAX];
        fullPath[0] = '\0';
        PNTR_STRCAT(fullPath, baseDir);
        PNTR_STRCAT(fullPath, tileset->source.ptr);

        int originalFirstgid = tileset->firstgid;
        cute_tiled_tileset_t* originalNext = tileset->next;

        unsigned int bytesRead;
        unsigned char* data = pntr_load_file(fullPath, &bytesRead);
        if (data != NULL) {
            cute_tiled_tileset_t* tt = cute_tiled_load_external_tileset_from_memory(data, bytesRead, NULL);
            if (tt != NULL) {
                // TODO: is this a memleak? I think it is. How do I do it better?
                pntr_memory_copy((void*)tileset, (void*)tt, sizeof(cute_tiled_tileset_t));
                tileset->firstgid = originalFirstgid;
                tileset->next = originalNext;
            }
            pntr_unload_file(data);
        }
    }
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(fileData, (int)dataSize, 0);
    if (map == NULL) {
        return NULL;
    }

    // Load all the tileset externaal tilesets & any tileset images.
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        _pntr_tiled_load_external_tilesets(tileset, baseDir);
        _pntr_load_tiled_string_texture(&tileset->image, baseDir);
        if (tileset->transparentcolor != 0) {
            pntr_image_color_replace((pntr_image*)tileset->image.ptr, pntr_tiled_color(tileset->transparentcolor), PNTR_BLANK);
        }
        tileset = tileset->next;
    }

    // Load all image layers.
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        _pntr_load_tiled_layer_images(layer, baseDir);
        layer = layer->next;
    }

    // Load the individual tiles as subimages.
    _pntr_load_map_data(map);

    return map;
}


static void _pntr_unload_tiled_layer_images(cute_tiled_layer_t* layer) {
    if (layer == NULL) {
        return;
    }

    if (PNTR_STRCMP(layer->type.ptr, "imagelayer") == 0) {
        pntr_unload_image((pntr_image*)layer->image.ptr);
        layer->image.ptr = NULL;
    }
    else if (PNTR_STRCMP(layer->type.ptr, "group") == 0) {
        cute_tiled_layer_t* childLayers = layer->layers;
        while (childLayers) {
            _pntr_unload_tiled_layer_images(childLayers);
            childLayers = childLayers->next;
        }
    }
}

PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map) {
    if (map == NULL) {
        return;
    }

    // Unload the internal tiles.
    pntr_unload_memory((void*)map->tiledversion.ptr);

    // Unload all images.
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        pntr_unload_image((pntr_image*)tileset->image.ptr);
        tileset->image.ptr = NULL;
        tileset = tileset->next;
    }

    // Unload all the internal raylib data.
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        _pntr_unload_tiled_layer_images(layer);
        layer = layer->next;
    }

    cute_tiled_free_map(map);
}

PNTR_TILED_API void pntr_draw_tiled_tile(pntr_image* dst, cute_tiled_map_t* map, int gid, int posX, int posY, pntr_color tint) {
    // Get the clean Tile ID
    int tileID = cute_tiled_unset_flags(gid);

    // Get the tile image.
    pntr_image* tile = pntr_tiled_tile_image(map, tileID);
    if (tile == NULL) {
        return;
    }

    // See if we are to flip the tile in anyway.
    if (tileID != gid) {
        // TODO: pntr_draw_tiled_tile: Add flipped tile with tint with pntr_draw_image_flipped_tint
        pntr_draw_image_flipped(dst, tile,
            posX, posY,
            !!(gid & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG),
            !!(gid & CUTE_TILED_FLIPPED_VERTICALLY_FLAG),
            !!(gid & CUTE_TILED_FLIPPED_DIAGONALLY_FLAG)
        );
    }
    else {
        pntr_draw_image_tint(dst, tile, posX, posY, tint);
    }
}

/**
 * Draw a single chunk of an infinite map's tile layer.
 *
 * @internal
 * @private
 */
static void _pntr_draw_tiled_chunk(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_chunk_t* chunk, int posX, int posY, pntr_color tint) {
    if (chunk->data == NULL) {
        return;
    }

    // A chunk's x/y is its position in tiles.
    posX += chunk->x * map->tilewidth;
    posY += chunk->y * map->tileheight;

    int left, top;
    for (int y = 0; y < chunk->height; y++) {
        // Only act on tiles within y bounds.
        top = posY + y * map->tileheight;
        if (top > dst->height) {
            break;
        }
        if (top + map->tileheight < 0) {
            continue;
        }

        for (int x = 0; x < chunk->width; x++) {
            // Only act on tiles within x bounds.
            left = posX + x * map->tilewidth;
            if (left > dst->width) {
                break;
            }
            if (left + map->tilewidth < 0) {
                continue;
            }

            int index = y * chunk->width + x;
            if (index >= chunk->data_count) {
                return;
            }

            // Draw the tile from the gid.
            pntr_draw_tiled_tile(dst, map, chunk->data[index], left, top, tint);
        }
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer_tilelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    // An infinite map has no single tile array. Its tiles live in a list of chunks instead.
    if (layer->chunks != NULL) {
        cute_tiled_chunk_t* chunk = layer->chunks;
        while (chunk) {
            _pntr_draw_tiled_chunk(dst, map, chunk, posX, posY, tint);
            chunk = chunk->next;
        }
        return;
    }

    if (layer->data == NULL) {
        return;
    }

    int left, top;
    for (int y = 0; y < layer->height; y++) {
        // Only act on tiles within y bounds.
        top = posY + y * map->tileheight;
        if (top > dst->height) {
            break;
        }
        if (top + map->tileheight < 0) {
            continue;
        }

        for (int x = 0; x < layer->width; x++) {
            // Only act on tiles within x bounds.
            left = posX + x * map->tilewidth;
            if (left > dst->width) {
                break;
            }
            if (left + map->tilewidth < 0) {
                continue;
            }

            int index = y * layer->width + x;
            if (index >= layer->data_count) {
                return;
            }

            // Draw the tile from the gid.
            pntr_draw_tiled_tile(dst, map,
                layer->data[index],
                left, top,
                tint
            );
        }
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer_imagelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    PNTR_UNUSED(map);
    pntr_image* image = (pntr_image*)layer->image.ptr;

    // Tiled's tintcolor multiplies the layer rather than replacing its tint. The layer's
    // opacity has already been folded into the tint by pntr_draw_tiled_layer().
    if (layer->tintcolor != 0) {
        tint = pntr_color_tint(tint, pntr_tiled_color(layer->tintcolor));
    }

    // TODO: Image layer: Support repeatx and repeaty

    pntr_draw_image_tint(dst, image, posX, posY, tint);
}

#ifndef PNTR_TILED_CEILF
#define PNTR_TILED_CEILF(x) ((int)((x) + 0.999999f))
#endif

// Helper: get bounding box for polygon
static void get_polygon_bounds(const float* vertices, int vert_count, float* out_min_x, float* out_min_y, float* out_max_x, float* out_max_y) {
    float min_x = vertices[0], max_x = vertices[0];
    float min_y = vertices[1], max_y = vertices[1];
    for (int i = 1; i < vert_count; ++i) {
        float x = vertices[i * 2];
        float y = vertices[i * 2 + 1];
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }
    *out_min_x = min_x;
    *out_min_y = min_y;
    *out_max_x = max_x;
    *out_max_y = max_y;
}

/**
 * Draw a single object from an object layer.
 *
 * @param tint The tint to draw a tile object with.
 * @param shapeColor The color to fill a rectangle, ellipse or polygon object with.
 *
 * @internal
 * @private
 */
static void pntr_tiled_draw_object(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_object_t* obj, int posX, int posY, pntr_color tint, pntr_color shapeColor) {
    if (obj->gid != 0) {
        pntr_draw_tiled_tile(dst, map, obj->gid, (int)(obj->x + posX), (int)(obj->y + posY - map->tileheight), tint);
        return;
    }

    // Rectangle or Ellipse
    if (obj->vert_count == 0) {
        int shape_w = (int)PNTR_TILED_CEILF(obj->width);
        int shape_h = (int)PNTR_TILED_CEILF(obj->height);

        // Create temp image. This has to start blank, since an ellipse only covers part of it
        // and the shape color can be partly transparent.
        pntr_image* temp = pntr_gen_image_color(shape_w, shape_h, PNTR_BLANK);
        if (temp == NULL) {
            return;
        }

        if (obj->ellipse) {
            // Draw ellipse centered in temp image
            pntr_draw_ellipse_fill(temp, shape_w/2, shape_h/2, shape_w/2, shape_h/2, shapeColor);
        } else {
            // Draw rectangle at (0,0)
            pntr_draw_rectangle_fill(temp, 0, 0, shape_w, shape_h, shapeColor);
        }

        // Tiled turns an object clockwise about its own x/y, which for a rectangle or an
        // ellipse is the top left corner of its bounding box, and so the pixel at (0, 0) of
        // the temporary image. That is exactly the pivot pntr_draw_image_rotated() takes:
        // the source pixel at the offset lands on the position at every angle, so there is
        // no grown bounding box left to compensate for. An unrotated draw falls through to a
        // plain blit, so the same call covers every object. Tiled's rotation is clockwise and
        // pntr's turns the other way, hence the negated angle.
        pntr_draw_image_rotated(dst, temp,
            (int)(obj->x + posX), (int)(obj->y + posY),
            -obj->rotation,
            0.0f, 0.0f,
            PNTR_FILTER_BILINEAR);

        pntr_unload_image(temp);
        return;
    }

    // Polygon
    if (obj->vert_type == 1 && obj->vert_count > 0) {
        // Compute bounding box of polygon
        float min_x, min_y, max_x, max_y;
        get_polygon_bounds(obj->vertices, obj->vert_count, &min_x, &min_y, &max_x, &max_y);
        int shape_w = (int)PNTR_TILED_CEILF(max_x - min_x);
        int shape_h = (int)PNTR_TILED_CEILF(max_y - min_y);

        // Allocate and shift points to local (0,0) space
        pntr_vector* points = (pntr_vector*)pntr_load_memory(sizeof(pntr_vector) * (size_t)obj->vert_count);
        if (points == NULL) {
            return;
        }

        for (int i = 0; i < obj->vert_count; ++i) {
            points[i].x = obj->vertices[i * 2] - min_x;
            points[i].y = obj->vertices[i * 2 + 1] - min_y;
        }

        // Create temp image. This has to start blank, since the polygon only covers part of it.
        pntr_image* temp = pntr_gen_image_color(shape_w, shape_h, PNTR_BLANK);
        if (temp == NULL) {
            pntr_unload_memory(points);
            return;
        }

        pntr_draw_polygon_fill(temp, points, obj->vert_count, shapeColor);
        pntr_unload_memory(points);

        // Tiled turns an object clockwise about its own x/y, which for a polygon is the point
        // its vertices are listed relative to, and not the top left of its bounding box. The
        // temporary image holds the polygon shifted by its own bounds, so that origin sits at
        // (-min_x, -min_y) within it, which is the pivot handed to pntr_draw_image_rotated().
        // See the rectangle above for why this needs no bounding box compensation.
        pntr_draw_image_rotated(dst, temp,
            (int)(obj->x + posX), (int)(obj->y + posY),
            -obj->rotation,
            -min_x, -min_y,
            PNTR_FILTER_BILINEAR);

        pntr_unload_image(temp);
        return;
    }
}

// Compare by Y (topdown)
/**
 * An object paired with its position in the file, used to order a layer's objects.
 *
 * @internal
 */
typedef struct pntr_tiled_object_order {
    cute_tiled_object_t* object;
    int index;
} pntr_tiled_object_order;

/**
 * Order two objects top to bottom, breaking a tie by file order.
 *
 * The tie break matters: PNTR_TILED_QSORT() is not required to be stable, so without it
 * two objects sharing a y would be drawn in an unspecified order and the same map could
 * render differently from one run to the next.
 *
 * @internal
 */
static int pntr_compare_y(const void* a, const void* b) {
    const pntr_tiled_object_order* oa = (const pntr_tiled_object_order*)a;
    const pntr_tiled_object_order* ob = (const pntr_tiled_object_order*)b;

    if (oa->object->y != ob->object->y) {
        return (oa->object->y > ob->object->y) - (oa->object->y < ob->object->y);
    }

    return (oa->index > ob->index) - (oa->index < ob->index);
}

PNTR_TILED_API void pntr_draw_tiled_layer_objectlayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    // Tiled's draworder decides this: "topdown" sorts the objects by their y position, and
    // "index" keeps the order they appear in the file. Tiled writes "topdown" by default and
    // omits the attribute in that case, so an absent draworder means topdown, not index.
    //
    // A layer class of "ysort" still forces the top down sort. That was this library's own
    // convention before draworder was read at all, so a map relying on it keeps working even
    // if its layer also says "index".
    int use_ysort = 1;
    if (layer->draworder.ptr != NULL && PNTR_STRCMP(layer->draworder.ptr, "index") == 0) {
        use_ysort = 0;
    }
    if (layer->class_.ptr != NULL && PNTR_STRCMP(layer->class_.ptr, "ysort") == 0) {
        use_ysort = 1;
    }

    // Count visible objects
    int count = 0;
    for (cute_tiled_object_t* o = layer->objects; o; o = o->next){
        if (o->visible) {
            count++;
        }
    }
    if (count == 0) {
        return;
    }

    // An object layer's "color" is only applied to the shapes. See the notes on
    // pntr_draw_tiled_layer_objectlayer() for why tile objects keep the unmodified tint.
    pntr_color shapeColor = tint;
    if (layer->tintcolor != 0) {
        shapeColor = pntr_color_tint(tint, pntr_tiled_color(layer->tintcolor));
    }

    pntr_tiled_object_order* order = (pntr_tiled_object_order*)pntr_load_memory((size_t)count * sizeof(*order));
    if (order == NULL) {
        return;
    }

    // cute_tiled hands back a layer's objects in reverse file order, so the array is filled
    // from the back to put them in the order the file declares them. That order is what
    // "index" means, and it is also the tie break for the top down sort.
    int i = count;
    for (cute_tiled_object_t* o = layer->objects; o; o = o->next) {
        if (o->visible) {
            i--;
            order[i].object = o;
            order[i].index = i;
        }
    }

    if (use_ysort) {
        PNTR_TILED_QSORT(order, (size_t)count, sizeof(*order), pntr_compare_y);
    }

    for (i = 0; i < count; i++) {
        pntr_tiled_draw_object(dst, map, order[i].object, posX, posY, tint, shapeColor);
    }

    pntr_unload_memory(order);
}

PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    if (dst == NULL || map == NULL || layer == NULL || tint.rgba.a == 0) {
        return;
    }

    while (layer) {
        if (layer->type.ptr != NULL && layer->opacity > 0 && layer->visible) {
            // Apply opacity to the layer
            pntr_color tintWithOpacity = tint;
            if (layer->opacity != 1) {
                pntr_color_set_a(&tintWithOpacity, (unsigned char)((float)pntr_color_a(tintWithOpacity) * layer->opacity));
            }

            // Draw the layer
            switch (layer->type.ptr[0]) {
                case 't': // "tilelayer"
                    pntr_draw_tiled_layer_tilelayer(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
                break;
                case 'g': // "group"
                    pntr_draw_tiled_layer(dst, map, layer->layers, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
                break;
                case 'o': // "objectgroup"
                    pntr_draw_tiled_layer_objectlayer(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
                break;
                case 'i': // "imagelayer"
                    pntr_draw_tiled_layer_imagelayer(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
                break;
            }
        }

        layer = layer->next;
    }
}

/**
 * Collect the bounding box, in tiles, of every chunk of the given layers and their children.
 *
 * @internal
 * @private
 */
static void _pntr_tiled_chunk_bounds(cute_tiled_layer_t* layer, int* minX, int* minY, int* maxX, int* maxY, int* found) {
    while (layer) {
        if (layer->layers != NULL) {
            _pntr_tiled_chunk_bounds(layer->layers, minX, minY, maxX, maxY, found);
        }

        cute_tiled_chunk_t* chunk = layer->chunks;
        while (chunk) {
            if (chunk->width > 0 && chunk->height > 0) {
                if (*found == 0) {
                    *minX = chunk->x;
                    *minY = chunk->y;
                    *maxX = chunk->x + chunk->width;
                    *maxY = chunk->y + chunk->height;
                    *found = 1;
                }
                else {
                    if (chunk->x < *minX) *minX = chunk->x;
                    if (chunk->y < *minY) *minY = chunk->y;
                    if (chunk->x + chunk->width > *maxX) *maxX = chunk->x + chunk->width;
                    if (chunk->y + chunk->height > *maxY) *maxY = chunk->y + chunk->height;
                }
            }

            chunk = chunk->next;
        }

        layer = layer->next;
    }
}

/**
 * Work out how large an image of the given map has to be, and where the map's origin sits
 * within it.
 *
 * @details An infinite map is sized and positioned from the bounding box of all of its chunks,
 * since its chunks can sit at negative tile coordinates. Every other map is the size of its own
 * tile grid, with its origin at the top left.
 *
 * @internal
 * @private
 */
static void _pntr_tiled_image_bounds(cute_tiled_map_t* map, int* width, int* height, int* posX, int* posY) {
    *width = map->tilewidth * map->width;
    *height = map->tileheight * map->height;
    *posX = 0;
    *posY = 0;

    if (map->infinite == 0) {
        return;
    }

    int minX = 0, minY = 0, maxX = 0, maxY = 0, found = 0;
    _pntr_tiled_chunk_bounds(map->layers, &minX, &minY, &maxX, &maxY, &found);
    if (found == 0) {
        return;
    }

    *width = (maxX - minX) * map->tilewidth;
    *height = (maxY - minY) * map->tileheight;
    *posX = -minX * map->tilewidth;
    *posY = -minY * map->tileheight;
}

PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint) {
    if (map == NULL) {
        return pntr_set_error(PNTR_ERROR_INVALID_ARGS);
    }

    int width, height, posX, posY;
    _pntr_tiled_image_bounds(map, &width, &height, &posX, &posY);

    // cute_tiled keeps the background color as the raw hex from the file (#RRGGBB or
    // #AARRGGBB), which pntr_tiled_color() decodes. A map with no background color is left
    // transparent.
    pntr_color background = map->backgroundcolor != 0 ? pntr_tiled_color(map->backgroundcolor) : PNTR_BLANK;
    pntr_image* output = pntr_gen_image_color(width, height, background);
    if (output == NULL) {
        return NULL;
    }

    pntr_draw_tiled(output, map, posX, posY, tint);
    return output;
}

PNTR_TILED_API pntr_image* pntr_gen_image_tiled_layer(cute_tiled_map_t* map, cute_tiled_layer_t* layer, pntr_color tint) {
    if (map == NULL) {
        return pntr_set_error(PNTR_ERROR_INVALID_ARGS);
    }

    // The whole map's bounds are used, rather than just this layer's, so that the images of
    // each layer of a map line up with each other.
    int width, height, posX, posY;
    _pntr_tiled_image_bounds(map, &width, &height, &posX, &posY);

    pntr_image* output = pntr_gen_image_color(width, height, PNTR_BLANK);
    if (output == NULL) {
        return NULL;
    }

    pntr_draw_tiled_layer(output, map, layer, posX, posY, tint);
    return output;
}

PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint) {
    if (map == NULL) {
        return;
    }

    pntr_draw_tiled_layer(dst, map, map->layers, posX, posY, tint);
}

PNTR_TILED_API void pntr_update_tiled(cute_tiled_map_t* map, float deltaTime) {
    // Update the animation counter
    map->nextlayerid += (int)(deltaTime * 1000);

    // Keep the counter from getting too big. 30 second animations seems long enough.
    if (map->nextlayerid > 30000) {
        map->nextlayerid -= 30000;
    }
}

PNTR_TILED_API cute_tiled_layer_t* pntr_tiled_layer(cute_tiled_map_t* map, const char* name) {
    if (map == NULL || name == NULL) {
        return NULL;
    }

    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        if (PNTR_STRCMP(layer->name.ptr, name) == 0) {
            return layer;
        }
        layer = layer->next;
    }

    return NULL;
}

PNTR_TILED_API cute_tiled_layer_t* pntr_tiled_layer_from_index(cute_tiled_map_t* map, int i) {
    if (map == NULL || i < 0) {
        return NULL;
    }

    cute_tiled_layer_t* layer = map->layers;
    int index = 0;
    while (layer) {
        if (index == i) {
            return layer;
        }
        index++;
        layer = layer->next;
    }

    return NULL;
}

PNTR_TILED_API int pntr_tiled_layer_count(cute_tiled_map_t* map) {
    if (map == NULL) {
        return 0;
    }

    cute_tiled_layer_t* layer = map->layers;
    int count = 0;
    while (layer) {
        count++;
        layer = layer->next;
    }
    return count;
}

/**
 * Find the position of the given tile within an infinite map layer's chunks.
 *
 * @param column The x coordinate of the tile, in the map's own tile coordinates, which can be
 * negative for an infinite map.
 * @param row The y coordinate of the tile.
 * @param index Where to put the index of the tile within the chunk that holds it.
 *
 * @return The chunk that holds the tile, or NULL when no chunk covers it.
 *
 * @internal
 * @private
 */
static cute_tiled_chunk_t* _pntr_tiled_find_chunk(cute_tiled_layer_t* layer, int column, int row, int* index) {
    cute_tiled_chunk_t* chunk = layer->chunks;
    while (chunk) {
        if (chunk->data != NULL &&
                column >= chunk->x && column < chunk->x + chunk->width &&
                row >= chunk->y && row < chunk->y + chunk->height) {
            int chunkIndex = (row - chunk->y) * chunk->width + (column - chunk->x);
            if (chunkIndex >= 0 && chunkIndex < chunk->data_count) {
                *index = chunkIndex;
                return chunk;
            }

            return NULL;
        }

        chunk = chunk->next;
    }

    return NULL;
}

PNTR_TILED_API int pntr_layer_tile(cute_tiled_layer_t* layer, int column, int row) {
    if (layer == NULL) {
        return 0;
    }

    // An infinite map keeps its tiles in chunks, which can sit at negative coordinates.
    if (layer->chunks != NULL) {
        int index;
        cute_tiled_chunk_t* chunk = _pntr_tiled_find_chunk(layer, column, row, &index);
        if (chunk == NULL) {
            return 0;
        }

        return cute_tiled_unset_flags(chunk->data[index]);
    }

    if (layer->data == NULL) {
        return 0;
    }

    int index = row * layer->width + column;
    if (index < 0 || index >= layer->data_count) {
        return 0;
    }

    // TODO: Allow getting flip status?
    return cute_tiled_unset_flags(layer->data[index]);
}

PNTR_TILED_API void pntr_set_layer_tile(cute_tiled_layer_t* layer, int column, int row, int gid) {
    if (layer == NULL || gid < 0) {
        return;
    }

    // An infinite map keeps its tiles in chunks, which can sit at negative coordinates. A tile
    // that no chunk covers cannot be set, since that would mean growing the map.
    if (layer->chunks != NULL) {
        int index;
        cute_tiled_chunk_t* chunk = _pntr_tiled_find_chunk(layer, column, row, &index);
        if (chunk != NULL) {
            chunk->data[index] = gid;
        }

        return;
    }

    if (layer->data == NULL) {
        return;
    }

    int index = row * layer->width + column;
    if (index >= layer->data_count || index < 0) {
        return;
    }

    // TODO: Add flip status to set_tiled_tile_at()
    layer->data[index] = gid;
}

PNTR_TILED_API pntr_vector pntr_layer_tile_from_position(cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY) {
    if (map == NULL || layer == NULL || map->tilewidth <= 0 || map->tileheight <= 0) {
        return (pntr_vector) {
            .x = -1,
            .y = -1
        };
    }

    // TODO: Is the layer offset correct?
    return (pntr_vector) {
        .x = (posX - layer->offsetx) / map->tilewidth,
        .y = (posY - layer->offsety) / map->tileheight
    };
}


// General helper to get a pntr_color from a tiled color
PNTR_TILED_API pntr_color pntr_tiled_color(uint32_t color) {
    if (color > 0xFFFFFF) {
        // Has alpha channel
        return pntr_new_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, (color >> 24) & 0xFF);
    } else {
        // No alpha, default to fully opaque
        return pntr_new_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 0xFF);
    }
}

// Find an object by name on an object-layer
PNTR_TILED_API cute_tiled_object_t* pntr_tiled_get_object(cute_tiled_layer_t* objects_layer, const char* name) {
  if (objects_layer == NULL || objects_layer->objects == NULL) {
    return NULL;
  }
  cute_tiled_object_t* current = objects_layer->objects;
  while (current != NULL) {
    if (current->name.ptr != NULL && PNTR_STRCMP(current->name.ptr, name) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

/**
 * Integrations
 */
#ifdef PNTR_ASSETSYS_API
    #include "pntr_tiled_assetsys.h"
#endif

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_IMPLEMENTATION_ONCE
#endif  // PNTR_TILED_IMPLEMENTATION
