#!/usr/bin/env python

from __future__ import division
import math
import random
import string
import re

def norm(file):
    n = 0
    f = open(file)
    for line in f.readlines(): 
    	if line.startswith(">") :
            if n == 0:
                 print line.strip('\n')
            else:
                continue
        else:	
            #lineTemp = 		
            print (line.strip('\n')).strip('\n')
        n += 1

def main():
    import sys
    if len(sys.argv) >= 1:
      #  if sys.argv[1] == "heap": test_heap()
      #  if sys.argv[1] == "mst": test_mst()
      #  if sys.argv[1] == "kruskal": test_kruskal()
      #  print sys.argv
        norm(sys.argv[1])
    else:
        print "Usage: mstprim.py [heap|mst|kruskal]"

if __name__ == "__main__": main()
