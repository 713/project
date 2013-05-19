#!/usr/bin/env python

import smtplib
import string
from config import *

# Gets user's email from user_email.txt
with open("user_email.txt", "r") as email_file:
  user_email = email_file.read().replace('\n', '')

# =============================================================================
# Important variables
# =============================================================================
TO = user_email
completed_job_link = HOST_713PROJECT + "user_results.html"
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
