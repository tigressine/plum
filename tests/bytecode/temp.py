import random

with open("too_long.plc", "w") as f:
    for i in range(2000):
        for j in range(4):
            f.write(str(random.randrange(100)))
            if j < 3:
                f.write(" ")
        f.write("\n")
