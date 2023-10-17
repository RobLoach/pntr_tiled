int tab_count = 0;

#define print_tabs() \
  do { for (int j = 0; j < tab_count; ++j) printf("\t"); } while (0)

#define print_category(name) \
  do { print_tabs(); printf(name " : \n"); } while (0)

#define print(pointer, name, specifier) \
  do { print_tabs(); printf(#name " : " #specifier "\n", pointer -> name); } while (0)

void print_properties(cute_tiled_property_t* properties, int property_count)
{
  print_category("properties");
  ++tab_count;
  for (int i = 0; i < property_count; ++i)
  {
    cute_tiled_property_t* p = properties + i;
    print_tabs();
    printf("%s : ", p->name.ptr);
    switch (p->type)
    {
    case CUTE_TILED_PROPERTY_INT: printf("%d\n", p->data.integer); break;
    case CUTE_TILED_PROPERTY_BOOL: printf("%d\n", p->data.boolean); break;
    case CUTE_TILED_PROPERTY_FLOAT: printf("%f\n", p->data.floating); break;
    case CUTE_TILED_PROPERTY_STRING: printf("%s\n", p->data.string.ptr); break;
    case CUTE_TILED_PROPERTY_FILE: printf("%s\n", p->data.file.ptr); break;
    case CUTE_TILED_PROPERTY_COLOR: printf("%d\n", p->data.color); break;
    case CUTE_TILED_PROPERTY_NONE: printf("CUTE_TILED_PROPERTY_NONE\n"); break;
    }
  }
  --tab_count;
}

void print_objects(cute_tiled_object_t* o)
{
  while (o)
  {
    print_category("object");
    ++tab_count;
    print(o, ellipse, %d);
    print(o, gid, %d);
    print(o, height, %f);
    print(o, id, %d);
    print(o, name.ptr, %s);
    print(o, point, %d);

    print_category("vertices");
    ++tab_count;
    for (int i = 0; i < o->vert_count; i += 2)
    {
      print_tabs();
      printf("%f, %f\n", o->vertices[i], o->vertices[i + 1]);
    }
    --tab_count;

    print(o, vert_type, %d);
    print_properties(o->properties, o->property_count);
    print(o, rotation, %f);
    print(o, type.ptr, %s);
    print(o, visible, %d);
    print(o, width, %f);
    print(o, x, %f);
    print(o, y, %f);

    o = o->next;
    --tab_count;
  }
}

void print_layer(cute_tiled_layer_t* layer)
{
  while (layer)
  {
    print_category("layer"); ++tab_count;
    ++tab_count;
    // for (int i = 0; i < layer->data_count; ++i) print(layer, data[i], %d);
    --tab_count;
    print(layer, draworder.ptr, %s);
    print(layer, height, %d);
    print(layer, name.ptr, %s);
    print_objects(layer->objects);
    print(layer, opacity, %f);
    print_properties(layer->properties, layer->property_count);
    print(layer, type.ptr, %s);
    print(layer, visible, %d);
    print(layer, width, %d);
    print(layer, x, %d);
    print(layer, y, %d);

    print_layer(layer->layers);

    layer = layer->next;
    --tab_count;
  }
}

void print_tilesets(cute_tiled_tileset_t* tileset)
{
  while (tileset)
  {
    print_category("tileset"); ++tab_count;

    print(tileset, columns, %d);
    print(tileset, firstgid, %d);
    print(tileset, image.ptr, %s);
    print(tileset, imagewidth, %d);
    print(tileset, imageheight, %d);
    print(tileset, margin, %d);
    print(tileset, name.ptr, %s);
    print_properties(tileset->properties, tileset->property_count);
    print(tileset, spacing, %d);
    print(tileset, tilecount, %d);
    print(tileset, tileheight, %d);
    print(tileset, tilewidth, %d);
    print(tileset, type.ptr, %s);
    print(tileset, source.ptr, %s);
    print(tileset, objectalignment.ptr, %s);

    cute_tiled_tile_descriptor_t* tile = tileset->tiles;
    print_category("tiles"); ++tab_count;
    while (tile) {
      print(tile, tile_index, %d);
      print(tile, frame_count, %d);
      print_category("frame"); ++tab_count;
      for (int i = 0; i < tile->frame_count; ++i) {
        cute_tiled_frame_t* frame = tile->animation + i;
        print(frame, duration, %d);
        print(frame, tileid, %d);
      }
      --tab_count;
      print_layer(tile->objectgroup);
      print_properties(tile->properties, tile->property_count);
      print(tile, probability, %f);

      tile = tile->next;
    }
    --tab_count;

    tileset = tileset->next;
    --tab_count;
  }
}

void print_map(cute_tiled_map_t* m)
{
  // print all map contents
  print_category("map"); ++tab_count;
  print(m, backgroundcolor, %d);
  print(m, height, %d);
  print(m, infinite, %d);
  print_layer(m->layers);
  print(m, nextobjectid, %d);
  print(m, orientation.ptr, %s);
  print_properties(m->properties, m->property_count);
  print(m, renderorder.ptr, %s);
  print(m, tiledversion.ptr, %s);
  print(m, tileheight, %d);
  // print_tilesets(m->tilesets);
  print(m, tilewidth, %d);
  print(m, type.ptr, %s);
  print(m, version, %f);
  print(m, width, %d);
}
