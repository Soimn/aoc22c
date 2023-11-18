import numpy as np

def compare_order(left, right):
    if type(left) == int and type(right) == int:
        return np.sign(right-left)
    else:
        if type(left) == int:
            left = [left]
        elif type(right) == int:
            right = [right]

        for i in range(max(len(left), len(right))):
            if  i >= len(left):
                return 1
            elif i >= len(right):
                return -1
            else:
                sub_result = compare_order(left[i], right[i])
                if sub_result != 0: return sub_result
                else:               continue
        return 0
                
#print(compare_order([1,1,3,1,1], [1,1,5,1,1]))
#print(compare_order([[1],[2,3,4]], [[1],4]))
#print(compare_order([1,[2,[3,[4,[5,6,7]]]],8,9], [1,[2,[3,[4,[5,6,0]]]],8,9]))

input_file = open("day13_input.txt", "r")
input_string = input_file.read()
input_file.close()

result = 0
for i, pair in enumerate([str_pair.split() for str_pair in input_string.split("\n\n")]):
    exec("left = " + pair[0])
    exec("right = " + pair[1])
    result += i+1 if compare_order(left, right) == 1 else 0

print("Part 1:", result)

packets = [[[2]], [[6]]]
for i, pair in enumerate([str_pair.split() for str_pair in input_string.split("\n\n")]):
    exec("packets.append(" + pair[0] + ")")
    exec("packets.append(" + pair[1] + ")")

from functools import cmp_to_key
packets = sorted(packets, reverse=True, key=cmp_to_key(compare_order))

print("Part 2:", (packets.index([[2]])+1)*(packets.index([[6]])+1))
