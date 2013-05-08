#!/usr/bin/env python

import sys
import os

#Function to check if the file is valid
def is_valid_file(filename):
    if(os.path.isfile(filename)):
        return True
    return False

#Function to check if the directory is valid
def is_valid_dir(dirpath):
    if(os.path.isdir(dirpath)):
        return True
    return False

#Function to get the path of a file
def get_filepath(dirpath, filename):
    if(is_valid_dir(dirpath)):
        filepath = "%s/%s" %(dirpath, filename)
        if(is_valid_file(filepath)):
            return filepath
        else:
            sys.error("File not in directory")
    else:
        sys.error("Invalid path to file")
def get_contig_count(filename):
    count = 0
    file = open("%s" %filename)
    for line in file:
        if(line[0] == ">"):
            count += 1
    file.close()
    return count

#Function to concatenate the contigs                      
def get_concat_contigs(dirpath, filename):
    filepath = get_filepath(dirpath, filename)
    if(is_valid_file(filepath)):
        count_contig = get_contig_count(filepath)
        if(count_contig > 1):
            file_org = open("%s" %filepath, "r")
            file_new = open("%s/concat-%s" %(dirpath, filename), "a")
            file_new.write(">%s\n" %(filename))
            for line in file_org:
                if(line[0] != ">"):
                      line = line.rstrip()
                      file_new.write(line)
            file_org.close()
            file_new.close()
    else:
        sys.error("Invalid path")

