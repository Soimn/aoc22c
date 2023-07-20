#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day2 <input_file>\n");
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
        R_V2S knots[10] = {0};

        R_uint tail_table_cap = 100000;
        R_V2S* tail_tables[2] = {
          [0] = malloc(sizeof(R_V2S)*tail_table_cap),
          [1] = malloc(sizeof(R_V2S)*tail_table_cap),
        };
        R_uint tail_table_size[2] = {0};

        tail_tables[0][tail_table_size[0]++] = R_V2S(0, 0);
        tail_tables[1][tail_table_size[1]++] = R_V2S(0, 0);

        while (input.size > 0)
        {
          R_u8 dir_char;
          R_u32 amount;
          input = R_String_EatN(input, R_String_PatternMatch(input, "%c %u32\r\n", &dir_char, &amount));

          R_int sign;
          R_uint direction_index;
          if      (dir_char == 'U') direction_index = 1, sign = +1;
          else if (dir_char == 'D') direction_index = 1, sign = -1;
          else if (dir_char == 'R') direction_index = 0, sign = +1;
          else                      direction_index = 0, sign = -1;

          for (R_uint i = 0; i < amount; ++i)
          {
            knots[0].e[direction_index] += sign;

            for (R_uint j = 1; j < R_STATIC_ARRAY_SIZE(knots); ++j)
            {
              R_V2S diff      = R_V2S_Sub(knots[j-1], knots[j]);
              R_V2S diff_on_2 = R_V2S(diff.x/2, diff.y/2);

              if (diff_on_2.x != 0 && diff_on_2.y != 0)
              {
                knots[j].x += diff_on_2.x;
                knots[j].y += diff_on_2.y;
              }
              else if (diff_on_2.x != 0)
              {
                knots[j].x += diff_on_2.x;
                knots[j].y += diff.y;
              }
              else if (diff_on_2.y != 0)
              {
                knots[j].y += diff_on_2.y;
                knots[j].x += diff.x;
              }
            }

            for (R_uint j = 0; j < 2; ++j)
            {
              R_uint tail_i = (R_uint[2]){ 1, R_STATIC_ARRAY_SIZE(knots)-1 }[j];

              R_uint k = 0;
              for (; k < tail_table_size[j]; ++k) if (tail_tables[j][k].x == knots[tail_i].x && tail_tables[j][k].y == knots[tail_i].y) break;
              if (k == tail_table_size[j])
              {
                R_ASSERT(tail_table_size[j] < tail_table_cap);
                tail_tables[j][tail_table_size[j]++] = knots[tail_i];
              }
            }
          }
        }

        printf("Part 1: %llu\n", tail_table_size[0]);
        printf("Part 2: %llu\n", tail_table_size[1]);
      }

      fclose(input_file);
    }
  }

  return 0;
}
