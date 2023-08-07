#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

typedef struct Sensor
{
  R_s64 x;
  R_s64 y;
  R_s64 bx;
  R_s64 by;
  R_s64 bdist;
} Sensor;

R_s64
ManhattanDistance(R_s64 x0, R_s64 y0, R_s64 x1, R_s64 y1)
{
  R_s64 xdiff = x1 - x0;
  if (x1 < x0) xdiff = x0 - x1;

  R_s64 ydiff = y1 - y0;
  if (y1 < y0) ydiff = y0 - y1;

  return xdiff + ydiff;
}

int
main(int argc, char** argv)
{
  FILE* input_file = fopen("day15_input.txt", "rb");
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
      #define MAX_SENSOR_COUNT 100
      Sensor sensors[MAX_SENSOR_COUNT];
      R_uint sensor_count = 0;

      for (R_String scan = input; scan.size != 0;)
      {
        Sensor* sensor = &sensors[sensor_count++];
        R_ASSERT(sensor->count < MAX_SENSOR_COUNT);
        scan = R_String_EatPatternMatch(scan, "Sensor at x=%s64, y=%s64: closest beacon is at x=%s64, y=%s64\r\n",
                                        &sensor->x, &sensor->y, &sensor->bx, &sensor->by);

        sensor->bdist = ManhattanDistance(sensor->x, sensor->y, sensor->bx, sensor->by);
      }

      R_s64 search_min = 0;
      R_s64 search_max = 4000000;
      R_s64 part1_y = 2000000;

      R_uint part1_result = 0;
      R_uint part2_result = 0;

      for (R_s64 search_y = search_min; part1_result == 0 || part2_result == 0; ++search_y)
      {
        R_ASSERT(search_y <= search_max);

        #define MAX_INTERVAL_COUNT (MAX_SENSOR_COUNT+1)
        R_s64 intervals[MAX_INTERVAL_COUNT][2];
        R_uint interval_count = 0;
        for (R_uint i = 0; i < sensor_count; ++i)
        {
          Sensor* sensor = &sensors[i];

          R_s64 ydiff  = R_ABS(sensor->y - search_y);
          R_s64 xcover = sensor->bdist - ydiff;

          if (xcover < 0) continue;

          R_s64 min_x = sensor->x - xcover;
          R_s64 max_x = sensor->x + xcover;

          R_ASSERT(interval_count < MAX_INTERVAL_COUNT);
          intervals[interval_count][0] = min_x;
          intervals[interval_count][1] = max_x;
          interval_count += 1;

          for (R_uint j = 0; j+1 < interval_count; ++j)
          {
            for (R_uint k = j+1; k < interval_count; ++k)
            {
              R_s64 jmin = intervals[j][0];
              R_s64 jmax = intervals[j][1];
              R_s64 kmin = intervals[k][0];
              R_s64 kmax = intervals[k][1];

              if (kmin <= jmax+1 && kmax >= jmin-1)
              {
                intervals[j][0] = R_MIN(jmin, kmin);
                intervals[j][1] = R_MAX(jmax, kmax);

                intervals[k][0] = intervals[interval_count-1][0];
                intervals[k][1] = intervals[interval_count-1][1];
                --interval_count;

                k = j;
              }
            }
          }
        }

        R_ASSERT(interval_count > 0);
        if (intervals[0][0] > search_min)
        {
          R_ASSERT(part2_result == 0);
          part2_result = (intervals[0][0]-1)*4000000 + search_y;
        }
        else if (intervals[0][1] < search_max)
        {
          R_ASSERT(part2_result == 0);
          part2_result = (intervals[0][1]+1)*4000000 + search_y;
        }

        if (search_y == part1_y)
        {
          for (R_uint i = 0; i < sensor_count; ++i)
          {
            Sensor* sensor = &sensors[i];

            if (sensor->by == part1_y)
            {
              for (R_uint j = 0; j < interval_count; ++j)
              {
                if (sensor->bx >= intervals[j][0] && sensor->bx <= intervals[j][1])
                {
                  R_s64 hmin = sensor->bx + 1;
                  R_s64 hmax = intervals[j][1];

                  if (hmin <= hmax)
                  {
                    R_ASSERT(interval_count < MAX_INTERVAL_COUNT);
                    intervals[interval_count][0] = hmin;
                    intervals[interval_count][1] = hmax;
                    ++interval_count;
                  }

                  intervals[j][1] = sensor->bx - 1;

                  if (intervals[j][0] > intervals[j][1])
                  {
                    intervals[j][0] = intervals[interval_count-1][0];
                    intervals[j][1] = intervals[interval_count-1][1];
                    --interval_count;
                  }
                }
              }
            }
          }

          for (R_uint i = 0; i < interval_count; ++i)
          {
            part1_result += (intervals[i][1]+1) - intervals[i][0];
          }
        }
      }

      printf("Part 1: %llu\n", part1_result);
      printf("Part 2: %llu\n", part2_result);
    }

    fclose(input_file);
  }

  return 0;
}
