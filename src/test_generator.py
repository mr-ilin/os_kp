#!/usr/bin/env python3

# -*- coding: utf8 -*-

import datetime

import sys
import random
import string

MAX_VALUE_LEN = 18446744073709551614

def generate_random_value():
    return random.randint(0, MAX_VALUE_LEN)

def generate_random_key_0():
    return "".join([str(random.randint(1, 31))])

def generate_random_key_1():
    return "".join([str(random.randint(1, 12))])

def generate_random_key_2():
    return "".join([str(random.randint(1700, 3100))])

def main():
    if len(sys.argv) < 3:
        print("Usage: {} <output file> <words count>".format(sys.argv[0]))
        sys.exit(1)

    output_file = sys.argv[1]
    words_cnt = int(sys.argv[2])
    new_line_cnt = 1
    with open( output_file, 'w') as foutput:
        for num in range( words_cnt ):
            foutput.write("{}".format(num))
            if num / 100 == new_line_cnt:
                foutput.write("\n")
                new_line_cnt = new_line_cnt + 1
            

if __name__ == "__main__":
    main()
