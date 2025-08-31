/*
    gmm2json: program that reads Gridmonger's GMM file and converts it into
   JSON format
    Copyright (C) 2025 Jagholin (github.com/Jagholin)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see
   <https://www.gnu.org/licenses/>
*/
#include <assert.h>
#include <json-c/json_object.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "dynarray.h"
#include "gmm_file.h"

#define JSOBJ_UINT(out, ck, prop)                                              \
  json_object_object_add((out), #prop, json_object_new_uint64((ck).prop))
#define JSOBJ_STR(out, ck, prop)                                               \
  json_object_object_add((out), #prop, json_object_new_string((ck).prop))
#define JSOBJ_INT(out, ck, prop)                                               \
  json_object_object_add((out), #prop, json_object_new_int((ck).prop));
#define JSOBJ_ARR(out, ck, type, prop, size)                                   \
  {                                                                            \
    json_object *new_array = json_object_new_array_ext(size);                  \
    for (size_t i = 0; i < size; ++i) {                                        \
      json_object_array_put_idx(new_array, i,                                  \
                                json_object_new_##type(ck.prop[i]));           \
    }                                                                          \
    json_object_object_add((out), #prop, new_array);                           \
  }

json_object *export_gmm(GmmChunk *ck) {
  json_object *result = json_object_new_object();
  size_t cells_cnt = 0;
  json_object_object_add(result, "chunk_type",
                         json_object_new_string(chunk_type_to_str(ck->ctype)));

  switch (ck->ctype) {
  case GMM_LIST:
    json_object_object_add(
        result, "list_type",
        json_object_new_string_len(ck->list_chunk.ckType, 4));
    size_t child_count = dynarray_size(&ck->list_chunk.children);
    json_object *child_array = json_object_new_array_ext(child_count);

    for (size_t i = 0; i < child_count; ++i) {
      GmmChunk *child = dynarray_get(&ck->list_chunk.children, i);
      assert(child != NULL);

      json_object_array_put_idx(child_array, i, export_gmm(child));
    }
    json_object_object_add(result, "children", child_array);
    break;
  case GMM_MAP_PROP:
    JSOBJ_UINT(result, ck->map_prop_chunk, version);
    JSOBJ_STR(result, ck->map_prop_chunk, title);
    JSOBJ_STR(result, ck->map_prop_chunk, game);
    JSOBJ_STR(result, ck->map_prop_chunk, author);
    JSOBJ_STR(result, ck->map_prop_chunk, creation_time);
    JSOBJ_STR(result, ck->map_prop_chunk, notes);
    break;
  case GMM_MAP_COOR:
    JSOBJ_UINT(result, ck->map_coor_chunk, origin);
    JSOBJ_UINT(result, ck->map_coor_chunk, row_style);
    JSOBJ_UINT(result, ck->map_coor_chunk, column_style);
    JSOBJ_UINT(result, ck->map_coor_chunk, row_start);
    JSOBJ_UINT(result, ck->map_coor_chunk, column_start);
    break;
  case GMM_LVL_PROP:
    JSOBJ_STR(result, ck->level_prop_chunk, location_name);
    JSOBJ_STR(result, ck->level_prop_chunk, level_name);
    JSOBJ_INT(result, ck->level_prop_chunk, elevation);
    JSOBJ_UINT(result, ck->level_prop_chunk, num_rows);
    JSOBJ_UINT(result, ck->level_prop_chunk, num_columns);
    JSOBJ_UINT(result, ck->level_prop_chunk, override_coord_opts);
    JSOBJ_STR(result, ck->level_prop_chunk, notes);
    break;
  case GMM_LVL_COOR:
    JSOBJ_UINT(result, ck->level_coor_chunk, origin);
    JSOBJ_UINT(result, ck->level_coor_chunk, row_style);
    JSOBJ_UINT(result, ck->level_coor_chunk, column_style);
    JSOBJ_UINT(result, ck->level_coor_chunk, row_start);
    JSOBJ_UINT(result, ck->level_coor_chunk, column_start);
    break;
  case GMM_LVL_CELL:
    cells_cnt = ck->level_cell_chunk.cells_count;
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, floor, cells_cnt);
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, floor_orientation,
              cells_cnt);
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, floor_color, cells_cnt);
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, wall_north, cells_cnt);
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, wall_west, cells_cnt);
    JSOBJ_ARR(result, ck->level_cell_chunk, uint64, trail, cells_cnt);
    break;
  case GMM_LVL_ANNO:
    JSOBJ_UINT(result, ck->level_anno_chunk, num_annotations);
    size_t anno_count = ck->level_anno_chunk.num_annotations;
    json_object *anno_array = json_object_new_array_ext(anno_count);
    for (size_t i = 0; i < anno_count; ++i) {
      AnnotationRecord *record = &ck->level_anno_chunk.records[i];
      json_object *anno = json_object_new_object();
      JSOBJ_UINT(anno, *record, row);
      JSOBJ_UINT(anno, *record, column);
      JSOBJ_UINT(anno, *record, kind);
      JSOBJ_STR(anno, *record, text);
      switch (record->kind) {
      case AK_COMMENT:
        break;
      case AK_INDEXED:
        JSOBJ_UINT(anno, record->indexed, index);
        JSOBJ_UINT(anno, record->indexed, index_color);
        break;
      case AK_CUSTOM:
        JSOBJ_STR(anno, record->custom, custom_id);
        break;
      case AK_ICON:
        JSOBJ_UINT(anno, record->icon, icon);
        break;
      case AK_LABEL:
        JSOBJ_UINT(anno, record->label, label_color);
        break;
      }
      json_object_array_put_idx(anno_array, i, anno);
    }
    json_object_object_add(result, "records", anno_array);
    break;
  case GMM_LVL_REGN:
    JSOBJ_UINT(result, ck->level_regn_chunk, enable_regions);
    JSOBJ_UINT(result, ck->level_regn_chunk, rows_per_region);
    JSOBJ_UINT(result, ck->level_regn_chunk, columns_per_region);
    JSOBJ_UINT(result, ck->level_regn_chunk, per_region_coords);
    JSOBJ_UINT(result, ck->level_regn_chunk, num_regions);
    const size_t regn_count = ck->level_regn_chunk.num_regions;
    json_object *regn_array = json_object_new_array_ext(regn_count);
    for (size_t i = 0; i < regn_count; ++i) {
      const LevelRegionRecord *record = &ck->level_regn_chunk.records[i];
      json_object *regn = json_object_new_object();
      JSOBJ_STR(regn, *record, name);
      JSOBJ_STR(regn, *record, notes);
      json_object_array_put_idx(regn_array, i, regn);
    }
    json_object_object_add(result, "records", regn_array);
    break;
  case GMM_MAP_LINKS:
    JSOBJ_UINT(result, ck->map_links_chunk, num_links);
    const size_t links_count = ck->map_links_chunk.num_links;
    json_object *links_array = json_object_new_array_ext(links_count);
    for (size_t i = 0; i < links_count; ++i) {
      const MapLinksRecord *record = &ck->map_links_chunk.records[i];
      json_object *link = json_object_new_object();
      JSOBJ_UINT(link, *record, src_level_index);
      JSOBJ_UINT(link, *record, src_row);
      JSOBJ_UINT(link, *record, src_column);
      JSOBJ_UINT(link, *record, dest_level_index);
      JSOBJ_UINT(link, *record, dest_row);
      JSOBJ_UINT(link, *record, dest_column);
      json_object_array_put_idx(links_array, i, link);
    }
    json_object_object_add(result, "records", links_array);
    break;
  case GMM_UNKNOWN:
    break;
  }

  return result;
}

