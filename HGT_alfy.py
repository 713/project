#!/usr/bin/env python

import os
import sys

def from_directory(path):
    filenames = []
    for root,dirs,files in os.walk(path):
        filenames = files
    filenames2 = []
    for f in filenames:
        if (".fasta" in f) and ".sslist" not in f:
            filenames2.append(f)
    return filenames2

def find_HGT(fasta_path,alfy_location,output_folder):
    #make alfy
    os.chdir(alfy_location)
    os.system("make")
    all_seqs = from_directory(fasta_path)
    os.chdir(fasta_path)
    for seq in all_seqs:
        os.system("awk -f %s/Scripts/cleanSeq.awk %s > %s_clean.fasta"
                %(alfy_location,seq,seq[0:seq.index(".")]))
    for root,dirs,files in os.walk(fasta_path):
        filenames = files
    f2 = []
    for f in filenames:
        if ("_clean.fasta" in f):
            f2.append(f)
    for seq in f2:
        #temporarily change file name                            
        ren_seq = seq
        rs = ren_seq[0:seq.index(".")]
        rs = rs+".temp"
        os.system("mv %s/%s %s/%s"%(fasta_path,seq,fasta_path,rs))
        #concatenate all files except for seq into single fasta
        os.system("cat %s/*clean.fasta > %s/merged.fasta"%(fasta_path,fasta_path))
        os.system("mv %s/%s %s/%s"%(fasta_path,rs,fasta_path,seq))
        #find HGTs for seq and store in output folder
        #default p-value = 0.4
        os.system("%s/alfy -i %s/%s -j %s/merged.fasta -M > %s/%s.alfy"
                  %(alfy_location,fasta_path,seq,fasta_path,
                    output_folder,seq[0:seq.index(".")-6]))
        os.system("rm %s/merged.fasta"%(fasta_path))


find_HGT("/Users/relyanow/713project/project/Alfy_1.5/data/hiv42",
         "/Users/relyanow/713project/project/Alfy_1.5",
         "/Users/relyanow/713project/project/test")

