# ns-3 uses murmur3 as the Hash function in packet scheduling
# This returns a 32-bit unsigned integer

import random

# number of values to generate
num_val = 8000

# generate integers in the range [0, 2^32 - 1]
max_val = 2**32 - 1
min_val = 0

f = open('hashvalues.txt', 'w')

for _ in range(num_val):
    f.write(str(random.randint(min_val, max_val)) + "\n")

print('Done')