#!/usr/bin/env python

import os
from Bio import SeqIO

def Convert_to_Fasta(filelist):
    for file in filelist:
        if file.endswith("gb") or file.endswith("gbk"):
            count = SeqIO.convert("/home/prateek/Downloads/project_data/"+file, "genbank", "/home/prateek/Downloads/project_data/"+file+".fasta", "fasta")
            print "Genbank file found - "+file
            #ct = ct+1
            print "Generating "+file+".fasta ..." + "Done!"

if __name__ == '__main__':
    filelist = [] #assign to be the list of files in the current folder
    for item in os.listdir("/home/prateek/Downloads/project_data/"):
        #print item
        filelist.append(item)
    Convert_to_Fasta(filelist)
    #makeReads(filelist)
    #CoreHGT()

