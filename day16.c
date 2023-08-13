#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

int
main(int argc, char** argv)
{
  FILE* input_file = fopen("day16_input.txt", "rb");
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
      R_uint line_count = 0;
      for (R_String scan = input; scan.size != 0;)
      {
        while (scan.size != 0 && *scan.data != '\n') scan = R_String_EatN(scan, 1);
        scan = R_String_EatN(scan, 1);
        ++line_count;
      }

      printf("%llu\n", line_count);
    }

    fclose(input_file);
  }

  return 0;
}
