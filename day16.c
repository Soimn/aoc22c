#include <stdlib.h>
#include <stdio.h>

#include "rin.h"

R_u16* Cache = 0;

typedef struct Valve
{
	R_u8 rate;
	R_u8 index;
	R_u64 tunnels;
} Valve;

Valve Valves[54] = {0};

R_u16
EvalRoutes(R_u8 steps_left, R_u8 position, R_u16 valve_mask)
{
	R_uint cache_index = (valve_mask&0x7FFF) | ((R_uint)(position&0x3F) << 15) | ((R_uint)(steps_left&0x1F) << 21);

	if (Cache[cache_index]&0x8000) return Cache[cache_index];
	else
	{
		R_u16 result = 0;

		Valve* valve = &Valves[position];
		
		for (R_u64 tunnels = valve->tunnels; tunnels != 0;)
		{
			R_u32* tunnel;
			_BitScanForward64(&tunnel, tunnels);
			tunnels ^= (1ULL << tunnel);

			R_u16 sub_result = EvalRoutes(steps_left-1, tunnel, valve_mask);
			if (sub_result > result) result = sub_result;
		}

		if (steps_left > 2 && valve->rate != 0 && ((1ULL << valve->index)&valve_mask) == 0)
		{
			R_u16 sub_valve_mask = valve_mask | (1ULL << valve->index);
		
			for (R_u64 tunnels = valve->tunnels; tunnels != 0;)
			{
				R_u32* tunnel;
				_BitScanForward64(&tunnel, tunnels);
				tunnels ^= (1ULL << tunnel);

				R_u16 sub_result = EvalRoutes(steps_left-2, tunnel, sub_valve_mask);
				sub_result += valve->rate*(steps_left-1);
				if (sub_result > result) result = sub_result;
			}
		}

		R_ASSERT((result&0x8000) == 0)
		Cahce[cache_index] = 0x8000 | result;

		return result;
	}
}

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
			// NOTE: Assumes there are 15 or less non-zero valves with no more than 64 valves in total, and total pressure cannot exceed 32K
			Cache = calloc((1ULL << (5 + 6 + 15)), sizeof(R_u16));
			R_ASSERT(Cache != 0);
    }

    fclose(input_file);
  }

  return 0;
}
