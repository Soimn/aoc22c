package main;

import "core:fmt";
import "core:strings";
import "core:strconv";
import "core:os";

main :: proc()
{
  if len(os.args) != 2 do fmt.println("Invalid number of arguments. Expected: day1 <input_file>");
  else
  {
    input, ok := os.read_entire_file(os.args[1]);
    if !ok do fmt.println("Failed to read input file");
    else
    {
      parse_blocks: {
        max_cals: [3]int;

        blocks := strings.split(string(input), "\r\n\r\n");

        for block in blocks
        {
          acc_cal := 0;
          for line in strings.split_lines(block)
          {
            if len(line) == 0 do break;

            cal, cal_ok := strconv.parse_int(line);
            if !cal_ok
            {
              fmt.println("Invalid input format");
              break parse_blocks
            }
            else do acc_cal += cal;
          }

          for i in 0..<len(max_cals)
          {
            if max_cals[i] < acc_cal
            {
              max_cals[i], acc_cal = acc_cal, max_cals[i];
            }
          }
        }

        fmt.printf("Part 1: %d\n", max_cals[0]);
        fmt.printf("Part 2: %d\n", max_cals[0] + max_cals[1] + max_cals[2]);
      }
    }
  }
}
