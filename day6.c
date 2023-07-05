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
        unsigned int i = 0;
        for (;;)
        {
          for (unsigned int j = 0; j < 4; ++j)
          {
            for (unsigned int k = j + 1; k < 4; ++k)
            {
              if (input[i + j] == input[i + k])
              {
                i += 1;
                goto next_1;
              }
            }
          }
          break;
          next_1:;
        }

        printf("Part 1: %u\n", i + 4);

        i = 0;
        for (;;)
        {
          for (unsigned int j = 0; j < 14; ++j)
          {
            for (unsigned int k = j + 1; k < 14; ++k)
            {
              if (input[i + j] == input[i + k])
              {
                i += 1;
                goto next_2;
              }
            }
          }
          break;
          next_2:;
        }

        printf("Part 2: %u\n", i + 14);
      }

      fclose(input_file);
    }
  }

  return 0;
}
