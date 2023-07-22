#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

typedef struct Map_Cell
{
  R_V2S prev;
  R_uint path_length;
  R_uint height;
  R_bool can_visit;
} Map_Cell;

void
InitMap(R_String input, Map_Cell* map, R_uint map_width, R_uint map_height, R_V2S* start_pos, R_V2S* end_pos)
{
  for (R_uint j = 0; j < map_height; ++j)
  {
    for (R_uint i = 0; i < map_width; ++i)
    {
      R_u8 c = input.data[j*(map_width + 2) + i];

      if (c == 'S')
      {
        *start_pos = R_V2S(i, j);
        c = 'a';
      }
      else if (c == 'E')
      {
        *end_pos = R_V2S(i, j);
        c = 'z';
      }

      map[j*map_width + i] = (Map_Cell){ .height = c - 'a', .can_visit = R_true };
    }
  }
}

R_uint
RunBFSToEnd(Map_Cell* map, R_uint map_width, R_uint map_height, R_V2S start_pos, R_V2S end_pos)
{
  R_uint visiting_ringbuffer_size = map_width*map_height;
  R_V2S* visiting_ringbuffer = malloc(sizeof(R_V2S)*visiting_ringbuffer_size);
  R_uint visiting_head = 0;
  R_uint visiting_tail = 0;

  visiting_ringbuffer[visiting_tail++] = start_pos;

  for (;;)
  {
    R_ASSERT(visiting_head != visiting_tail);
    R_V2S current_pos = visiting_ringbuffer[visiting_head];
    visiting_head = (visiting_head + 1) % visiting_ringbuffer_size;

    if (R_V2S_Match(current_pos, end_pos)) break;

    Map_Cell* current_cell = &map[current_pos.y*map_width + current_pos.x];

    R_V2S deltas[4] = { R_V2S(1, 0), R_V2S(0, 1), R_V2S(-1, 0), R_V2S(0, -1) };
    for (R_uint i = 0; i < R_STATIC_ARRAY_SIZE(deltas); ++i)
    {
      R_V2S candidate_pos = R_V2S_Add(current_pos, deltas[i]);

      if (candidate_pos.x >= 0 && candidate_pos.x < map_width &&
          candidate_pos.y >= 0 && candidate_pos.y < map_height)
      {
        Map_Cell* candidate_cell = &map[candidate_pos.y*map_width + candidate_pos.x];

        if (candidate_cell->can_visit && current_cell->height + 1 >= candidate_cell->height)
        {
          candidate_cell->can_visit   = R_false;
          candidate_cell->prev        = current_pos;
          candidate_cell->path_length = current_cell->path_length + 1;

          visiting_ringbuffer[visiting_tail] = candidate_pos;
          visiting_tail = (visiting_tail + 1) % visiting_ringbuffer_size;
        }
      }
    }
  }

  free(visiting_ringbuffer);

  return map[end_pos.y*map_width + end_pos.x].path_length;
}

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day12 <input_file>\n");
  else
  {
    FILE* input_file = fopen(argv[1], "rb");
    if (input_file == 0) fprintf(stderr, "Failed to open input file\n");
    else
    {
      fseek(input_file, 0, SEEK_END);
      R_umm input_file_size = ftell(input_file);
      rewind(input_file);

      R_String input = {
        .data = malloc(input_file_size),
        .size = input_file_size,
      };

      if (input.data == 0 || fread(input.data, 1, input.size, input_file) != input_file_size) fprintf(stderr, "Failed to read input file\n");
      else
      {
        R_uint map_width = 0;
        for (; map_width < input.size && input.data[map_width] != '\r'; ++map_width);

        R_uint map_height = input.size / (map_width + 2);

        R_V2S start_pos = R_V2S(0, 0);
        R_V2S end_pos   = R_V2S(0, 0);

        Map_Cell* map = malloc(sizeof(Map_Cell)*map_width*map_height);

        InitMap(input, map, map_width, map_height, &start_pos, &end_pos);
        R_uint part1_result = RunBFSToEnd(map, map_width, map_height, start_pos, end_pos);

        R_uint part2_result = part1_result;
        R_V2S best_pos      = start_pos;

        for (R_uint j = 0; j < map_height; ++j)
        {
          for (R_uint i = 0; i < map_width; ++i)
          {
            if (input.data[j*(map_width + 2) + i] == 'a')
            {
              InitMap(input, map, map_width, map_height, &(R_V2S){0}, &(R_V2S){0});
              R_uint path_length = RunBFSToEnd(map, map_width, map_height, R_V2S(i, j), end_pos);

              if (path_length < part2_result)
              {
                part2_result = path_length;
                best_pos = R_V2S(i, j);
              }
            }
          }
        }

        printf("Part 1: %llu\n", part1_result);
        printf("Part 2: %llu\n", part2_result);

        InitMap(input, map, map_width, map_height, &(R_V2S){0}, &(R_V2S){0});
        RunBFSToEnd(map, map_width, map_height, best_pos, end_pos);

        for (R_uint i = 0; i < map_width*map_height; ++i) map[i].can_visit = R_false;

        for (R_V2S scan = end_pos;;)
        {
          Map_Cell* scan_cell = &map[scan.y*map_width + scan.x];
          scan_cell->can_visit = R_true;

          if (R_V2S_Match(scan, best_pos)) break;
          else
          {
            scan = scan_cell->prev;
            continue;
          }
        }

        for (R_uint j = 0; j < map_height; ++j)
        {
          for (R_uint i = 0; i < map_width; ++i)
          {
            R_V2S curr = R_V2S(i, j);
            if (R_V2S_Match(curr, end_pos)) printf("E");
            else
            {
              Map_Cell* cell = &map[j*map_width + i];

              if (!cell->can_visit) printf(".");
              else
              {
                R_V2S delta = R_V2S_Sub(curr, cell->prev);

                if      (R_V2S_Match(delta, R_V2S( 1, 0))) printf(">");
                else if (R_V2S_Match(delta, R_V2S( 0, 1))) printf("^");
                else if (R_V2S_Match(delta, R_V2S(-1, 0))) printf("<");
                else                                       printf("v");
              }
            }
          }

          printf("\n");
        }
      }

      fclose(input_file);
    }
  }

  return 0;
}
