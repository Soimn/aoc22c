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
        R_int part1_result = 0;

        R_int x = 1;
        R_uint cycle = 1;
        while (input.size > 0)
        {
          R_int new_x;
          R_uint cycle_stall;

          R_umm eat;
          if ((eat = R_String_PatternMatch(input, "noop\r\n")) != -1)
          {
            input = R_String_EatN(input, eat);

            new_x       = x;
            cycle_stall = 1;
          }
          else
          {
            R_s32 v;
            input = R_String_EatN(input, R_String_PatternMatch(input, "addx %s32\r\n", &v));

            new_x       = x + v;
            cycle_stall = 2;
          }

          for (R_uint i = 0; i < cycle_stall; ++i, ++cycle)
          {
            R_int c = (cycle-1) % 40;
            printf("%s%s", (c == 0 ? "\n" : ""), ((x - c)/2 == 0 ? "#" : "."));

            if (cycle % 20 == 0 && (cycle/20) % 2 == 1)
            {
              part1_result += cycle*x;
            }
          }

          x = new_x;
        }

        printf("Part 1: %lld\n", part1_result);
      }

      fclose(input_file);
    }
  }

  return 0;
}
