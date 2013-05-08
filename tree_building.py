#!/usr/bin/env python

from Bio.Align.Applications import MafftCommandline
from Bio import AlignIO
from Bio import Phylo

in_file = "seq"
mafft_cline = MafftCommandline(input=in_file)
f = mafft_cline()
stdout, stderr = mafft_cline()
handle = open("aligned.fasta", "w")
handle.write(stdout)
handle.close()
handle2 = AlignIO.read(open("aligned.fasta"), "fasta")
h = open("phylip-format-bp", "w")
h.write(handle2.format("phylip"))
h.close()
tree = Phylo.read("outtree","newick")
Phylo.draw(tree)

