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
PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
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

// cute_tiled
#define CUTE_TILED_IMPLEMENTATION
#define CUTE_TILED_ALLOC(size, ctx) pntr_load_memory(size)
#define CUTE_TILED_FREE(mem, ctx) pntr_unload_memory(mem)
#define CUTE_TILED_MEMCPY PNTR_MEMCPY
#define CUTE_TILED_MEMSET PNTR_MEMSET
#define CUTE_TILED_SEEK_SET 0
#define CUTE_TILED_SEEK_END 0
#define CUTE_TILED_FILE void
#define CUTE_TILED_FOPEN(path, state) NULL
#define CUTE_TILED_FSEEK(fp, num, seekpart)
#define CUTE_TILED_FREAD(data, sz, num, fp)
#define CUTE_TILED_FTELL(fp) 0
#define CUTE_TILED_FCLOSE(fp) (void)fp
#include PNTR_TILED_CUTE_TILED_H

#ifdef __cplusplus
extern "C" {
#endif

char *_pntr_tiled_find_last_slash(const char *str) {
   const char *slash     = PNTR_STRRCHR(str, '/');
   const char *backslash = PNTR_STRRCHR(str, '\\');

   if (!slash || (backslash > slash))
      return (char*)backslash;
   else
      return (char*)slash;
}

void _pntr_tiled_path_basedir(char *path) {
   char *last = NULL;
   if (!path || path[0] == '\0' || path[1] == '\0')
      return;

   if ((last = _pntr_tiled_find_last_slash(path)))
      last[1] = '\0';
   else
   {
      path[0] = '\0';
   }
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* data = pntr_load_file(fileName, &bytesRead);
    if (data == NULL) {
        return NULL;
    }

    // Copy the fileName, along with its null terminator to find the basePath.
    size_t length = 0;
    while (fileName[length++] != '\0');
    char* baseDir = pntr_load_memory(length + 1);
    pntr_memory_copy((void*)baseDir, (void*)fileName, length);
    _pntr_tiled_path_basedir(baseDir);

    // Load the tiled map.
    cute_tiled_map_t* output = pntr_load_tiled_from_memory(data, bytesRead, baseDir);
    pntr_unload_memory(data);
    pntr_unload_memory(baseDir);
    return output;
}

void _pntr_load_tiled_string_texture(cute_tiled_string_t* image, const char* baseDir) {
    // TODO: Allow loading images from a base64 embedded image.
    char fullPath[255];
    fullPath[0] = '\0';
    PNTR_STRCAT(fullPath, baseDir);
    PNTR_STRCAT(fullPath, image->ptr);

    pntr_image* loadedImage = pntr_load_image(fullPath);
    if (loadedImage == NULL) {
        printf("pntr_tiled: Failed to load image: %s", fullPath);
    }
    image->ptr = (void*)loadedImage;
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(fileData, (int)dataSize, 0);
    if (map == NULL) {
        return NULL;
    }

    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        _pntr_load_tiled_string_texture(&tileset->image, baseDir);
        tileset = tileset->next;
    }

    return map;
}

PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map) {
    if (map == NULL) {
        return;
    }

    // Unload all images.
    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        pntr_unload_image((pntr_image*)tileset->image.ptr);
        tileset->image.ptr = NULL;
        tileset = tileset->next;
    }

    cute_tiled_free_map(map);
}

