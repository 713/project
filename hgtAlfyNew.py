import os
import sys
import string

def from_directory_alfy(path):
    filenames = []
    for root,dirs,files in os.walk(path):
        filenames = files
    filenames2 = []
    for f in filenames:
        if (".fas" in f) and ("concat" in f) and ".sslist" not in f:
            filenames2.append(f)
    return filenames2

#####################################
#  Code for generate the Kr matrix
#  then return strains' closest mates
#####################################

def run_kr(fasta_file,kr_path):

    #the fasta_file should contain the whole genome of all strains

    os.chdir(kr_path)
    os.system("make")
    os.system("./kr64 %s > kr_matrix"%fasta_file)
    f = open("kr_matrix")
    listSeq = []
    listValue = []
    n = 0
    for line in f.read().splitlines():
        listTemp = []
        if len(line) <= 2:
    	    n = int(line)
        else:
    	    split_line = line.split(' ')
            listSeq.append(split_line[0])
            for j in range(len(split_line)-1):
                #print len(split_line[j])
                if len(split_line[j+1]) != 0:
                    listTemp.append(float(split_line[j+1]))
            listValue.append(listTemp)
            #print listTemp
    #print listValue
    dict = {}
    for i in range(n):
	    for j in range(n):
		    dict[(listSeq[i],listSeq[j])] = listValue[i][j]
    #print dict
    Dict = {}
    for i in range(n):
        for j in range(n):
            if((dict[(listSeq[i],listSeq[j])] > 0) ) and (dict[(listSeq[i],listSeq[j])] < 0.004):  #threshold: 0.004
                if listSeq[i] in Dict:
                    Dict[listSeq[i]].append(listSeq[j])
                else:
                    Dict[listSeq[i]] = []
                    Dict[listSeq[i]].append(listSeq[j])
    #print listSeq
    #print len(Dict)
    print Dict
    return Dict
##################################
# End of run_kr
##################################

def find_HGT(fasta_path,dirpath,alfy_location,output_folder):
    ##############################################################################
    #please change the corresponding path or add them to the arguments of find_HGT
    ##############################################################################
    os.system("dd if=temp2.fasta of=allSeq.fasta conv=ucase")
    Dict = run_kr(dirpath+"/allSeq.fasta",dirpath+"/dependencies/Kr_2.0.2/Src/Kr")
    #print Dict
    print("KR execution compelete, moving to Alphy\n")
    HGT_dict = dict()
    all_seqs = from_directory_alfy(fasta_path)
    print all_seqs
    filenames = list()
    for seq in all_seqs:
        #input = open(fasta_path+"/"+seq,"r")
        #output = open(fasta_path+"/"+seq,"w")
        #for line in fd:
         #   if line.startswith(">"):
          #      templine = line[10]
           #     #line.replace(line,"")
            #    fd.write(templine)

        #close(fd)
        os.system("awk -f %s/Scripts/cleanSeq.awk %s > %s_clean.fasta"
                %(alfy_location,fasta_path+"/"+seq,fasta_path+"/"+seq[0:seq.index(".")]))
    for root,dirs,files in os.walk(fasta_path):
        filenames = files
    f2 = []
    for f in filenames:
        if ("_clean.fasta" in f):
            f2.append(f)
    #print f2
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

        result = "%s/%s.alfy"%(output_folder,seq[0:seq.index(".")-6])
        f = open(result)

        g = f.read().splitlines()
        for line in g:
            if line.startswith(">"):
                tempname = line[1:]
                if len(tempname) >= 10:
                    name = tempname[0:10]
                else:
                    name = tempname
                print name
                continue
            splt_line = line.split(' ')
            start = splt_line[0]
            end = splt_line[1]
            #for j in range(len(splt_line)-4):
            name2temp = splt_line[3]
            if len(name2temp) >= 10:
                name2 = name2temp[0:10]
            else:
                name2 = name2temp
                #continue

            print name2, "This is from Alfy output"
            if name2 == "nh":
                continue
            else:
                if name in Dict:
                    print name, "THis is from Kr matrix"
                    if name2 in Dict[name]:
                        continue
                    else:
                        if (name,name2) in HGT_dict:
                            HGT_dict[(name,name2)].append((start,end))
                        else:
                            HGT_dict[(name,name2)] = []
                            HGT_dict[(name,name2)].append((start,end))
        f.close
    os.chdir(fasta_path)
    os.system("rm *_clean.fasta")
    os.chdir(dirpath)
    print HGT_dict
    return HGT_dict

#dirpath = "/home/prateek/Dropbox/Semester_2/Project'
#dirpath = os.getcwd()
#find_HGT(dirpath+"/seq12", dirpath,
#         dirpath+"/dependencies/Alfy_1.5",dirpath+"/test")
#find_HGT(dirpath, dirpath+"/dependencies/Alfy_1.5",dirpath+"/output")
#find_HGT("/afs/andrew.cmu.edu/usr24/yimingx/private/03713/seq12",
#         "/afs/andrew.cmu.edu/usr24/yimingx/private/03713/Alfy_1.5",
#         "/afs/andrew.cmu.edu/usr24/yimingx/private/03713/test")
