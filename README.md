# pntr_tiled

Use [Tiled](https://www.mapeditor.org) maps in [pntr](https://github.com/RobLoach/pntr), with [cute_tiled](https://github.com/RandyGaul/cute_headers/blob/master/cute_tiled.h).

![Screenshot of pntr_tiled_example](examples/pntr_tiled_example.png)

## Usage

``` c
#define PNTR_TILED_IMPLEMENTATION
#include "pntr_tiled.h"

int main() {
    // Load the Tiled JSON file
    cute_tiled_map_t* map = pntr_load_tiled("myworld.json");

    // Generate an image of the Tiled map
    pntr_image* image = pntr_gen_image_tiled(map, PNTR_WHITE);

    // Save the image as a PNG
    pntr_save_image(image, "world.png");

    return 0;
}
```

### API

``` c
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled(const char* fileName);
PNTR_TILED_API cute_tiled_map_t* pntr_load_tiled_from_memory(const unsigned char *fileData, unsigned int dataSize, const char* baseDir);
PNTR_TILED_API void pntr_unload_tiled(cute_tiled_map_t* map);
PNTR_TILED_API void pntr_draw_tiled(pntr_image* dst, cute_tiled_map_t* map, int posX, int posY, pntr_color tint);
PNTR_TILED_API void pntr_draw_tiled_layer(pntr_image* dst, cute_tiled_map_t* map, cute_tiled_layer_t* layer, int posX, int posY, pntr_color tint);
PNTR_TILED_API pntr_image* pntr_gen_image_tiled(cute_tiled_map_t* map, pntr_color tint);
```

## License

Unless stated otherwise, all works are:

- Copyright (c) 2023 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