#undef JSOBJ_STR
#undef JSOBJ_UINT
#undef JSOBJ_INT
#undef JSOBJ_ARR

void print_chunk(GmmChunk *ck, unsigned int tabs) {
  for (unsigned int i = 0; i < tabs; ++i) {
    printf("--");
  }
  RiffChunkHeader *head = (RiffChunkHeader *)ck;
  char ck_type[5];
  strncpy(ck_type, (char *)head->ckId, 4);
  ck_type[4] = '\0';
  printf("'%s' riff chunk of size %d bytes.\n", ck_type, head->ckSize);

  if (ck->ctype == GMM_LIST) {
    for (unsigned int i = 0; i < tabs; ++i) {
      printf("  ");
    }
    strncpy(ck_type, (char *)ck->list_chunk.ckType, 4);
    printf("LIST chunk type: '%s'\n", ck_type);
    for (unsigned int i = 0; i < dynarray_size(&ck->list_chunk.children); ++i) {
      print_chunk((GmmChunk *)dynarray_get(&ck->list_chunk.children, i),
                  tabs + 1);
    }
  }
}

int main(int argc, char **argv) {
  FILE *gmfile = NULL;
  Context ctx;
  RiffFile gmm_data;

  last_error = RES_OK;

  assert(sizeof(uint8) == 1);
  assert(sizeof(uint16) == 2);
  assert(sizeof(uint32) == 4);
  if (argc != 2) {
    printf("%s\n", "gmm2json is a to-json converter for Gridmonger .gmm files");
    printf("Usage: %s <file_name>\n\n", argv[0]);
    printf("gmm2json Copyright (C) 2025 Jagholin.\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it \n");
    printf("under certain conditions. See COPYING and COPYING.LESSER for more "
           "details\n");
    return EXIT_SUCCESS;
  }
  // printf("Opening file: %s\n", argv[1]);
  gmfile = fopen(argv[1], "rb");
  if (!gmfile) {
    printf("Cannot open file %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  ctx.file_name = argv[1];
  gmm_data = read_riff(gmfile, &ctx);
  // printf("Loaded GMM file with length: %u\n", gmm_data.length);
  Dynarray chunks = decode_chunks(&gmm_data);
  // Dynarray chunks = decode_chunks(&gmm_data, 0, 0, NULL);
  // for (unsigned int i = 0; i < dynarray_size(&chunks); ++i) {
  //   print_chunk((GmmChunk *)dynarray_get(&chunks, i), 0);
  // }

  json_object *gmm_array = json_object_new_array_ext(dynarray_size(&chunks));
  for (size_t i = 0; i < dynarray_size(&chunks); ++i) {
    json_object *gmm_json = export_gmm(dynarray_get(&chunks, i));
    json_object_array_put_idx(gmm_array, i, gmm_json);
  }
  const char *output = json_object_to_json_string(gmm_array);
  printf("%s\n", output);

  json_object_put(gmm_array);
  free_chunks(&chunks);
  free_gmmfile(&gmm_data);
  fclose(gmfile);
  return EXIT_SUCCESS;
}
