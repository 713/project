import os
import sys
import string

def from_directory(path):
    filenames = []
    for root,dirs,files in os.walk(path):
        filenames = files
    filenames2 = []
    for f in filenames:
        if (".fasta" in f) and ".sslist" not in f:
            filenames2.append(f)
    return filenames2

def run_kr(fasta_path,kr_path):
    #make and run kr to compute Kr
    #os.chdir(kr_path)
    #os.system("make")
    #os.system("./Kr %s > krmatrix"%fasta_path)
    f = open("krmatrix")
    listSeq = []
    listValue = []
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
            if((dict[(listSeq[i],listSeq[j])] > 0) ) and (dict[(listSeq[i],listSeq[j])] < 1):
                if listSeq[i] in Dict:
                    Dict[listSeq[i]].append(listSeq[j])
                else:
                    Dict[listSeq[i]] = []	
                    Dict[listSeq[i]].append(listSeq[j])					
    return Dict	
	
	
	
	
	
	
def find_HGT(fasta_path,alfy_location,output_folder):
    Dict = run_kr("a","b")
	
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

        result = "%s/%s.alfy"%(output_folder,seq[0:seq.index(".")-6])
        f = open(result)
        HGT_dict = dict()
        g = f.read().splitlines()
        for line in g:
            if line.startswith(">"):
                name = line[1:]
                continue
            splt_line = line.split(' ')
            start = splt_line[0]
            end = splt_line[1]
            name2 = splt_line[3]
            if (start,end) in HGT_dict or (end,start) in HGT_dict or name2 == "nh":
                continue
            else:   
                if name2 in Dict[name]:
                    continue
                else:					
                    HGT_dict[(start,end)] = (name,name2)
        f.close
    print HGT_dict
    return HGT_dict


find_HGT("/afs/andrew.cmu.edu/usr24/yimingx/private/03713/hiv42",
         "/afs/andrew.cmu.edu/usr24/yimingx/private/03713/Alfy_1.5",
         "testalfy")
