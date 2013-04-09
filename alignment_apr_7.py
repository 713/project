import os
import sys

#Function to fetch the sequence for a given strain.
def get_sequence(filename, dirpath):
    sequence = ""
    if(os.path.isdir(dirpath)):
        filepath = "%s"%(filename)
        print filepath
        if(os.path.isfile(filepath)):
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

def mauve(dirpath, output,backbone,file_list):
    file_list2 = file_list
    for x in xrange(len(file_list2)):
        file_list2[x] = "%s/%s"%(dirpath, file_list[x])
    systemCall = "/Applications/Mauve.app/Contents/MacOS/progressiveMauve --output=%s --backbone-output=%s" %(output,backbone)
    for x in xrange(len(file_list2)):
        systemCall = "%s %s" %(systemCall,file_list2[x])
    print systemCall
    os.system(systemCall)

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
            assert(len(sequence) > end)
            if(start != 0 or end != 0):
                align_seq += sequence[start:end]
        count += 1
    bac_file.close()
    return align_seq

def aligned_sequence(dirpath, output, backbone, file_list):
    out_file = open("out_alignment.fa", "a")
    mauve(dirpath, output,backbone,file_list)
    print file_list
    for i in xrange(len(file_list)):
        seq = get_sequence(file_list[i], dirpath)
        concat_seq = get_concat_seq(seq, i, backbone)
        out_file.write(">seq%s\t%s\n" %(i,concat_seq))
    out_file.close()

aligned_sequence("/Users/relyanow/Dropbox/pipeline_output","/Users/relyanow/Dropbox/pipeline_output/out_apr7_2.txt","/Users/relyanow/Dropbox/pipeline_output/bb_apr7_2.txt", ["SP9BS68.fas","Sp3BS71.fasta"])

        



    

