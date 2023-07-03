#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day2 <input_file>\n");
  else
  {
    FILE* input_file = fopen(argv[1], "r");
    if (input_file == 0) fprintf(stderr, "Failed to open input file\n");
    else
    {
      unsigned long long part1_score = 0;
      unsigned long long part2_score = 0;

      char c = 0;
      for (;;)
      {
        char c = fgetc(input_file);
        if (c == EOF) break;

        char opponents_move = c - 'A';
        fgetc(input_file);
        char your_move = fgetc(input_file) - 'X';
        fgetc(input_file);
        
        part1_score += (your_move + 1) + (int[]){6, 0, 3, 6, 0, 3}[(your_move+2)%3 + (3-opponents_move)];

        part2_score += (int[]){3, 1, 2, 3, 1}[your_move + opponents_move] + your_move*3;
      }

      printf("Part 1: %llu\n", part1_score);
      printf("Part 2: %llu\n", part2_score);

      fclose(input_file);
    }
  }

  return 0;
}
