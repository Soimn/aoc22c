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
        uint64_t width  = 0;
        uint64_t height = 0;

        for (char* scan = input; *scan != '\r'; ++scan) ++width;
        height = input_file_size / (width + 2);

        unsigned char* visible_trees = malloc(sizeof(unsigned char)*width*height);
        for (uint64_t i = 0; i < width*height; ++i) visible_trees[i] = ((i/width) % (height-1) == 0 || i % width == 0 || i % width == width - 1);

        for (uint64_t j = 0; j < height; ++j)
        {
          for (uint64_t i = 0; i < width; ++i)
          {
            printf("%u", visible_trees[j*width + i]);
          }
          printf("\n");
        }
        printf("\n");

        unsigned char* north_front = malloc(sizeof(int)*width);
        unsigned char* south_front = malloc(sizeof(int)*width);

        for (uint64_t i = 0; i < width; ++i)
        {
          north_front[i] = input[i];
          south_front[i] = input[(height - 1)*(width + 2) + i];
        }

        for (uint64_t i = 0; i < height; ++i)
        {
          if (i > 0 && i < width)
          {
            char* north_line = input + i*(width + 2);
            char* south_line = input + ((height-1) - i)*(width + 2);

            for (uint64_t j = 0; j < width; ++j)
            {
              unsigned char north_scan = north_line[j];
              unsigned char south_scan = south_line[j];

              visible_trees[i*width + j]              |= (north_scan > north_front[j]);
              visible_trees[((height-1)-i)*width + j] |= (south_scan > south_front[j]);

              north_front[j] = (north_scan > north_front[j] ? north_scan : north_front[j]);
              south_front[j] = (south_scan > south_front[j] ? south_scan : south_front[j]);
            }
          }

          char* line = input + i*(width + 2);

          unsigned char west_front = line[0];
          unsigned char east_front = line[width - 1];

          for (uint64_t j = 1; j < width; ++j)
          {
            unsigned char west_scan = line[j];
            unsigned char east_scan = line[(width - 1) - j];

            visible_trees[i*width + j]           |= (west_scan > west_front);
            visible_trees[i*width + (width-1)-j] |= (east_scan > east_front);

            west_front = (west_scan > west_front ? west_scan : west_front);
            east_front = (east_scan > east_front ? east_scan : east_front);
          }
        }

        for (uint64_t j = 0; j < height; ++j)
        {
          for (uint64_t i = 0; i < width; ++i)
          {
            printf("%u", visible_trees[j*width + i]);
          }
          printf("\n");
        }
        printf("\n");

        uint64_t part1_result = 0;
        uint64_t part2_result = 0;

        for (unsigned int i = 0; i < width*height; ++i) part1_result += visible_trees[i];

        printf("Part 1: %llu\n", part1_result);

        for (uint64_t j = 0; j < height; ++j)
        {
          for (uint64_t i = 0; i < width; ++i)
          {
            unsigned char center_height = input[j*(width+2) + i];

            uint64_t north_score = 0;
            for (uint64_t k = 1; k < j + 1; ++k)
            {
              north_score += 1;
              if (input[(j - k)*(width+2) + i] > center_height) break;
            }

            uint64_t south_score = 0;
            for (uint64_t k = 1; k < height - j; ++k)
            {
              south_score += 1;
              if (input[(j + k)*(width+2) + i] > center_height) break;
            }

            uint64_t east_score = 0;
            for (uint64_t k = 1; k < width - j; ++k)
            {
              east_score += 1;
              if (input[j*(width+2) + i + k] > center_height) break;
            }

            uint64_t west_score = 0;
            for (uint64_t k = 1; k < i + 1; ++k)
            {
              west_score += 1;
              if (input[j*(width+2) + (i  - k)] > center_height) break;
            }

            uint64_t score = north_score*south_score*east_score*west_score;

              printf("%llu, %llu: %llu[%llu, %llu, %llu, %llu]\n", i, j, score, north_score, south_score, east_score, west_score);
            if (score > part2_result)
            {
              part2_result = score;
            }
          }
        }

        printf("Part 2: %llu\n", part2_result);
      }

      fclose(input_file);
    }
  }

  return 0;
}
