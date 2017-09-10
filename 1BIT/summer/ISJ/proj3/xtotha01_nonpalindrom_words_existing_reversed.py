#!/usr/bin/env python3

import fileinput

palindroms = set()
words = set()
for line in fileinput.input():
    line = line.rstrip()
    words.add(line)
    if line == line[::-1]:
        palindroms.add(line)

print(sorted(w for w in words if w not in palindroms and w[::-1] in words))
