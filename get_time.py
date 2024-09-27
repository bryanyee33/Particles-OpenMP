import sys

with open(sys.argv[1], "r") as file:
    lst = file.readlines()
    for line in lst:
        if "seconds time elapsed" in line:
            print(line[:line.index("+-")].strip())