#!/usr/bin/env python
import os
import sys
import shutil
import string
sys.path.insert(1,"../../biopython")
sys.path.insert(1,"../../ete2")
sys.path.insert(1,"../../networkx")
from Bio import SeqIO
import hgtAlfyNew as hka
import core_tree
import hgt_graph as hg
################################################
#Function to check if the file is valid. Requires the file name with the
#entire path.
def is_valid_file(filename):
    if(os.path.isfile(filename)):
        return True
    return False

#Function to check if the directory is valid. Requires the path to the directory
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
def get_filename(filepath):
    if(is_valid_file(filepath)):
        path = filepath.split("/")
        last_elem = len(path) - 1
        if(path[last_elem] == ""):
            return path[last_elem - 1]
        return path[last_elem]
def generate_dirpath(dirlist):
    path = ""
    for i in xrange(len(dirlist)):
        if(i == len(dirlist) - 1):
            path += "%s" %(dirlist[i])
        else:
            path += "%s/" %(dirlist[i])
    return path
def is_multi_contig(filename):
    fh = open(filename,"r")
    lines = fh.readlines()
    count = 0
    for line in lines:
        if line.startswith(">"):
            count = count + 1
        if count >= 2:  #Has multiple contigs
            return True
    return False

def get_multi_contig_files(files):
    list_of_files = []
    for cur_file in files:
        if cur_file.endswith("fas") or cur_file.endswith("fasta") and is_multi_contig(cur_file):
            list_of_files.append(cur_file)
    return list_of_files

def get_dirpath(filepath):
    if(is_valid_file(filepath)):
        path = filepath.split("/")
        last_elem = len(path) - 1
        if(path[last_elem] == ""):
            return generate_dirpath(path[:last_elem - 1])
        return generate_dirpath(path[:last_elem])
    else:
        sys.exit("Invalid filepath")
#Function to concatenate the contigs

def get_concat_contigs(filepath):
    if(is_valid_file(filepath)):
        filename = get_filename(filepath)
        dirpath = get_dirpath(filepath)
        print filename, dirpath
        if((filename.endswith(".fasta") or filename.endswith(".fas"))
            and filename not in ["allSeq.fasta", "out_core.fasta"]):
            file_org = open("%s" %filepath, "r")
            file_new = open("%s/concat-%s" %(dirpath, filename), "a")
            file_new.write(">%s\n" %(filename))
            for line in file_org:
                #if (line[0] == ">"):
                    #line.replace(" ","") ####################remove the space sign
                if(line != "" and line[0] != ">"):
                      line = line.rstrip()
                      file_new.write(line)
            file_org.close()
            file_new.close()
    else:
        sys.exit("Invalid path")

def get_best_model(filename):
    assert os.path.isfile(filename)
    file = open("%s" %filename)
    found_best_model = False
    models_list = ['HKY85','JC69','K80','F81','F84','TN93','GTR','custom']
    for line in file:
        if("::Best Models::") in line:
            found_best_model = True
        if(found_best_model == True):
            if("AIC" in line):
                line = line.split()
                if line[1].startswith("GTR"):
                    model = "GTR" #Model is present in the second column
                elif line[1].startswith("HKY") or line[1].startswith("SYM"):
                    model = "HKY85"
                elif line[1].startswith("JC"):
                    model = "JC69"
                elif line[1].startswith("K80"):
                    model = "K80"
                else:
                    model = line[1]
                if line[-2] == "N/A":
                    pInv = 0.0
                else:
                    pInv = float(line[-2]) #Prob. of invariant sites is the second last col.
                if line[-1] == "N/A":
                    gamma = 0.0
                else:
                    gamma = float(line[-1]) #Gamma is the last column.
                return (model, pInv, gamma)

def model_selection(core_file, output, location_jModelTest):
    print location_jModelTest, output, core_file
    os.system("java -jar %s -d %s -g 4 -f -AIC -BIC -a -S BEST > %s"%(location_jModelTest,core_file,output))
    print "executed"
    model = get_best_model(output)
    print model
    return model

#Get all the concatenated fasta files from the directory.
def from_directory(dirpath):
    assert(is_valid_dir(dirpath))
    filenames = []
    for filename in os.listdir(dirpath):
        if(filename.startswith("concat") and not filename.endswith("sslist")):
            filepath = "%s/%s" %(dirpath, filename)
            assert(is_valid_file(filepath))
            filenames.append(filepath)
    return filenames

#Function to fetch the sequence for a given strain.
def get_sequence(filepath):
    sequence = ""
    count = 0
    if(is_valid_file("%s" %(filepath))):
        file = open("%s" %filepath)
        #Read the sequence in a string.
        for line in file:
            if(count > 0):
                sequence += line.rstrip()
                #sequence += line
            count += 1
        file.close()
    else:
        sys.exit("Not a valid file", filepath)
    return sequence
"""First this function fetches all the sequence files in the directory
The command to run mauve is

    mauve (or progressiveMauve) --output=<alignment_filename>
    --backbone-output=<backbone_filename> <seqfile_1> <seqfile_2> ... <seqfile_n>

    This function produces the backbone file containing the sequence positions common in all species being aligned
    Also, this function produces the alignment file which is used for ******
"""
def mauve(dirpath, output, backbone,location_mauve):
    print location_mauve, output
    print "Running Mauve"
    file_list = from_directory(dirpath)
    # Additional option flags will be in a config file later.
    systemCall = ("%s --output=%s --backbone-output=%s"
                 %(location_mauve, output, backbone))
    for filename in file_list:
        systemCall = "%s %s" %(systemCall, filename)
    os.system(systemCall)

#Returns the start position of a sequence
def get_start_pos(seq_num):
    return 2 * seq_num

