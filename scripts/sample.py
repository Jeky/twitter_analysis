#!/usr/bin/python
from random import sample
import sys
import os
import shutil

def sample(prefix, size):
    with open(prefix + '-ids.txt') as fin:
        ids = [l.strip() for l in fin.readlines()]

    sampledIds = sample(ids, size)
    os.mkdir(prefix + '-sample-tweets')
    with open(prefix + '-sampled-ids.txt', 'w') as fout:
        for uid in sampledIds:
            fout.write(uid + '\n')
            copyfile.copyfile(prefix + '-tweets/' + uid, prefix + '-sampled-tweets/' + uid)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print 'USAGE: ./sample.py suspended|non-suspended SIZE'
    else:
        sample(sys.argv[1], int(sys.argv[2]))