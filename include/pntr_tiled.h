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

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled(const char* fileName);
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir);
PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map);
PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_tile(pntr_image* dst, cute_tiled_map_t* map, int gid, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer_imagelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer_tilelayer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API pntr_image* pntr_get_tiled_tile(cute_tiled_map_t* map, int gid);
PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint);

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
 * Prepare the internal tiles.
 *
 * @internal
 * @private
 */
void _pntr_load_tiled_tiles(cute_tiled_map_t* map) {
    if (map == NULL) {
        return;
    }

    // Count how many tiles there are
    int tileCount = 0;
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        tileCount += tileset->tilecount;
        tileset = tileset->next;
    }

    // Prepare the entire tiles set
    pntr_image* tiles = pntr_load_memory(sizeof(pntr_image) * (size_t)tileCount);

    // Build all the tiles from each tileset.
    tileset = map->tilesets;
    while (tileset) {
        for (int i = 0; i < tileset->tilecount; i++) {
            // Calculate the gid.
            int gid = tileset->firstgid + i - 1;

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
            pntr_memory_copy((void*)(tiles + gid), (void*)temporaryTile, sizeof(pntr_image));

            // Don't need the temporary tile anymore.
            pntr_unload_image(temporaryTile);
        }
        tileset = tileset->next;
    }

    map->tiledversion.ptr = (const char*)tiles;
}

PNTR_TILED_API pntr_image* pntr_get_tiled_tile(cute_tiled_map_t* map, int gid) {
    if (gid <= 0) {
        return NULL;
    }

    // Reference the internal gid'ed tiles.
    pntr_image* tiles = (pntr_image*)map->tiledversion.ptr;
    return tiles + gid - 1;
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
    _pntr_load_tiled_tiles(map);

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
                    // TODO: Draw the objects?
                    //DrawMapLayerObjects(layer->objects, layer->offsetx + posX, layer->offsety + posY, tintWithOpacity);
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
    _pntr_load_tiled_tiles(map);

    return map;
}
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_IMPLEMENTATION_ONCE
#endif  // PNTR_TILED_IMPLEMENTATION
