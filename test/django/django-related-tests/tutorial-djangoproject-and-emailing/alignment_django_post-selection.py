#!/usr/bin/env python

import os
import sys
#from django.core import EmailMessage

# =============================================================================
# Fields for user to change.
# =============================================================================
# Luigi's variables
data_dirpath   = "/Users/lleung/713project/data_sample"

data_backbone  = "/Users/lleung/713project/data_sample/backbone.txt"
data_file_list = ["seq0.txt", "seq1.txt"]
data_file_list = ["HIV_vpu.ref2.fas"]   #testing jModelTest

data_output    = "/Users/lleung/713project/output_mauve.txt"
location_mauve = "/Applications/Mauve.app/Contents/MacOS/progressiveMauve"
location_jModelTest = "/Users/lleung/713project/jmodeltest-2.1.3/jModelTest.jar"
output_model = "/Users/lleung/713project/output_model_selection.txt"

# =============================================================================
## Prateek's variables
#data_backbone = data_dirpath+"/backbone.txt"
#data_output = data_dirpath+"/output_mauve.txt"
'''Should we hardcode the things as MacOS?'''
# =============================================================================
## Rebecca's variables
# data_dirpath   = "/Users/relyanow/Dropbox/pipeline_output"
# data_backbone  = "/Users/relyanow/Dropbox/pipeline_output/backbone"
# #data_file_path = ["SP9BS68.fas","Sp3BS71.fasta"]
# data_output    = "/Users/relyanow/Dropbox/pipeline_output/output"
#  
# location_mauve = "/Applications/Mauve.app/Contents/MacOS/progressiveMauve"
# location_jModelTest = "/Users/relyanow/Downloads/jmodeltest-2.1.3/jModelTest.jar"
# output_model = "/Users/relyanow/Dropbox/pipeline_output/outpu_model_selection.txt"
# 
# =============================================================================
# ## Stuti's variables
# data_dirpath   = "/afs/andrew.cmu.edu/usr23/stutia/private/03713"
# data_backbone  = "/afs/andrew.cmu.edu/usr23/stutia/private/03713/backbone.txt"
# data_file_list = ["seq0.txt", "seq1.txt"]
#
# =============================================================================
# User do not have to edit anything below.
# =============================================================================
#### For joining the lines the following line is faster.
#### sequence = ''.join(file.read().splitlines())

#get sequence files stored in specified directory
def from_directory(path):
    filenames = []
    for root,dirs,files in os.walk(path):
        filenames = files
    filenames2 = []
    for f in filenames:
        if (".gb" in f or ".gbk" in f or ".fas" in f or  ".fasta" in f) and ".sslist" not in f:
            filenames2.append(f)
    return filenames2

#Function to fetch the sequence for a given strain.
def get_sequence(filename, dirpath):
    sequence = ""
    if(os.path.isdir(dirpath)):
        filepath = "%s/%s"%(dirpath,filename)
        print filepath
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

def mauve(dirpath, output, backbone,location_mauve):
    file_list = from_directory(dirpath)
    file_list2 = []
    for x in xrange(len(file_list)):
        file_list2.append("%s/%s"%(dirpath, file_list[x]))
    # Additional option flags will be in a config file later.
    systemCall = "%s --output=%s --backbone-output=%s" %(location_mauve, output, backbone)
    for x in xrange(len(file_list)):
        systemCall = "%s %s" %(systemCall, file_list2[x])
    os.system(systemCall)

def model_selection(core_file, output, location_jModelTest):
    print("Model Selection in progress")
    # Additional option flags will be in a config file later.
    file_list = [core_file]
    systemCallHead = "java -jar %s" %(location_jModelTest)
    systemCallTail = " -g 4 -f -AIC -BIC -a -S BEST"    # Flags as config file later.
    # find a way to append the output to .txt
    for x in xrange(len(file_list)):
        systemCallHead = "%s -d %s" %(systemCallHead, file_list[x])
    systemCall = systemCallHead + systemCallTail
    print systemCall
    os.system(systemCall) 

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
                nucl_in_core = True
                break
        #If the nucleotide is not there in the core genome, 
        #append it to the distributed.
        if(nucl_in_core == False):
            distr_seq += sequence[i]
    return distr_seq

def aligned_sequence(dirpath, output, backbone, location_mauve, location_jModelTest, output_model):
    out_file = open("out_test.fasta", "a")
    distr_file = open("out_distr.fasta","a")
    file_list = from_directory(dirpath)
    # Alignment with Mauve
    mauve(dirpath, output, backbone, location_mauve)
    # Model Selection with JModelTest
 #   model_selection(dirpath, output_model, location_jModelTest)
    for i in xrange(len(file_list)):
        seq = get_sequence(file_list[i], dirpath)
        concat_seq = get_concat_seq(seq, i, backbone)
        out_file.write(">seq%s\n%s\n" %(i,concat_seq))
        distr_seq = rm_core_genome(seq, i, backbone)
        distr_file.write(">seq%s\n%s\n" %(i, distr_seq))
    out_file.close()
    distr_file.close()
#    model_selection("out_alignment.fasta",output_model,location_jModelTest)

# aligned_sequence(data_dirpath, data_output, data_backbone, location_mauve, location_jModelTest, output_model)

# Send email to user when process is done.
#
# Temporary global variables:
# =============================================================================
# Project's URL
b713_project_URL = "http://www.contrib.andrew.cmu.edu/~lleung/project713/"

# Ask user for email on webpage
user_email = "luigileung@gmail.com"
# =============================================================================

# Need to generate unique link.
# Using user's email address as unique id. Hash/checksum of it, if not working.
# Using email as unique link works.
user_job_link = b713_project_URL + b713user_email + ".html"

## Using Django to send email
#email_job_complete = EmailMessage("713 Project: Job Complete",
#        "Your submitted job is complete. Please go to %s for results." %user_job_link,
#        to=["%s" %user_email])
#email_job_complete.send()
