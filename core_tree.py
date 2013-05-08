#!/usr/bin/env python

def make_tree(seq_type):
    if seq_type == "core":
        #Call the aligner to align the core genomes and produce an alignment file
        pass
    elif seq_type == "distributed":
        pass
        #Call the aligner to align the distributed genomes and produce an alignment file
    #call the command "phylip dnaml alignment_file"
    #produce the file as the name "intree"
    #call the command "phylip drawtree intree"
    #you get the tree drawn

def core_genome_tree(backbone,seq_num):
    positions = get_core_positions(backbone,seq_num)
    pos = []
    core_regions = []
    #Positions contains the core genomes in all the sequences
    #This loop is to organise into a list of lists.
    #Each list of lists contains the indices of the core_genomes of the indexed sequence.
    for start_index in xrange(0,12):
        for core_index in xrange(start_index,len(positions),12):
            pos.append(core_index)
        core_regions.append(pos)
        pos = []
    #core_regions[0] gives a list of indices for seq0
    #list_of_files = [seq0,seq1,seq2...]
    fh = open("core_genomes.fasta",a)
    filenum = 0
    for fil in list_of_files:
        cur_seq = []
        for index in core_regions[filenum]:
            cur_seq.append(get_seq(index,fil))
        seq = cur_seq.join('')
        #Add some annotation and form a fasta file  
        annotation = "> Coregenome of "+fil+"\n"
        fasta = annotation+seq+"\n"
        fh.write(fasta)
    make_tree("core")
    fh.close()

