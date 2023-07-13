#if 0
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day1 <input_file>\n");
  else
  {
    FILE* input_file = fopen(argv[1], "r");

    if (input_file == 0) fprintf(stderr, "Failed to open input file\n");
    else
    {
      unsigned long long max_cals[3] = {0};
      char c = fgetc(input_file);
      while (c != EOF)
      {
        unsigned long long acc_cal = 0;
        for (;;)
        {
          unsigned long long cal = 0;
          while ((unsigned char)(c - 0x30) < 10u)
          {
            cal = cal*10 + (c & 0xF);
            c = fgetc(input_file);
          }

          acc_cal += cal;

          if (c == '\n') c = fgetc(input_file);

          if      ((unsigned char)(c - 0x30) < 10u) continue;
          else if (c == '\n' || c == EOF)
          {
            while (c == '\n') c = fgetc(input_file);

            for (unsigned int i = 0; i < sizeof(max_cals)/sizeof(max_cals[0]); ++i)
            {
              if (acc_cal > max_cals[i])
              {
                unsigned long long old_max = max_cals[i];
                max_cals[i] = acc_cal;
                acc_cal = old_max;
              }
            }

            break;
          }
          else
          {
            fprintf(stderr, "Invalid input format, %d\n", c);
            goto close_file;
          }
        }
      }

      printf("Part 1: %llu\n", max_cals[0]);
      printf("Part 2: %llu\n", max_cals[0] + max_cals[1] + max_cals[2]);

      close_file:
      fclose(input_file);
    }
  }

  return 0;
}
#else
#include <stdlib.h>
#include <stdio.h>
#include "rin.h"

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day1 <input_file>\n");
  else
  {
    FILE* input_file = fopen(argv[1], "rb");

    if (input_file == 0) fprintf(stderr, "Failed to open input file\n");
    else
    {
      fseek(input_file, 0, SEEK_END);
      R_u64 input_file_size = ftell(input_file);
      rewind(input_file);

      R_String input_string = {
        .data = malloc(input_file_size),
        .size = input_file_size,
      };

      if (input_string.data == 0 || fread(input_string.data, 1, input_file_size, input_file) != input_file_size)
      {
        fprintf(stderr, "Failed to read input file\n");
      }
      else
      {
        unsigned long long max_cals[3] = {0};
        while (input_string.size != 0)
        {
          R_u64 acc_cal = 0;
          for (;;)
          {
            R_u64 cal;
            R_umm matched_chars = R_String_PatternMatch(input_string, "%u64\r\n", &cal);
            input_string = R_String_EatN(input_string, matched_chars);

            acc_cal += cal;

            if      (input_string.size != 0 && R_Char_IsDigit(*input_string.data)) continue;
            else
            {
              while (input_string.size != 0 && (*input_string.data == '\r' || *input_string.data == '\n')) input_string = R_String_EatN(input_string, 1);

              for (unsigned int i = 0; i < R_STATIC_ARRAY_SIZE(max_cals); ++i)
              {
                if (acc_cal > max_cals[i])
                {
                  unsigned long long old_max = max_cals[i];
                  max_cals[i] = acc_cal;
                  acc_cal = old_max;
                }
              }

              break;
            }
          }
        }

        printf("Part 1: %llu\n", max_cals[0]);
        printf("Part 2: %llu\n", max_cals[0] + max_cals[1] + max_cals[2]);

        fclose(input_file);
      }
    }
  }

  return 0;
}
#endif
