import networkx as nx
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

def hgt_graph(HGT_dict):
	dict2 = dict()
	for i in HGT_dict:
	    dict2[i] = len(HGT_dict[i])

	G = nx.Graph()
	li = []
	for k in HGT_dict:
	    s,v = k
	    li.append(v)
	    li.append(s)
	li2 = []


	for key in dict2:
		s,v = key
		li2.append((s,v,dict2[key]))

	uniq = []
	for node in set(li):
	    uniq.append(node)

	G.add_nodes_from(uniq)
	G.add_weighted_edges_from(li2)
	
	pos = nx.spring_layout(G)
	nx.draw(G,pos)
	nx.draw_networkx_edge_labels(G,pos)
	plt.savefig("HGT_graph.png")
