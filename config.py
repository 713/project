'''
Configuration Description:
==========================
data_dirpath         = path to directory that holds to sequences you wish to analyze
data_output          = the name of the output alignment file from mauve
data_backbone        = the name of the output backbone file from mauve
location_mauve       = path to mauve program
location_jModelTest  = path to jModelTest program
output_model         = name of model chosen by jModelTest

To email user when job is complete
EMAIL_HOST_USER      = 'your-username@gmail.com'
EMAIL_HOST_PASSWORD  = 'your-password'

Usage:
======
Inside alignment.py under the #! line, import the following variables using:
from config import *
'''

HOST_713PROJECT      = "http://localhost/"
data_dirpath         = "./upload/"
data_output          = "./user_results/"
data_backbone        = "./backbone.txt"
location_mauve       = "./dependencies/mauve_2.3.1/progressiveMauve"
location_jModelTest  = "./dependencies/jmodeltest-2.1.3/jModelTest.jar" 
output_model         = "./output_model_selection.txt"

EMAIL_HOST_USER      = '03713.project@gmail.com'
EMAIL_HOST_PASSWORD  = 'bioinformatics'

''' User don't have to change if using gmail '''
EMAIL_HOST           = 'smtp.gmail.com'
EMAIL_PORT           = 587
EMAIL_USE_TLS        = True