void pntr_draw_tiled_layer_tiles(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    int gid;

	for (int y = 0; y < layer->height; y++) {
		for (int x = 0; x < layer->width; x++) {
            gid = layer->data[(y * layer->width) + x];
            cute_tiled_tileset_t* activeTileset = NULL;
            cute_tiled_tileset_t* tileset = map->tilesets;
            int tileID;

            while (tileset) {
                if (gid >= tileset->firstgid && gid < tileset->firstgid + tileset->tilecount) {
                    tileID = gid - tileset->firstgid;
                    activeTileset = tileset;
                    break;
                }
                tileset = tileset->next;
            }

            if (activeTileset != NULL) {
                int tileX = tileID % activeTileset->columns;
                int tileY = tileID / activeTileset->columns;

                pntr_rectangle srcRect = {
                    .x = tileX * activeTileset->tilewidth + tileX * activeTileset->spacing + activeTileset->margin,
                    .y = tileY * activeTileset->tileheight  + tileY * activeTileset->spacing + activeTileset->margin,
                    .width = activeTileset->tilewidth,
                    .height = activeTileset->tileheight
                };

                pntr_draw_image_rec(dst, (pntr_image*)activeTileset->image.ptr, srcRect, posX + x * activeTileset->tilewidth, posY + y * activeTileset->tileheight);
            }
            else {
                printf("Tile not found: %d\n", gid);
            }
        }
    }
}

PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    if (dst == NULL || map == NULL || layer == NULL) {
        return;
    }

	while (layer) {
		if (layer->visible == 1) {
            // TODO: Move to a switch statement on the first character instead.
            if (PNTR_STRCMP(layer->type.ptr, "tilelayer") == 0) {
                pntr_draw_tiled_layer_tiles(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tint);
            }
            else if (PNTR_STRCMP(layer->type.ptr, "group") == 0) {
                pntr_draw_tiled_layer(dst, map, layer->layers, layer->offsetx + posX, layer->offsety + posY, tint);
            } else if (PNTR_STRCMP(layer->type.ptr, "objectgroup") == 0) {
                // TODO: Draw the objects?
                //DrawMapLayerObjects(layer->objects, layer->offsetx + posX, layer->offsety + posY, tint);
            } else if (PNTR_STRCMP(layer->type.ptr, "imagelayer") == 0) {
                // TODO: Draw the image layers
                //DrawMapLayerImage(layer->image, layer->offsetx + posX, layer->offsety + posY, tint);
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
    if (map == NULL) {
        return;
    }
    pntr_color background = pntr_get_color(map->backgroundcolor);
    pntr_draw_rectangle_fill(dst, posX, posY, map->tilewidth * map->width, map->tileheight * map->height, background);
    pntr_draw_tiled_layer(dst, map, map->layers, posX, posY, tint);
}

// pntr_assetsys integration
#ifdef PNTR_ASSETSYS_API
/**
 * Load the given cute_tiled_string_t image as a pntr_image.
 */
void _pntr_load_tiled_assetsys_string_texture(assetsys_t* sys, cute_tiled_string_t* image, const char* baseDir) {
    char fullPath[255];
    fullPath[0] = '\0';
    PNTR_STRCAT(fullPath, baseDir);
    PNTR_STRCAT(fullPath, image->ptr);

    pntr_image* loadedImage = pntr_load_image_from_assetsys(sys, fullPath);
    if (loadedImage == NULL) {
        printf("pntr_tiled: Failed to load image: %s", fullPath);
    }
    image->ptr = (void*)loadedImage;
}

PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_assetsys(assetsys_t* sys, const char* fileName) {
    unsigned int size;
    unsigned char* data = pntr_load_file_from_assetsys(sys, fileName, &size);
    if (data == NULL) {
        return NULL;
    }

    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, (int)size, 0);
    if (map == NULL) {
        pntr_unload_memory(data);
        return NULL;
    }

    // Create the baseDir
    size_t length = 0;
    while (fileName[length++] != '\0');
    char* baseDir = pntr_load_memory(length);
    pntr_memory_copy((void*)baseDir, (void*)fileName, length);
    _pntr_tiled_path_basedir(baseDir);

    cute_tiled_tileset_t* tileset = map->tilesets;
    while (tileset) {
        _pntr_load_tiled_assetsys_string_texture(sys, &tileset->image, baseDir);
        tileset = tileset->next;
    }

    pntr_unload_memory(data);
    pntr_unload_memory(baseDir);

    return map;
}
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_IMPLEMENTATION_ONCE
#endif  // PNTR_TILED_IMPLEMENTATION