#Returns the end position of a sequence
def get_end_pos(seq_num):
    return 2 * seq_num + 1

#Checks whether the region is a core genome
def is_core_genome(positions, sequence, HGT_set):
    for i in xrange(0,len(positions), 2):
        #In the entire region, the start and end positions
        #for any one organism should be non-zero
        if(positions[i] == "" or positions[i+1] == ""):
            return False
        if(int(positions[i]) <= 0 or int(positions[i+1]) <= 0):
            return False
        if(int(positions[i+1]) >= len(sequence)):
           return False
        if((int(positions[i]), int(positions[i+1])) in HGT_set):
           return False
    return True

#Returns the concatenated sequence of the core genome.
def get_concat_seq(sequence, seq_num, backbone, HGT_set):
    bac_file = open("%s"%backbone, "r")
    align_seq = ""
    count = 0
    num_core = 0
    for line in bac_file:
        #Ignore the first line which
        #contains the sequence name.
        if(count > 0 and num_core < 10):
            line = line.split()
            if(is_core_genome(line, sequence, HGT_set)):
                #print len(line), line, get_start_pos(seq_num), seq_num
                if(len(line) > get_start_pos(seq_num)):
                #Fetch the sequence positions from the backbone file.
                  start = int(line[get_start_pos(seq_num)])
                  end = int(line[get_end_pos(seq_num)])
                  #Switch the start and end positions in case of inversions
                  if(start > end):
                      start, end = end, start
                  #Slice the sequence in those positions.
                  assert(len(sequence) >= end)
                  #if(start != 0 or end != 0):
                  #key = (start,end)
                  if(start != end):
                      align_seq += sequence[start:end].upper()
                      num_core += 1
        count += 1
    bac_file.close()
    return align_seq

def gb_to_fasta(dirpath):
    for filename in os.listdir(dirpath):
        if filename.endswith("gb") or filename.endswith("gbk"):
            outname = 'concat-'+filename+'.fas'
            count = SeqIO.convert(filename, "genbank", outname, "fasta")
            print "Genbank file found - "+filename
            print "Generating "+outname+" ...Done!"
            f = open(outname)
            lines = f.readlines()
            f.close()
            f = open(outname,"w")
            for line in lines:
                 if line.startswith(">"):
                      line = ">"+filename+"\n"
                      f.write(line)
                 else:
                      f.write(line)
            f.close()

def aligned_sequence(dirpath, output, backbone,
                     location_mauve, location_jModelTest, output_model):
    assert(is_valid_dir(dirpath))
    out_file = open("%s/out_core.fasta" %(dirpath), "a")
    multi_files = get_multi_contig_files(os.listdir(dirpath))
    #Concatenate all the contigs for each organism in the file.
    for filename in multi_files:
        get_concat_contigs("%s/%s" %(dirpath, filename))
    #Convert all gbk to fasta
    gb_to_fasta(dirpath)
    #Get the list of all the concatenated fasta sequence files
    #present in the directory
    file_list = from_directory(dirpath)
    print file_list
    #Alignment with Mauve
    mauve(dirpath, output, backbone, location_mauve)
    print "Alignment done!"
    #Iterating through the files in the directory
    # Variable i in each iteration represents the name of the fil
    query = "awk 'FNR==1{print \"\"}1' concat*.fas > temp2.fasta"
    os.system(query)
    os.system("cp concat*.fas %s/seq12/" %(dirpath))
    HGT_dict = dict()
    print("Alignment complete, extracting HGTs...")
    HGT_dict = hka.find_HGT(dirpath+"/seq12", dirpath,
         dirpath+"/dependencies/Alfy_1.5",dirpath+"/test")
    print("Found all HGTs, extracting core genome...\n")
    HGT_set =  set()
    for key in HGT_dict:
        for position in HGT_dict[key]:
            HGT_set.add(position)
    for i in xrange(len(file_list)):
        count_char = 0
        seq = get_sequence(file_list[i])
        concat_core_seq = get_concat_seq(seq, i, backbone, HGT_set)
        out_file.write(">seq%s\n" %i)
        for j in xrange(len(concat_core_seq)):
          if(concat_core_seq[j] != "\n" and count_char < 60):
            out_file.write("%s" %(concat_core_seq[j].upper()))
            count_char += 1
          else:
            out_file.write("\n")
            count_char = 0
        out_file.write("\n")
    out_file.close()
    print("Core genome extracted, aligning with Muscle\n")
    """After this we get a file by the name out_core.fasta"""
    """This out_core.fasta contains the sequences belonging
    to the core genomes and which have not gone through HGT """
    core_tree.muscle(dirpath+"/out_core.fasta")
    """After this we get an alignment file by the name aligned.fasta"""
    print "Moving to model selection"
    # Model Selection with JModelTest
    (mod,pinv,gamma) = model_selection("aligned.fasta",output_model,location_jModelTest)
    core_tree.make_core_tree("aligned.fasta",gamma, mod,pinv)
    core_tree.tree_image(dirpath+"/aligned.fasta_align_phylip_phyml_tree.txt")
    hg.hgt_graph(HGT_dict)

if __name__ == "__main__":
    data_dirpath   = os.getcwd()
    cwd = data_dirpath
    data_backbone  = cwd+"/pipeline_output/backbone"
    data_output    = cwd+"/pipeline_output/output"
    location_mauve = cwd+"/dependencies/mauve_2.3.1/progressiveMauve"
    location_jModelTest = cwd+"/dependencies/jmodeltest-2.1.3/jModelTest.jar"
    output_model = cwd+"/pipeline_output/output_model_selection.txt"
    aligned_sequence(data_dirpath, data_output, data_backbone, location_mauve, location_jModelTest, output_model)
