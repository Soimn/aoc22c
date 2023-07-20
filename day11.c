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
        typedef struct Monkey
        {
          R_u32 items[256];
          R_u32 item_count;
          R_s32 time;
          R_u32 add;
          R_u32 divisor;
          R_u32 true_monkey;
          R_u32 false_monkey;
          R_u64 inspection_count;
        } Monkey;

        Monkey monkeys[10] = {0};
        R_uint monkey_count = 0;

        while (input.size > 0)
        {
          R_u32 monkey_index;
          input = R_String_EatN(input, R_String_PatternMatch(input, "Monkey %u32:\r\n", &monkey_index));
          input = R_String_EatN(input, R_String_PatternMatch(input, "  Starting items: "));

          R_ASSERT(monkey_index == monkey_count && monkey_index < R_STATIC_ARRAY_SIZE(monkeys));
          Monkey* monkey = &monkeys[monkey_count++];

          for (;;)
          {
            R_u32 value;
            input = R_String_EatN(input, R_String_PatternMatch(input, "%u32", &value));
            monkey->items[monkey->item_count++] = value;

            R_int eat = R_String_PatternMatch(input, ", ");
            if (eat != -1) input = R_String_EatN(input, eat);
            else
            {
              input = R_String_EatN(input, sizeof("\r\n") - 1);
              break;
            }
          }

          input = R_String_EatN(input, R_String_PatternMatch(input, "  Operation: new = "));

          R_u32 time;
          R_u32 add;
          R_int eat_old  = R_String_PatternMatch(input, "old * old\r\n");
          R_int eat_time = R_String_PatternMatch(input, "old * %u32\r\n", &time);
          R_int eat_add  = R_String_PatternMatch(input, "old + %u32\r\n", &add);
          if (eat_old != -1)
          {
            monkey->time = -1;
            monkey->add  = 0;
            input = R_String_EatN(input, eat_old);
          }
          else if (eat_time != -1)
          {
            monkey->time = time;
            monkey->add  = 0;
            input = R_String_EatN(input, eat_time);
          }
          else
          {
            monkey->time = 1;
            monkey->add  = add;
            input = R_String_EatN(input, eat_add);
          }

          input = R_String_EatN(input, R_String_PatternMatch(input, "  Test: divisible by %u32\r\n", &monkey->divisor));

          input = R_String_EatN(input, R_String_PatternMatch(input, "    If true: throw to monkey %u32\r\n", &monkey->true_monkey));
          input = R_String_EatN(input, R_String_PatternMatch(input, "    If false: throw to monkey %u32\r\n", &monkey->false_monkey));

          input = R_String_EatN(input, sizeof("\r\n") - 1);
        }

        for (R_uint round = 0; round < 20; ++round)
        {
          for (R_uint i = 0; i < monkey_count; ++i)
          {
            Monkey* monkey = &monkeys[i];

            while (monkey->item_count > 0)
            {
              monkey->inspection_count += 1;

              R_uint item = monkey->items[0];
              R_MemCopy(&monkey->items[1], &monkey->items[0], sizeof(monkey->items) - sizeof(monkey->items[0]));
              monkey->item_count -= 1;

              item = item*(monkey->time == -1 ? item : (R_uint)monkey->time) + (R_uint)monkey->add;

              item /= 3;

              R_uint throw_i = (item % monkey->divisor == 0 ? monkey->true_monkey : monkey->false_monkey);
              monkeys[throw_i].items[monkeys[throw_i].item_count++] = item;
            }
          }

          if (round == 0 || round == 19 || round == 999)
          {
            for (R_uint i = 0; i < monkey_count; ++i)
            {
              printf("Monkey %llu inspected items %llu times.\n", i, monkeys[i].inspection_count);
            }

            printf("\n");

            if (round == 999) break;
          }
        }

        printf("\n\n");

        R_uint max_2[2] = {0};
        for (R_uint i = 0; i < monkey_count; ++i)
        {
          R_uint n = monkeys[i].inspection_count;
          if (max_2[0] < n) max_2[0] ^= (n ^= (max_2[0] ^= n));
          if (max_2[1] < n) max_2[1] ^= (n ^= (max_2[1] ^= n));
        }

        printf("%llu\n", max_2[0]*max_2[1]);
      }

      fclose(input_file);
    }
  }

  return 0;
}
