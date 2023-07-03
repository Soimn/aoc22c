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

        for (uint32_t i = 0; i < input_file_size; )
        {
          uint64_t group_mask = ~0ull;
          for (uint32_t k = 0; k < 3; ++k)
          {
            uint32_t start_i = i;
            while (i < input_file_size && input[i] != '\r') ++i;
            uint32_t end_i = i;

            uint32_t len = end_i - start_i;

            uint64_t masks[2] = {0};
            for (uint32_t j = 0; j < len; ++j)
            {
              char c = input[j + start_i];
              masks[j >= len/2] |= (1ull << ((c & 0x1F) + 26*(c < 0x60)));
            }

            unsigned long dup_item = 0;
            _BitScanForward64(&dup_item, masks[0] & masks[1]);

            part1_result += dup_item;

            group_mask &= (masks[0] | masks[1]);

            i += 2; // Skip \r\n
          }

          unsigned long dup_item = 0;
          _BitScanForward64(&dup_item, group_mask);

          part2_result += dup_item;
        }

        printf("Part 1: %llu\n", part1_result);
        printf("Part 2: %llu\n", part2_result);
      }

      fclose(input_file);
    }
  }

  return 0;
}
