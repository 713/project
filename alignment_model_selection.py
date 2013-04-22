#!/usr/bin/env python

import os
import sys
#from subprocess import Popen,PIPE,STDOUT,call
#import cStringIO


###############################################################################
# Fields for user to change.
###############################################################################
data_dirpath   = "/Users/lleung/713project/data_sample"

data_backbone  = "/Users/lleung/713project/data_sample/backbone.txt"
#data_file_list = ["seq0.txt", "seq1.txt"]
data_file_list = ["HIV_vpu.ref2.fas"]   #testing jModelTest

data_output    = "/Users/lleung/713project/output_mauve.txt"
location_mauve = "/Applications/Mauve.app/Contents/MacOS/progressiveMauve"
location_jModelTest = "/Users/lleung/713project/jmodeltest-2.1.3/jModelTest.jar"
output_model = "/Users/lleung/713project/output_model_selection.txt"

###############################################################################
# ## Prateek's
#
#data_backbone = data_dirpath+"/backbone.txt"
#data_output = data_dirpath+"/output_mauve.txt"
'''Should we hardcode the things as MacOS?'''
# (LL) Best not to.
#      Maybe have the user select the path and/or when installing our script,
#      the settings somehow specify where the paths/dirpath/location_, etc are.
###############################################################################
# ## Rebecca's variables
#
# data_dirpath   = "/Users/relyanow/Dropbox/pipeline_output"
# data_backbone  = "/Users/relyanow/Dropbox/pipeline_output/bb_apr7_2.txt"
# data_file_list = ["SP9BS68.fas","Sp3BS71.fasta"]
# data_output    = "/Users/relyanow/Dropbox/pipeline_output/out_apr7_2.txt"
# 
# location_mauve = "/Applications/Mauve.app/Contents/MacOS/progressiveMauve"
#
###############################################################################
# ## Stuti's variables
#
# data_dirpath   = "/afs/andrew.cmu.edu/usr23/stutia/private/03713"
# data_backbone  = "/afs/andrew.cmu.edu/usr23/stutia/private/03713/backbone.txt"
# data_file_list = ["seq0.txt", "seq1.txt"]
# 
###############################################################################
### User do not have to edit anything below. ###
###############################################################################
#### For joining the lines the following line is faster.
#### sequence = ''.join(file.read().splitlines())
# Function to fetch the sequence for a given strain.
def get_sequence(filename, dirpath):
    sequence = ""
    if(os.path.isdir(dirpath)):
        filepath = "%s"%(filename) 
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

def mauve(dirpath, output, backbone, file_list, location_mauve):
    file_list2 = file_list
    for x in xrange(len(file_list2)):
        file_list2[x] = "%s/%s"%(dirpath, file_list[x])
    # Additional option flags will be in a config file later.
    systemCall = "%s --output=%s --backbone-output=%s" %(location_mauve, output, backbone)
    for x in xrange(len(file_list2)):
        systemCall = "%s %s" %(systemCall, file_list2[x])
    print systemCall
    os.system(systemCall)

def model_selection(dirpath, file_list, output_filename, location_jModelTest):
    print("Model Selection in progress")
    # Additional option flags will be in a config file later.
    file_list2 = file_list
    for x in xrange(len(file_list2)):
        file_list2[x] = "%s"%(file_list[x])
    systemCallHead = "java -jar %s" %(location_jModelTest)
    systemCallTail = " -g 4 -f -AIC -BIC -a -S BEST -o %s" %(output_filename)
    # Flags as config file later.
    for x in xrange(len(file_list2)):
        systemCallHead = "%s -d %s" %(systemCallHead, file_list2[x])
    systemCall = systemCallHead + systemCallTail
    print systemCall
    os.system(systemCall)
#    output = cStringIO.StringIO()
#    f=open(output, 'w')
#1    print(os.system(systemCall)) >> f  #try to write data
#2    f.write(os.popen(systemCall).readline()) #writes only one line then stops
#3    temp=subprocess.Popen(systemCall, shell=True, stdout=subprocess.PIPE).stdin #doesn't work
#4    temp=subprocess.call([systemCall])    #broke subprocess library
#5    proc=Popen(systemCall, shell=True, stdout=PIPE, )
#5    output.write(proc.communicate()[0])
#5    output.close()
#    print output >> f
#    f.close()

def get_concat_seq(sequence, seq_num, backbone):
    bac_file = open("%s"%backbone, "r")
    align_seq = ""
    count = 0
    for line in bac_file:
        if(count > 0): #Ignore the first line which
            #contains the sequence name.
            line = line.split()
            assert(len(line) > 2*(seq_num))
            #Fetch the sequence positions from the backbone file.
            start = int(line[2*seq_num])
            end = int(line[(2*seq_num)+1])
            #Slice the sequence in those positions.
            #print(sequence)
            assert(len(sequence) > end)
            if(start != 0 or end != 0):
                align_seq += sequence[start:end]
        count += 1
    bac_file.close()
    return align_seq

def aligned_sequence(dirpath, output, backbone, file_list, location_mauve, location_jModelTest, output_model):
    out_file = open("out_alignment.fa", "a")
    # Alignment with Mauve
    mauve(dirpath, output, backbone, file_list, location_mauve)
    # Model Selection with JModelTest
    model_selection(dirpath, file_list, output_model, location_jModelTest)
    for i in xrange(len(file_list)):
        seq = get_sequence(file_list[i], dirpath)
        concat_seq = get_concat_seq(seq, i, backbone)
        out_file.write(">seq%s\t%s\n" %(i, concat_seq))
    out_file.close()

aligned_sequence(data_dirpath, data_output, data_backbone, data_file_list, location_mauve, location_jModelTest, output_model)

# Testing
# model_selection(data_dirpath, data_file_list, output_model, location_jModelTest)

