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
PNTR_TILED_API void pntr_draw_tiled_layer_imagelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer_tilelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer_objectlayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);

/**
 * Retrieves an image representing the desired tile from the given global tile ID.
 *
 * @param map The map to get the tile from.
 * @param gid The global tile ID for the tile. This cannot exceed the number of tiles in the map.
 *
 * @return A subimage from the tileset for the given tile.
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

// TODO: Remove stdio.h
#include <stdio.h> // printf

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
#include PNTR_TILED_CUTE_TILED_H

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

static pntr_color _pntr_get_tiled_color(uint32_t hexValue) {
    return pntr_new_color(
        hexValue & 0xff,
        (hexValue >> 8) & 0xff,
        (hexValue >> 16) & 0xff,
        (hexValue >> 24) & 0xff);
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
        tileCount += tileset->tilecount;
        tileset = tileset->next;
    }

    // Prepare the entire tiles set
    pntr_tiled_tile* tiles = pntr_load_memory(sizeof(pntr_tiled_tile) * (size_t)tileCount);

    // Build all the tiles from each tileset.
    tileset = map->tilesets;
    while (tileset) {
        for (int i = 0; i < tileset->tilecount; i++) {
            // Calculate the gid.
            int gid = tileset->firstgid + i - 1;
            pntr_tiled_tile* tile = tiles + gid;
            tile->tileset = tileset;
            tile->descriptor = NULL;
            tile->animationDuration = 0;

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

            // Build the subimage as part of the tiles array.
            pntr_image* temporaryTile = pntr_image_subimage((pntr_image*)tileset->image.ptr, srcRect.x, srcRect.y, srcRect.width, srcRect.height);
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
}

PNTR_TILED_API pntr_image* pntr_tiled_tile_image(cute_tiled_map_t* map, int gid) {
    if (gid <= 0) {
        return NULL;
    }

    pntr_tiled_tile* tile = (pntr_tiled_tile*)map->tiledversion.ptr;
    tile = tile + gid - 1;

    // Process any descriptive tile properties.
    if (tile->descriptor != NULL) {
        // Animation
        if (tile->descriptor->frame_count > 0) {
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
            tile = (pntr_tiled_tile*)map->tiledversion.ptr;
            tile = tile + gid - 1;
        }
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
        printf("pntr_tiled: Failed to load image: %s", fullPath);
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
            pntr_image_color_replace((pntr_image*)tileset->image.ptr, _pntr_get_tiled_color(tileset->transparentcolor), PNTR_BLANK);
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

PNTR_TILED_API void pntr_draw_tiled_layer_tilelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
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

            // Draw the tile from the gid.
            pntr_draw_tiled_tile(dst, map,
                layer->data[y * layer->width + x],
                left, top,
                tint
            );
        }
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer_imagelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    PNTR_UNUSED(map);
    pntr_image* image = (pntr_image*)layer->image.ptr;

    if (layer->tintcolor != 0) {
        tint = pntr_color_alpha_blend(tint,
            _pntr_get_tiled_color(layer->tintcolor)
        );
        tint.rgba.a *= layer->opacity;
    }

    // TODO: Image layer: Support repeatx and repeaty

    pntr_draw_image_tint(dst, image, posX, posY, tint);
}

PNTR_TILED_API void pntr_draw_tiled_layer_objectlayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint){
    cute_tiled_object_t* object = layer->objects;
    while (object != NULL) {
        pntr_draw_tiled_tile(dst, map, object->gid, object->x + posX, object->y + posY -  map->tileheight, tint);
        object = object->next;
    }
}


PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    if (dst == NULL || map == NULL || layer == NULL || tint.rgba.a == 0) {
        return;
    }

    while (layer) {
        if (layer->type.ptr != NULL && layer->opacity > 0) {
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

PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint) {
    if (map == NULL) {
        return pntr_set_error(PNTR_ERROR_INVALID_ARGS);
    }

    pntr_color background = pntr_get_color(map->backgroundcolor);
    pntr_image* output = pntr_gen_image_color(map->tilewidth * map->width, map->tileheight * map->height, background);
    if (output == NULL) {
        return NULL;
    }

    pntr_draw_tiled(output, map, 0, 0, tint);
    return output;
}

PNTR_TILED_API pntr_image* pntr_gen_image_tiled_layer(cute_tiled_map_t* map, cute_tiled_layer_t* layer, pntr_color tint) {
    if (map == NULL) {
        return pntr_set_error(PNTR_ERROR_INVALID_ARGS);
    }

    pntr_image* output = pntr_gen_image_color(map->tilewidth * map->width, map->tileheight * map->height, PNTR_BLANK);
    if (output == NULL) {
        return NULL;
    }

    pntr_draw_tiled_layer(output, map, layer, 0, 0, tint);
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

PNTR_TILED_API int pntr_layer_tile(cute_tiled_layer_t* layer, int column, int row) {
    if (layer == NULL || layer->data == NULL) {
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
    if (layer == NULL || layer->data == NULL || gid < 0) {
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
    if (current->name.ptr != NULL && strcmp(current->name.ptr, name) == 0) {
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
