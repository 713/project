import os
import shutil

def phylip():
    location = "E:/Projects and Trainings/JNU/Phylip/phylip-3.69/exe"
    #Generate distance matrix
    os.system("%s/protdist.exe" %(location))
    #Copyt the file as default input to neighbout
    shutil.copy2("%s/outfile" %(location), "%s/infile" %(location))
    #Generate neighbour joining tree
    os.system("%s/neighbor.exe" %(location))
phylip()
