from cStringIO import StringIO
import sys
import os
sys.path.insert(1,"../../biopython")
sys.path.insert(1,"../../networkx")
sys.path.insert(1,"../../matplotlib")
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import networkx as nx
from Bio import Phylo

#treedata = "(A, (B,C), (D,E))"
f = open("test.txt")
handle = StringIO(f.read().rstrip())
tree = Phylo.read(handle, "newick")
print tree
net = Phylo.to_networkx(tree)
nx.draw(net)
plt.savefig("test.png")
Phylo.draw(tree)
plt.savefig("test-phylo.png")
