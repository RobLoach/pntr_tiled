/**********************************************************************************************
*
*   pntr_tiled_assetsys - pntr_tiled assetsys integration.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       cute_tiled https://github.com/RandyGaul/cute_headers/blob/master/cute_tiled.h
*       pntr https://github.com/robloach/pntr_assetsys
*
*   LICENSE: zlib/libpng
*
*   pntr_tiled_assetsys is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
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

#ifndef PNTR_TILED_ASSETSYS_H__
#define PNTR_TILED_ASSETSYS_H__

/**
 * Load a Tiled map from assetsys.
 *
 * @param sys The assetsys to load from.
 * @param fileName The file name of the Tiled map.
 *
 * @return The loaded map, or NULL on failure.
 */
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_assetsys(assetsys_t* sys, const char* fileName);

#endif  // PNTR_TILED_ASSETSYS_H__

#ifdef PNTR_TILED_IMPLEMENTATION
#ifndef PNTR_TILED_ASSETSYS_IMPLEMENTATION_ONCE
#define PNTR_TILED_ASSETSYS_IMPLEMENTATION_ONCE

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
}

/**
 * Load the given layer images from assetsys.
 *
 * @internal
 * @private
 */
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
        if (tileset->transparentcolor != 0) {
            pntr_image_color_replace((pntr_image*)tileset->image.ptr, _pntr_get_tiled_color(tileset->transparentcolor), PNTR_BLANK);
        }
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

#endif
#endif
