import os

# Fields for user to change.
data_dirpath = "./test_data_dir"
data_backbone = "./test_data_dir/test-alignment_backbone.txt"
data_file_list = ["./test_data_dir/seq0.txt", "./test_data_dir/seq1.txt"]

### User do not have to edit anything below. ###

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

def aligned_sequence(dirpath, backbone, file_list):
    out_file = open("out_alignment.fa", "a")
    for i in xrange(len(file_list)):
        seq = get_sequence(file_list[i], dirpath)
        concat_seq = get_concat_seq(seq, i, backbone)
        out_file.write(">seq%s\t%s\n" %(i,concat_seq))
    out_file.close()

aligned_sequence(data_dirpath, data_backbone, data_file_list)

