#!/usr/bin/python
import cgi
import sys
import os

with open("aligned.fasta_align_phylip_phyml_tree.txt", "r") as newick_file:
  tree = newick_file.read()

print("Content-Type: text/html"                                           )
print(<html>                                                              )
print(<head>                                                              )
print(<script type="text/javascript" src="raphael-min.js" ></script>      )
print(<script type="text/javascript" src="jsphylosvg-min.js"></script>    )
print(                                                                    )
print(<script type="text/javascript">                                     )
print(window.onload = function(){                                         )
print(  var dataObject = { newick: '%s' };                                )
print(  phylocanvas = new Smits.PhyloCanvas(                              )
print(      dataObject,                                                   )
print(      'svgCanvas',                                                  )
print(      500, 500                                                      )
print(      );                                                            )
print(};                                                                  )
print(</script>                                                           )
print(</head>                                                             )
print(<body>                                                              )
print(<div id="svgCanvas"> </div>                                         )
print(</body>                                                             )
print(</html>		                                                          )
