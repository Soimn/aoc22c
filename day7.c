#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct File
{
  struct File* next;
  char* name;
  uint32_t size;
} File;

typedef struct Directory
{
  struct Directory* next;
  struct Directory* parent;
  char* name;
  File* files;
  struct Directory* subdirs;
  uint64_t size;
} Directory;

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

        while (input[i] != '\n') ++i;
        ++i;

        Directory root_dir = {
          .next    = 0,
          .parent  = 0,
          .name    = "/",
          .files   = 0,
          .subdirs = 0,
        };

        Directory* current_dir = &root_dir;

        for (;;)
        {
          if (i >= input_file_size) break;

          i += 2; // skip $\s
          
          if (input[i] == 'c')
          {
            i += 3;

            char* dir_name = input + i;

            while (input[i] != '\r') ++i;
            input[i] = 0;
            i += 2;

            if (strcmp(dir_name, "..") == 0) current_dir = (current_dir->parent ? current_dir->parent : current_dir);
            else
            {
              Directory* scan = current_dir->subdirs;
              for (; scan != 0; scan = scan->next)
              {
                if (strcmp(scan->name, dir_name) == 0) break;
              }

              current_dir = scan;
            }
          }
          else
          {
            i += 4;

            if (current_dir->files == 0 && current_dir->subdirs == 0)
            {
              File** next_file     = &current_dir->files;
              Directory** next_dir = &current_dir->subdirs;

              for (;;)
              {
                if (i >= input_file_size || input[i] == '$') break;

                if (input[i] == 'd')
                {
                  i += 4;

                  char* dir_name = input + i;
                  while (input[i] != '\r') ++i;
                  input[i] = 0;
                  i += 2;

                  *next_dir = malloc(sizeof(Directory));
                  **next_dir = (Directory){
                    .next    = 0,
                    .parent  = current_dir,
                    .name    = dir_name,
                    .files   = 0,
                    .subdirs = 0,
                    .size    = 0,
                  };

                  next_dir = &(*next_dir)->next;
                }
                else
                {
                  uint32_t size = 0;
                  while ((unsigned char)(input[i] - 0x30) < 10u) size = size*10 + (input[i++] & 0xF);

                  ++i; // skip \s

                  char* file_name = input + i;

                  while (input[i] != '\r') ++i;
                  input[i] = 0;
                  i += 2;

                  *next_file = malloc(sizeof(File));
                  **next_file = (File){
                    .next     = 0,
                    .name     = file_name,
                    .size     = size,
                  };

                  next_file = &(*next_file)->next;
                }
              }
            }
          }
        }

        unsigned int level = 0;

#if 0
        for (Directory* scan = &root_dir; scan != 0; )
        {
          for (unsigned int i = 0; i < level; ++i) printf("  ");
          printf("- %s (dir)\n", scan->name);

          if (scan->subdirs != 0)
          {
            scan = scan->subdirs;
            level += 1;
          }
          else
          {
            for (File* file_scan = scan->files; file_scan != 0; file_scan = file_scan->next)
            {
              for (unsigned int i = 0; i < level + 1; ++i) printf("  ");
              printf("- %s (file, size=%lu)\n", file_scan->name, file_scan->size);

              scan->size += file_scan->size;
            }

            if (scan->next != 0) scan = scan->next;
            else
            {
              for (;;)
              {
                for (File* file_scan = scan->parent->files; file_scan != 0; file_scan = file_scan->next)
                {
                  for (unsigned int i = 0; i < level; ++i) printf("  ");
                  printf("- %s (file, size=%lu)\n", file_scan->name, file_scan->size);

                  scan->parent->size += file_scan->size;
                }

                for (Directory* dir_scan = scan->parent->subdirs; dir_scan != 0; dir_scan = dir_scan->next) scan->parent->size += dir_scan->size;

                scan = scan->parent;
                level -= 1;

                if (scan->parent == 0)
                {
                  scan = 0;
                  break;
                }
                else if (scan->next == 0) continue;
                else
                {
                  scan = scan->next;
                  break;
                }
              }
            }
          }
        }
#else
        for (Directory* scan = &root_dir; scan != 0; )
        {
          if (scan != &root_dir)
          {
            for (unsigned int i = 0; i < level-1; ++i) printf("    ");
            printf("%s\n", scan->name);
          }

          if (scan->subdirs != 0)
          {
            scan = scan->subdirs;
            level += 1;
          }
          else
          {
            for (File* file_scan = scan->files; file_scan != 0; file_scan = file_scan->next)
            {
              for (unsigned int i = 0; i < level-1 + 1; ++i) printf("    ");
              printf("%lu\n", file_scan->size);

              scan->size += file_scan->size;
            }

            if (scan->next != 0) scan = scan->next;
            else
            {
              for (;;)
              {
                for (File* file_scan = scan->parent->files; file_scan != 0; file_scan = file_scan->next)
                {
                  for (unsigned int i = 0; i < level-1; ++i) printf("    ");
                  printf("%lu\n", file_scan->size);

                  scan->parent->size += file_scan->size;
                }

                for (Directory* dir_scan = scan->parent->subdirs; dir_scan != 0; dir_scan = dir_scan->next) scan->parent->size += dir_scan->size;

                scan = scan->parent;
                level -= 1;

                if (scan->parent == 0)
                {
                  scan = 0;
                  break;
                }
                else if (scan->next == 0) continue;
                else
                {
                  scan = scan->next;
                  break;
                }
              }
            }
          }
        }
#endif

        printf("%llu\n", root_dir.size);
        int64_t free_space    = 70000000 - (int64_t)root_dir.size;
        int64_t space_to_free = 30000000 - free_space;

        uint64_t smallest_dir_to_delete_size = root_dir.size;
        char* smallest_dir_to_delete_name    = root_dir.name;

        printf("[%lld, %lld]\n", free_space, space_to_free);

        uint64_t part1_result = 0;
        for (Directory* scan = &root_dir; scan != 0;)
        {
          if (scan->size < 100000) part1_result += scan->size;
          if (scan->size >= space_to_free && scan->size < smallest_dir_to_delete_size)
          {
            smallest_dir_to_delete_size = scan->size;
            smallest_dir_to_delete_name = scan->name;
          }

          if      (scan->subdirs != 0) scan = scan->subdirs;
          else if (scan->next    != 0) scan = scan->next;
          else
          {
            for (;;)
            {
              scan = scan->parent;
              if      (scan       == 0) break;
              else if (scan->next == 0) continue;
              {
                scan = scan->next;
                break;
              }
            }
          }
        }

        printf("Part 1: %llu\n", part1_result);
        printf("Part 2: %lld\n", smallest_dir_to_delete_size);
      }

      fclose(input_file);
    }
  }

  return 0;
}
