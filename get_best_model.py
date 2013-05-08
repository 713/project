#!/usr/bin/env python

import os

def get_best_model(filename):
    assert os.path.isfile(filename)
    file = open("%s" %filename)
    found_best_model = False
    for line in file:
        if("::Best Models::") in line:
            found_best_model = True
        if(found_best_model == True):
            if("AIC" in line):
                line = line.split()
                model = line[1] #Model is present in the second column
                pInv = line[-2] #Prob. of invariant sites is the second last col.
                gamma = line[-1] #Gamma is the last column.
    return(model, pInv, gamma)

print get_best_model("D:\Bioinformatics Practicum\jmodeltest.txt")

