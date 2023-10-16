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

/**
 * Retrieves an image representing the desired tile from the given global tile ID.
 *
 * @param map The map to get the tile from.
 * @param gid The global tile ID for the tile. This cannot exceed the number of tiles in the map.
 *
 * @return A subimage from the tileset for the given tile.
 */
PNTR_TILED_API pntr_image* pntr_get_tiled_tile(cute_tiled_map_t* map, int gid);

/**
 * Generate an image of the given Tiled map.
 *
 * @param map The map to build an image of.
 * @param tint What color to tint the map.
 * @return An image representing the rendered map, or NULL on failure.
 */
PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint);

/**
 * Update the internal animation frame time counter for the map.
 *
 * @param map The map to update.
 * @param deltaTime The amount of time that changed from the last update, in seconds.
 */
PNTR_TILED_API void pntr_update_tiled(cute_tiled_map_t* map, float deltaTime);

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

#ifdef __cplusplus
extern "C" {
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

/**
 * Finds the last slash in the given path.
 *
 * @internal
 * @private
 */
char* _pntr_tiled_find_last_slash(const char* str) {
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
void _pntr_tiled_path_basedir(char* path) {
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
void _pntr_load_map_data(cute_tiled_map_t* map) {
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

PNTR_TILED_API pntr_image* pntr_get_tiled_tile(cute_tiled_map_t* map, int gid) {
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
void _pntr_load_tiled_string_texture(cute_tiled_string_t* image, const char* baseDir) {
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

void _pntr_load_tiled_layer_images(cute_tiled_layer_t* layer, const char* baseDir) {
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

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(fileData, (int)dataSize, 0);
    if (map == NULL) {
        return NULL;
    }

    // Load all the tileset images.
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        _pntr_load_tiled_string_texture(&tileset->image, baseDir);
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

void _pntr_unload_tiled_layer_images(cute_tiled_layer_t* layer) {
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
    pntr_image* tile = pntr_get_tiled_tile(map, tileID);
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

    // TODO: Image layer: Support tint color from image layer... JSON output is either RGBA or RGB, but cute_tiled doesn't distinguish.
    //pntr_color tintcolor = pntr_get_color(layer->tintcolor);
    //printf("Color: %dx%dx%dx%d", tintcolor.r, tintcolor.g, tintcolor.b, tintcolor.a);

    // TODO: Image layer: Support repeatx and repeaty

    pntr_draw_image_tint(dst, image, posX, posY, tint);
}

PNTR_TILED_API void pntr_draw_tiled_object(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_object_t* object, int posX, int posY, pntr_color tint) {
    if (object == NULL || dst == NULL || map == NULL) {
        return;
    }

    if (object->gid != 0) {
        pntr_draw_tiled_tile(dst, map, object->gid, posX, posY, tint);
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer_objectgroup(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    cute_tiled_object_t* object = layer->objects;
    while (object) {
        #ifndef PNTR_DRAW_TILED_OBJECT
        #define PNTR_DRAW_TILED_OBJECT pntr_draw_tiled_object
        #endif
        PNTR_DRAW_TILED_OBJECT(dst, map, object, posX + object->x, posY + object->y - object->height, tint);

        object = object->next;
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    if (dst == NULL || map == NULL || layer == NULL || tint.a == 0) {
        return;
    }

	while (layer) {
		if (layer->type.ptr != NULL && layer->opacity > 0) {
            // Apply opacity to the layer
            pntr_color tintWithOpacity = tint;
            if (layer->opacity != 1) {
                tintWithOpacity.a = (float)tintWithOpacity.a * layer->opacity;
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
                    pntr_draw_tiled_layer_objectgroup(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
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

PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint) {
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

// pntr_assetsys integration
#ifdef PNTR_ASSETSYS_API
/**
 * Load the given cute_tiled_string_t image as a pntr_image.
 *
 * @internal
 * @private
 */
void _pntr_load_tiled_assetsys_string_texture(assetsys_t* sys, cute_tiled_string_t* image, const char* baseDir) {
    if (sys == NULL || image == NULL || image->ptr == NULL) {
        return;
    }

    char fullPath[PNTR_PATH_MAX];
    fullPath[0] = '\0';
    PNTR_STRCAT(fullPath, baseDir);
    PNTR_STRCAT(fullPath, image->ptr);

    image->ptr = (const char*)pntr_load_image_from_assetsys(sys, fullPath);
    if (image->ptr == NULL) {
        printf("pntr_tiled: Failed to load image from assetsys: %s", fullPath);
    }
}

void _pntr_load_tiled_layer_images_from_assetsys(assetsys_t* sys, cute_tiled_layer_t* layer, const char* baseDir) {
    if (layer == NULL || sys == NULL) {
        return;
    }

    if (PNTR_STRCMP(layer->type.ptr, "imagelayer") == 0) {
        _pntr_load_tiled_assetsys_string_texture(sys, &layer->image, baseDir);
    }
    else if (PNTR_STRCMP(layer->type.ptr, "group") == 0) {
        cute_tiled_layer_t* childLayers = layer->layers;
        while (childLayers) {
            _pntr_load_tiled_layer_images_from_assetsys(sys, childLayers, baseDir);
            childLayers = childLayers->next;
        }
    }
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_assetsys(assetsys_t* sys, const char* fileName) {
    unsigned int size;
    unsigned char* data = pntr_load_file_from_assetsys(sys, fileName, &size);
    if (data == NULL) {
        return NULL;
    }

    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, (int)size, 0);
    pntr_unload_memory(data);
    if (map == NULL) {
        return NULL;
    }

    // Copy the fileName, along with its null terminator to find the basePath.
    char baseDir[PNTR_PATH_MAX];
    size_t fileNameLength = PNTR_STRLEN(fileName);
    pntr_memory_copy((void*)baseDir, (void*)fileName, fileNameLength);
    baseDir[fileNameLength] = '\0';
    _pntr_tiled_path_basedir(baseDir);

    // Tilesets
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        _pntr_load_tiled_assetsys_string_texture(sys, &tileset->image, baseDir);
        tileset = tileset->next;
    }

    // Load all image layers.
    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        _pntr_load_tiled_layer_images_from_assetsys(sys, layer, baseDir);
        layer = layer->next;
    }

    // Build the tile global IDs as subimages.
    _pntr_load_map_data(map);

    return map;
}
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_IMPLEMENTATION_ONCE
#endif  // PNTR_TILED_IMPLEMENTATION
