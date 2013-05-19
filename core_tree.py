import sys
sys.path.insert(1,"../../biopython")
from Bio.Align.Applications import MuscleCommandline
from Bio.Phylo.Applications import PhymlCommandline
from Bio import AlignIO
import os
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
from cStringIO import StringIO
from Bio import Phylo
#from ete2 import Tree
# import dendropy

"""This file takes in the path to out_core.fasta and the 
best model selected from the jModel test to plot the tree as a whole
The function aligns the sequences in 
    >>> out_core.fasta > alignment.fasta
It then generates alignment in the phylip format >> alignment_phylip
In the end it generates tree
    >>> out_core.fasta_phyml_tree.txt
This tree is a newick format tree which is used for plotting the image
    >>> coretree.png
"""
"""Perform a muscle alignment over the sequence that is available"""
def muscle(path_to_seq):
    muscle_exe = "/afs/andrew.cmu.edu/usr23/lleung/muscle/muscle"
    muscle_cline = MuscleCommandline(muscle_exe, input=path_to_seq)
    head,filename = os.path.split(path_to_seq)
    stdout, stderr = muscle_cline()
    handle = open("aligned.fasta", "w")  #change name of alignment file
    handle.write(stdout)
    handle.close()
    
"""Takes in the alignment from MAFFT and the parameters from jModelTest and
generates the newick tree"""
def make_core_tree(path_to_seq, jmodel_gamma, jmodel_model,jmodel_pinv):
    handle2 = AlignIO.read(open(path_to_seq), "fasta") #name!
    h = open(path_to_seq+"_align_phylip", "w")       #name!
    h.write(handle2.format("phylip"))
    h.close()
    phyml_exe = "/afs/andrew.cmu.edu/usr23/lleung/phyml/phyml"
    cmdline = PhymlCommandline(phyml_exe,input=path_to_seq+"_align_phylip",
        alpha = jmodel_gamma, prop_invar=jmodel_pinv,model=jmodel_model)
    if jmodel_pinv == 0.0:
        if jmodel_gamma == 0.0:
            cmdline = PhymlCommandline(phyml_exe,input=path_to_seq+"_align_phylip",model=jmodel_model)
        else:
            cmdline = PhymlCommandline(phyml_exe,input=path_to_seq+"_align_phylip",alpha = jmodel_gamma,model=jmodel_model)
            #change name of alignment file and the model
    stdo,stder=cmdline()
    
"""Displaying the incoming newick tree"""
def tree_image(nwk_tree):
    f = open(nwk_tree)
    handle = StringIO(f.read().rstrip())
    tree = Phylo.read(handle, "newick")
    Phylo.draw(tree)
    plt.savefig("%s.png" %nwk_tree)
    plt.clf()
    
#    tree = Tree(tree2.read())
#    tree.render(nwk_tree+".png",  w=183, units="mm")
    
#def hgt_nwk_tree(ref_nwk_tree,positions,sequences):
#    """process the newick tree"""
#    f = open("ref_nwk_tree")
#    g = f.read()
#   pass
#
#make_core_tree("aligned.fasta_align_phylip",0.0,"GTR",0.0)