#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

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
        R_V2S start_pos    = R_V2S(0, 0);
        R_V2S end_pos      = R_V2S(0, 0);
        R_uint* height_map;
        R_uint map_width;
        R_uint map_height;

        for (map_width = 0; input.data[map_width] != '\r'; ++map_width);
        map_height = input.size / (map_width + 2);

        height_map = malloc(sizeof(R_uint)*map_width*map_height);

        for (R_uint j = 0; j < map_height; ++j)
        {
          for (R_uint i = 0; i < map_width; ++i)
          {
            R_u8 c = input.data[j*(map_width + 2) + i];

            if (c == 'S')
            {
              start_pos = R_V2S(i, j);
              c         = 'a';
            }
            else if (c == 'E')
            {
              end_pos = R_V2S(i, j);
              c       = 'z';
            }

            height_map[j*map_width + i] = c - 'a';
          }
        }

        R_uint visiting_ringbuffer_size = map_width*map_height;
        R_V2S* visiting_ringbuffer      = malloc(sizeof(R_V2S)*visiting_ringbuffer_size);
        R_uint visiting_head = 0;
        R_uint visiting_tail = 0;

        visiting_ringbuffer[visiting_tail] = start_pos;
        visiting_tail = (visiting_tail + 1) % visiting_ringbuffer_size;

        R_uint* path_length_map = calloc(map_width*map_height, sizeof(R_uint));
        path_length_map[start_pos.y*map_width + start_pos.x] = 1;

        for (;;)
        {
          R_ASSERT(visiting_head != visiting_tail);
          R_V2S curr_pos = visiting_ringbuffer[visiting_head];
          visiting_head = (visiting_head + 1) % visiting_ringbuffer_size;

          R_uint curr_index = curr_pos.y*map_width + curr_pos.x;
          R_uint curr_height = height_map[curr_index];
          R_uint curr_len    = path_length_map[curr_index];

          if (R_V2S_Match(curr_pos, end_pos))
          {
            printf("Part 1: %llu\n", curr_len - 1);
            break;
          }

          R_V2S deltas[] = { R_V2S(1, 0), R_V2S(0, 1), R_V2S(-1, 0), R_V2S(0, -1) };
          for (R_uint i = 0; i < R_STATIC_ARRAY_SIZE(deltas); ++i)
          {
            R_V2S cand_pos = R_V2S_Add(curr_pos, deltas[i]);

            if (cand_pos.x >= 0 && cand_pos.x < map_width &&
                cand_pos.y >= 0 && cand_pos.y < map_height)
            {
              R_uint cand_index = cand_pos.y*map_width + cand_pos.x;
              R_uint cand_height = height_map[cand_index];
              R_uint* cand_len   = &path_length_map[cand_index];

              if (curr_height + 1 >= cand_height && *cand_len == 0)
              {
                *cand_len = curr_len + 1;

                visiting_ringbuffer[visiting_tail] = cand_pos;
                visiting_tail = (visiting_tail + 1) % visiting_ringbuffer_size;
              }
            }
          }
        }


        visiting_head = 0;
        visiting_tail = 0;

        visiting_ringbuffer[visiting_tail] = end_pos;
        visiting_tail = (visiting_tail + 1) % visiting_ringbuffer_size;

        memset(path_length_map, 0, sizeof(R_uint)*map_width*map_height);
        path_length_map[end_pos.y*map_width + end_pos.x] = 1;

        for (;;)
        {
          R_V2S curr_pos = visiting_ringbuffer[visiting_head];
          visiting_head = (visiting_head + 1) % visiting_ringbuffer_size;

          R_uint curr_index = curr_pos.y*map_width + curr_pos.x;
          R_uint curr_height = height_map[curr_index];
          R_uint curr_len    = path_length_map[curr_index];

          if (curr_height == 0)
          {
            printf("Part 2: %llu\n", curr_len - 1);
            break;
          }
          else
          {
            R_V2S deltas[] = { R_V2S(1, 0), R_V2S(0, 1), R_V2S(-1, 0), R_V2S(0, -1) };
            for (R_uint i = 0; i < R_STATIC_ARRAY_SIZE(deltas); ++i)
            {
              R_V2S cand_pos = R_V2S_Add(curr_pos, deltas[i]);

              if (cand_pos.x >= 0 && cand_pos.x < map_width &&
                  cand_pos.y >= 0 && cand_pos.y < map_height)
              {
                R_uint cand_index = cand_pos.y*map_width + cand_pos.x;
                R_uint cand_height = height_map[cand_index];
                R_uint* cand_len   = &path_length_map[cand_index];

                if (curr_height <= cand_height + 1 && *cand_len == 0)
                {
                  *cand_len = curr_len + 1;

                  visiting_ringbuffer[visiting_tail] = cand_pos;
                  visiting_tail = (visiting_tail + 1) % visiting_ringbuffer_size;
                }
              }
            }
          }
        }
      }

      fclose(input_file);
    }
  }

  return 0;
}
