#if 0
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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
      uint32_t input_file_size = ftell(input_file);
      rewind(input_file);

      char* input = malloc(input_file_size);

      if (input == 0 || fread(input, 1, input_file_size, input_file) != input_file_size) fprintf(stderr, "Failed to read input file\n");
      else
      {
        uint64_t part1_result = 0;
        uint64_t part2_result = 0;

        for (int i = 0; i < input_file_size; )
        {
          uint64_t a0 = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) a0 = a0*10 + (input[i++] & 0xF);

          ++i; // skip -

          uint64_t a1 = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) a1 = a1*10 + (input[i++] & 0xF);

          ++i; // skip ,
          
          uint64_t b0 = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) b0 = b0*10 + (input[i++] & 0xF);

          ++i; // skip -

          uint64_t b1 = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) b1 = b1*10 + (input[i++] & 0xF);

          i += 2; // skip \r\n
          
          if (a0 <= b0 && a1 >= b1 || b0 <= a0 && b1 >= a1) part1_result += 1;
          if (!(a0 > b1 || a1 < b0)) part2_result += 1;
        }

        printf("Part 1: %llu\n", part1_result);
        printf("Part 2: %llu\n", part2_result);
      }

      fclose(input_file);
    }
  }

  return 0;
}
#else
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
      uint32_t input_file_size = ftell(input_file);
      rewind(input_file);

      R_String input_string = {
        .data = malloc(input_file_size),
        .size = input_file_size,
      };

      if (input_string.data == 0 || fread(input_string.data, 1, input_file_size, input_file) != input_file_size) fprintf(stderr, "Failed to read input file\n");
      else
      {
        uint64_t part1_result = 0;
        uint64_t part2_result = 0;

        while (input_string.size != 0)
        {
          uint64_t a0, a1, b0, b1;
          input_string = R_String_EatN(input_string, R_String_PatternMatch(input_string, "%u64-%u64,%u64-%u64\r\n", &a0, &a1, &b0, &b1));
          
          if (a0 <= b0 && a1 >= b1 || b0 <= a0 && b1 >= a1) part1_result += 1;
          if (!(a0 > b1 || a1 < b0)) part2_result += 1;
        }

        printf("Part 1: %llu\n", part1_result);
        printf("Part 2: %llu\n", part2_result);
      }

      fclose(input_file);
    }
  }

  return 0;
}
#endif
