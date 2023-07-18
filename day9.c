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
        R_V2S head_pos = R_V2S(0, 0);
        R_V2S tail_pos = R_V2S(0, 0);
        R_uint tail_visit_table_cap = 1024*1024;
        R_V2S* tail_visit_table = malloc(sizeof(R_V2S)*tail_visit_table_cap);
        R_uint tail_visit_table_size = 0;

        tail_visit_table[tail_visit_table_size++] = tail_pos;

        while (input.size > 0)
        {
          R_u8 direction;
          R_u32 amount;
          input = R_String_EatN(input, R_String_PatternMatch(input, "%c %u32\r\n", &direction, &amount));

          R_uint direction_index;
          R_int sign;
          if      (direction == 'U') direction_index = 1, sign = +1;
          else if (direction == 'D') direction_index = 1, sign = -1;
          else if (direction == 'R') direction_index = 0, sign = +1;
          else                       direction_index = 0, sign = -1;

          for (R_uint i = 0; i < amount; ++i)
          {
            head_pos.e[direction_index] += sign;

            R_V2S tail_head = R_V2S_Sub(head_pos, tail_pos);

            R_int a = tail_head.e[direction_index] / 2;
            tail_pos.e[direction_index]     += a;
            tail_pos.e[1 - direction_index] += R_ABS(a)*tail_head.e[1 - direction_index];

            R_uint i = 0;
            for (; i < tail_visit_table_size; ++i) if (tail_visit_table[i].x == tail_pos.x && tail_visit_table[i].y == tail_pos.y) break;

            if (i == tail_visit_table_size)
            {
              R_ASSERT(tail_visit_table_size < tail_visit_table_cap);
              tail_visit_table[tail_visit_table_size++] = tail_pos;
            }
          }
        }

        for (R_uint i = 0; i < tail_visit_table_size; ++i)
        {
          R_V2S v = tail_visit_table[i];
          printf("(%d, %d)\n", v.x, v.y);
        }

        printf("Part 1: %llu\n", tail_visit_table_size);
      }

      fclose(input_file);
    }
  }

  return 0;
}
