#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "rin.h"

typedef struct List
{
  struct List_Item* items;
  R_u32 size;
} List;

typedef struct List_Item
{
  R_bool is_list;
    R_u32 value;
    List list;
} List_Item;

R_uint
ParseList(R_String input, R_uint i, List* list)
{
  R_ASSERT(i < input.size && input.data[i] == '[');
  ++i;

  list->size = 0;
  for (R_uint j = i;;)
  {
    R_ASSERT(j < input.size);
    if (input.data[j] == ']') break;

    if (R_Char_IsDigit(input.data[j]))
    {
      while (j < input.size && R_Char_IsDigit(input.data[++j]));
      R_ASSERT(j < input.size && (input.data[j] == ',' || input.data[j] == ']'));
    }
    else
    {
      R_ASSERT(input.data[j] == '[');
      ++j;

      R_uint nesting = 1;
      while (nesting != 0)
      {
        R_ASSERT(j < input.size);

        if (input.data[j] == '[')
        {
          ++nesting;
        }
        else if (input.data[j] == ']')
        {
          --nesting;
        }

        ++j;
      }
    }

    ++list->size;
    if (input.data[j] == ',') ++j;
  }

  if (list->size == 0)
  {
    list->items = 0;
    i          += 1;
  }
  else
  {
    //list->items = malloc(sizeof(List_Item)*list->size);
    R_u8* mem = VirtualAlloc(0, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    //list->items = (List_Item*)(mem + (4096 - sizeof(List_Item)*list->size));
    list->items = (List_Item*)mem;

    for (R_uint j = 0; j < list->size; ++j)
    {
      List_Item* item = &list->items[j];

      if (input.data[i] == '[')
      {
        item->is_list = R_true;
        i = ParseList(input, i, &item->list);
      }
      else
      {
        item->is_list = R_false;
        R_int match = R_String_PatternMatch(R_String_EatN(input, i), "%u32", &item->value);
        R_ASSERT(match != -1);
        i += match;
      }

      R_ASSERT(i < input.size && (input.data[i] == ',' || j == list->size-1 && input.data[i] == ']'));
      ++i;
    }
  }

  return i;
}

R_int
CompareLists(List a, List b)
{
  R_bool result = 0;

  for (R_uint i = 0; result == 0 && i < a.size; ++i)
  {
    if (i == b.size) result = -1;
    else
    {
      List_Item item_a = a.items[i];
      List_Item item_b = b.items[i];

      if      (item_a.is_list && item_b.is_list) result = CompareLists(item_a.list, item_b.list);
      else if (item_a.is_list ^ item_b.is_list)
      {
        List list_a;
        List list_b;
        if (item_a.is_list)
        {
          list_a = item_a.list;
          list_b = (List){ .items = &item_b, .size = 1 };
        }
        else
        {
          list_a = (List){ .items = &item_a, .size = 1 };
          list_b = item_b.list;
        }

        result = CompareLists(list_a, list_b);
      }
      else
      {
        R_int subtraction = ((R_int)item_a.value - (R_int)item_b.value);

        result = (subtraction < 0 ? -1 : !!subtraction);
      }
    }
  }

  return result;
}

int
main(int argc, char** argv)
{
  if (argc != 2) fprintf(stderr, "Invalid number of arguments. Expected: day12 <input_file>\n");
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
        R_uint line_count = 0;
        for (R_uint i = 0;;)
        {
          while (i < input.size && input.data[i] != '\r') ++i;
          i += 2;

          line_count += 1;

          if (i >= input.size) break;
          else                 continue;
        }

        R_uint list_count = (line_count + 1) / 2;
        List* lists = malloc(sizeof(List)*list_count);

        R_ASSERT(list_count % 2 == 0);

        R_uint i = 0;

        for (R_uint j = 0, k = 0; j < line_count; ++j)
        {
          if (input.data[i] == '\r')
          {
            i += 2;
            continue;
          }
          else
          {
            List* list = &lists[k++];

            i = ParseList(input, i, list);
            R_ASSERT(i < input.size && input.data[i] == '\r');
            i += 2;
          }
        }

        /*
        R_uint part1_result = 0;

        for (R_uint i = 0; i < list_count; i += 2)
        {
          R_ASSERT(i + 1 < list_count);

          part1_result += (CompareLists(lists[i], lists[i + 1]) > -1 ? i/2 : 0);
        }

        printf("Part 1: %llu\n", part1_result);*/

        printf("[%u", lists[0].items[0].value);
        for (R_uint i = 1; i < lists[0].size; ++i) printf(", %u", lists[0].items[i].value);
        printf("]\n");
      }

      fclose(input_file);
    }
  }

  return 0;
}
