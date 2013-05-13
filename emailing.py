#!/usr/bin/env python

import smtplib
import string

# =============================================================================
# temp variable
# =============================================================================
TO = "luigileung@outlook.com"
completed_job_link = "http://www.contrib.andrew.cmu.edu/~lleung/project713/user_results/"
# =============================================================================

FROM = "03713.project@gmail.com"
SUBJECT = "Job Completed - A message from 03-713 Team B's web app"
text = """Requested job in Team B's 03-713 project is complete.\n
       Please view results at: %s \n\n Sincerely,\n Team B's web app
       """ %(completed_job_link)

BODY = string.join((
        "From: %s" % FROM,
        "To: %s" % TO,
        "Subject: %s" % SUBJECT ,
        "",
        text
        ), "\r\n")
 
# Credentials (to send email to user via gmail.com)
username = '03713.project'  
password = 'bioinformatics'  
 
# Sends email
server = smtplib.SMTP('smtp.gmail.com:587')  
server.starttls()  
server.login(username,password)  
server.sendmail(FROM, [TO], BODY)
server.quit() 
