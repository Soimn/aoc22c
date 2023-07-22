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

void
ParseList(R_String string, List* list)
{
  R_ASSERT(string.size != 0 && string.data[0] == '[' && string.data[string.size-1] == ']');
  string = R_String_EatN(R_String_ChopN(string, 1), 1);

  list->size = 0;
  for (R_uint i = 0; i < string.size;)
  {
    if (string.data[i] == '[')
    {
      ++i;

      R_uint nesting = 1;
      while (nesting != 0)
      {
        if      (string.data[i] == '[') ++nesting;
        else if (string.data[i] == ']') --nesting;
        ++i;
      }
    }
    else
    {
      R_ASSERT(R_Char_IsDigit(string.data[i]));
      while (R_Char_IsDigit(string.data[i])) ++i;
    }

    ++list->size;
    if (string.data[i] == ',') ++i;
  }

  if (list->size == 0) list->items = 0;
  else
  {
    list->items = calloc(list->size, sizeof(List_Item));

    for (R_uint i = 0; string.size > 0;)
    {
      List_Item* item = &list->items[i++];

      if (R_Char_IsDigit(*string.data))
      {
        item->is_list = R_false;
        string = R_String_EatN(string, R_String_PatternMatch(string, "%u32", &item->value));
      }
      else
      {
        R_ASSERT(*string.data == '[');

        R_String nested_string = {
          .data = string.data,
          .size = 0,
        };

        string = R_String_EatN(string, sizeof("[") - 1);
        R_uint nesting = 1;
        while (nesting != 0)
        {
          if      (*string.data == '[') ++nesting;
          else if (*string.data == ']') --nesting;
          string = R_String_EatN(string, 1);
        }

        nested_string.size = string.data - nested_string.data;

        item->is_list = R_true;
        ParseList(nested_string, &item->list);
      }

      string = R_String_EatN(string, sizeof(",") - 1);
    }
  }
}

R_int
CompareLists(List a, List b)
{
  R_int result = 0;

  for (R_uint i = 0; result == 0; ++i)
  {
    if      (i >= a.size && i >= b.size) break;
    else if (i >= a.size) result = -2;
    else if (i >= b.size) result =  2;
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
        R_int diff = (R_int)item_a.value - (R_int)item_b.value;
        result = (diff < 0 ? -1 : !!diff);
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
        R_uint list_count = 0;
        for (R_uint i = 0; i < input.size;)
        {
          if (input.data[i] != '\r')
          {
            ++list_count;
            while (input.data[i] != '\r') ++i;
          }

          i += 2;
        }

        List* lists = calloc(list_count, sizeof(List));

        for (R_uint i = 0, j = 0; i < input.size;)
        {
          if (input.data[i] != '\r')
          {
            List* list = &lists[j++];

            R_String string = { .data = input.data + i, .size = 0 };
            while (input.data[i] != '\r') ++i, ++string.size;

            ParseList(string, list);
          }

          i += 2;
        }

        R_uint part1_result = 0;

        R_ASSERT(i % 2 == 0);
        for (R_uint i = 0; i < list_count; i += 2)
        {
          part1_result += (CompareLists(lists[i], lists[i+1]) < 1 ? i/2 + 1 : 0);
        }

        printf("Part 1: %llu\n", part1_result);

        List first_divider;
        ParseList(R_STRING("[[2]]"), &first_divider);

        List second_divider;
        ParseList(R_STRING("[[6]]"), &second_divider);

        R_uint first_pos  = 0;
        R_uint second_pos = 0;
        for (R_uint i = 0; i < list_count; ++i)
        {
          first_pos  += (CompareLists(first_divider,  lists[i]) > 0);
          second_pos += (CompareLists(second_divider, lists[i]) > 0);
        }

        // NOTE: Account for eachother
        first_pos  += (first_pos > second_pos);
        second_pos += (first_pos < second_pos);

        // NOTE: Account for 1 indexing
        first_pos  += 1;
        second_pos += 1;

        printf("Part 2: %llu\n", first_pos*second_pos);
      }

      fclose(input_file);
    }
  }

  return 0;
}
