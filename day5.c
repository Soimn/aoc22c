#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct Stack
{
  char* data;
  unsigned int height;
} Stack;

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
        int i = 0;

        unsigned int highest_stack = 0;
        unsigned int stack_count   = 0;

        while (!(input[i] == ' ' && input[i + 1] == '1'))
        {
          while (input[i] != '\n') ++i;
          ++i; // skip \n
          ++highest_stack;
        }

        unsigned int line_length = 0;
        while (input[i] != '\n') ++i, ++line_length;

        stack_count = line_length / 4;

        unsigned int stack_stride = highest_stack*stack_count;
        Stack* p1_stacks = malloc(stack_count*sizeof(Stack));
        char* p1_stack_memory = malloc(stack_count*stack_stride);

        Stack* p2_stacks = malloc(stack_count*sizeof(Stack));
        char* p2_stack_memory = malloc(stack_count*stack_stride);

        for (unsigned int j = 0; j < stack_count; ++j)
        {
          p1_stacks[j] = (Stack){ .height = 0, .data = p1_stack_memory + stack_stride*j };
          p2_stacks[j] = (Stack){ .height = 0, .data = p2_stack_memory + stack_stride*j };
        }

        i = 0;
        for (unsigned int j = 0; j < highest_stack; ++j)
        {
          i += 1;
          for (unsigned int k = 0; k < stack_count; ++k)
          {
            if (input[i] != ' ')
            {
              for (int h = p1_stacks[k].height; h >= 0; --h)
              {
                p1_stacks[k].data[h + 1] = p1_stacks[k].data[h];
              }
              p1_stacks[k].data[0] = input[i];
              p1_stacks[k].height += 1;

              for (int h = p2_stacks[k].height; h >= 0; --h)
              {
                p2_stacks[k].data[h + 1] = p2_stacks[k].data[h];
              }
              p2_stacks[k].data[0] = input[i];
              p2_stacks[k].height += 1;
            }

            i += 4;
          }
        }

        while (input[i] != '\n') ++i;
        i += 3; // skip \n\r\n

        for (;;)
        {
          if (i >= input_file_size) break;

          i += sizeof("move ") - 1;

          unsigned int amount = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) amount = amount*10 + (input[i++] & 0xF);

          i += sizeof(" from ") - 1;

          unsigned int src = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) src = src*10 + (input[i++] & 0xF);
          src -= 1; // 1-index to 0-index

          i += sizeof(" to ") - 1;

          unsigned int dst = 0;
          while ((unsigned char)(input[i] - 0x30) < 10u) dst = dst*10 + (input[i++] & 0xF);
          dst -= 1; // 1-index to 0-index

          i += sizeof("\r\n") - 1;

          
          for (unsigned int j = 0; j < amount; ++j)
          {
            p1_stacks[dst].data[p1_stacks[dst].height++] = p1_stacks[src].data[--p1_stacks[src].height];
          }
          
          for (unsigned int j = 0; j < amount; ++j)
          {
            p2_stacks[dst].data[p2_stacks[dst].height + j] = p2_stacks[src].data[p2_stacks[src].height - (amount - j)];
          }
          p2_stacks[src].height -= amount;
          p2_stacks[dst].height += amount;
        }

        printf("Part 1: ");
        for (unsigned int j = 0; j < stack_count; ++j) printf("%c", p1_stacks[j].data[p1_stacks[j].height - 1]);
        printf("\n");
        printf("Part 2: ");
        for (unsigned int j = 0; j < stack_count; ++j) printf("%c", p2_stacks[j].data[p2_stacks[j].height - 1]);
        printf("\n");
      }

      fclose(input_file);
    }
  }

  return 0;
}
