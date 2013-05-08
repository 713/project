#!/usr/bin/env python

import os
import sys
#Function to fetch the sequence for a given strain.
def get_sequence(filename, dirpath):
    sequence = ""
    if(os.path.isdir(dirpath)):
        filepath = "%s/%s"%(dirpath, filename)
        if(os.path.isfile("%s" %(filepath))):
                file = open("%s" %filepath)
                #Read the sequence in a string.
                for line in file:
                    sequence += line.rstrip()
                file.close()
        else:
            print("Not a valid file")
            return
    else:
        print("Not a valid directory")
        return 
    return sequence

#Returns the start position of a sequence
def get_start_pos(seq_num):
    return 2 * seq_num

#Returns the end position of a sequence
def get_end_pos(seq_num):
    return 2 * seq_num + 1

#Checks whether the region is a core genome
def is_core_genome(positions):
    for i in xrange(0,len(positions), 2):
        #In the entire region, the start and end positions
        #for any one organism should be non-zero 
        if(positions[i] == "0" and positions[i+1] == "0"):
            return False
    return True

#Returns the concatenated sequence of the core genome.
def get_concat_seq(sequence, seq_num, backbone):
    bac_file = open("%s"%backbone, "r")
    align_seq = ""
    count = 0
    for line in bac_file:
        if(count > 0): #Ignore the first line which
            #contains the sequence name.
            line = line.split()
            if(is_core_genome(line)):
                assert(len(line) > get_start_pos(seq_num))
                #Fetch the sequence positions from the backbone file.
                start = int(line[get_start_pos(seq_num)])
                end = int(line[get_end_pos(seq_num)])
                #Switch the start and end positions in case of inversions
                if(start > end):
                    start, end = end, start
                #Slice the sequence in those positions.
                assert(len(sequence) > end)
                if(start != 0 or end != 0):
                    align_seq += sequence[start:end]
        count += 1
    bac_file.close()
    return align_seq

#Gets the positions in the sequence which are there in the core
#genome
def get_core_positions(backbone, seq_num):
    assert(os.path.isfile(backbone))
    bac_file = open("%s" %backbone)
    count = 0
    pos = list()
    for line in bac_file:
        if(count > 0): #Ignore the header
            line = line.split()
            if(is_core_genome(line)):
                start = int(line[get_start_pos(seq_num)])
                end = int(line[get_end_pos(seq_num)])
                pos.append((start,end))
        count += 1
    bac_file.close()
    return pos

#Function to get the distributed genome
def rm_core_genome(sequence, seq_num, backbone):
    #Get all positions which are there in the core
    positions = get_core_positions(backbone, seq_num)
    distr_seq = ""
    #For each nucleotide, check whether it falls in any of the
    #core genome regions for the sequence
    for i in xrange(len(sequence)):
        nucl_in_core = False
        for (start, end) in positions:
            if(start <= i and  i < end):
                print sequence[i], i
                nucl_in_core = True
                break
        #If the nucleotide is not there in the core genome, 
        #append it to the distributed.
        if(nucl_in_core == False):
            distr_seq += sequence[i]
    return distr_seq

def aligned_sequence(dirpath, backbone, file_list):
    out_file = open("out_alignment.fa", "a")
    distr_file = open("out_distr.fa", "a")
    for i in xrange(len(file_list)):
        seq = get_sequence(file_list[i], dirpath)
        concat_seq = get_concat_seq(seq, i, backbone)
        out_file.write(">seq%s\n%s\n" %(i,concat_seq))
        distr_seq = rm_core_genome(seq, i, backbone)
        distr_file.write(">seq%s\n%s\n" %(i, distr_seq))
    out_file.close()
    distr_file.close()
aligned_sequence("/afs/andrew.cmu.edu/usr23/stutia/private/03713",
    "/afs/andrew.cmu.edu/usr23/stutia/private/03713/backbone.txt", ["seq0.txt",
        "seq1.txt"])

#def get_region(dirpath, backbone, file_list):
#    file = open("%s" %backbone, "r")
#    for line in file:
#        if(count > 0):
#            line = line.split()
#            for i in xrange(0,len(line), 2):
#                file_num = i/2 
#                reg_file = open("%s" %(
#                seq = get_sequence(dirpath, file_list[i])
#                start = int(line[i])
#                end = int(line[i+1])
#                assert(start < end)
#                file.write(">%s\n" %(seq[start:end])

