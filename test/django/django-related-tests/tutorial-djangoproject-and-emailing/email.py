#from django.core.mail import send_mail
#
## Send email to user when process is done.
##
## Temporary global variables:
## =============================================================================
## Project's URL
#b713_project_URL = "http://www.contrib.andrew.cmu.edu/~lleung/project713/"
##
## Sender's email (us)
#sender_email = "03713.project@gmail.com"
## Ask user for email on webpage
#user_email = "luigileung@gmail.com"
## =============================================================================
#
## Need to generate unique link.
## Using user's email address as unique id. Hash/checksum of it, if not working.
## Using email as unique link works.
#user_job_link = b713_project_URL + b713user_email + ".html"
#
## Using Django to send email
#send_mail("713 Project: Job Complete",
#        "Your submitted job is complete. Please go to %s for results." %user_job_link,
#        sender_email, ["%s" %user_email], fail_silently=False)
