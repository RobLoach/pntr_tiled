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
PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map);

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
    pntr_memory_copy((void*)baseDir, (void*)fileName, length + 1);
    _pntr_tiled_path_basedir(baseDir);

    // Load the tiled map.
    cute_tiled_map_t* output = pntr_load_tiled_from_memory(data, bytesRead, baseDir);
    pntr_unload_memory(data);
    pntr_unload_memory(baseDir);
    return output;
}


void _pntr_load_tiled_string_texture(cute_tiled_string_t* image, const char* baseDir) {
    char fullPath[255];
    fullPath[0] = '\0';
    strcat(fullPath, baseDir);
    strcat(fullPath, image->ptr);

    pntr_image* loadedImage = pntr_load_image(fullPath);
    if (image == NULL) {
        printf("pntr_tiled: Failed to load image: %s", fullPath);
        return;
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
        tileset = tileset->next;
    }

    cute_tiled_free_map(map);
}

void pntr_draw_map_layer_tiles(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
    unsigned int gid;
            printf("Layer! \n");
    cute_tiled_tileset_t* tilesets = map->tilesets;

	for (int y = 0; y < layer->height; y++) {
		for (int x = 0; x < layer->width; x++) {
            gid = layer->data[(y * layer->width) + x];
            cute_tiled_tileset_t* activeTileset = NULL;
            cute_tiled_tile_descriptor_t* tile = NULL;
            cute_tiled_tileset_t* tileset = map->tilesets;
            int tileID = -1;


            while (tileset) {



                cute_tiled_tile_descriptor_t* tileCounter = tileset->tiles;
                while (tileCounter) {
                    //printf("Index: %d %d\n", gid, tileCounter->tile_index);
                    //return;
                    if (gid == tileCounter->tile_index + tileset->firstgid) {
                        tile = tileCounter;
                        activeTileset = tileset;
                        break;
                    }
                    tileCounter = tileCounter->next;
                }

                if (tile != NULL ) {
                    break;
                }


                
                // if (tileset->firstgid >= gid && gid < tileset->firstgid + tileset->tilecount) {

                //     tile = tileset->tiles + gid - tileset->firstgid - 1;
                //     activeTileset = tileset;
                //     break;
                // }

                tileset = tileset->next;
            }

            if (activeTileset != NULL) {
                //printf("found!\n");

                int tileX = tile->tile_index % activeTileset->columns;
                int tileY = tile->tile_index / activeTileset->columns;

                pntr_rectangle rec = {
                    .x = tileX * activeTileset->tilewidth + tileX * activeTileset->spacing + activeTileset->margin,
                    .y = tileY * activeTileset->tileheight  + tileY * activeTileset->spacing + activeTileset->margin,
                    .width = map->tilewidth,
                    .height = map->tileheight
                };

                printf("src: %d %d %dx%d %dx%d - %dx%d\n", gid, tile->tile_index, rec.x, rec.y, rec.width, rec.height, tileX, tileY);
                pntr_draw_image_rec(dst, (pntr_image*)activeTileset->image.ptr, rec, posX + x * activeTileset->tilewidth, posY + y * activeTileset->tileheight);
                pntr_draw_rectangle(dst,  posX + x * activeTileset->tilewidth,  posY + y * activeTileset->tileheight, activeTileset->tilewidth, activeTileset->tileheight, 1, PNTR_RED);
            }

            //printf("%d, ", gid);
        }
        //printf("\n");
    }
    /*
	unsigned long i, j;
	unsigned int gid, x, y, w, h, flags;
	float opacity;
	cute_tiled_tileset_t *ts;
	cute_tiled_string_t *im;
	void* image;
	opacity = layer->opacity;
	for (i = 0; i < layer->height; i++) {
		for (j = 0; j < layer->width; j++) {
			gid = layer->data[(i * layer->width) + j];

            cute_tiled_tile_descriptor_t* tile = GetTileFromGid(map, gid);

            if (tile != NULL) {
                ts = GetTilesetFromHash(map->tilesets, tile->image.hash_id);
                if (ts != NULL) {
                    x = ts->margin + (j * ts->tilewidth)  + (j * ts->spacing);
                    y = ts->margin + (i * ts->tileheight)  + (i * ts->spacing);
                    // x  = map->tiles[gid]->ul_x;
                    // y  = map->tiles[gid]->ul_y;
                    w  = ts->tilewidth;
                    h  = ts->tileheight;
                    // flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
                    //DrawMapTile((Texture*)ts->image.ptr, x, y, w, h, j*ts->tilewidth + posX, i*ts->tileheight + posY, op,  tint);
                }
            }
		}
	}

    */
}

void pntr_draw_map_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint) {
	while (layer) {
		if (layer->visible == 1) {
            if (PNTR_STRCMP(layer->type.ptr, "group") == 0) {
                //DrawMapLayer(map, layer->layers, layer->offsetx + posX, layer->offsety + posY, tint);
            } else if (PNTR_STRCMP(layer->type.ptr, "objectgroup") == 0) {
                //DrawMapLayerObjects(layer->objects, layer->offsetx + posX, layer->offsety + posY, tint);
            } else if (PNTR_STRCMP(layer->type.ptr, "imagelayer") == 0) {
                //DrawMapLayerImage(layer->image, layer->offsetx + posX, layer->offsety + posY, tint);
            } else if (PNTR_STRCMP(layer->type.ptr, "tilelayer") == 0) {
                pntr_draw_map_layer_tiles(dst, map, layer, layer->offsetx + posX, layer->offsety + posY, tint);
            }
		}
		layer = layer->next;
	}
}

PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint) {
    pntr_color background = pntr_get_color(map->backgroundcolor);
    pntr_draw_rectangle_fill(dst, posX, posY, map->tilewidth * map->width, map->tileheight * map->height, background);
    pntr_draw_map_layer(dst, map, map->layers, posX, posY, tint);
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_TILED_IMPLEMENTATION_ONCE
#endif  // PNTR_TILED_IMPLEMENTATION
