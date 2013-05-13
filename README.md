Team B's Project
================
Please see manual for more information (Team B - User Manual.pdf)

## Git Usage
Click on that download ("ZIP") button on the top of this github page.  
Or in the terminal:
  
    $ mkdir teamB
    $ cd teamB
    $ git clone git@github.com:713/project.git .

This will download this repository to the directory that you are currently in.  
In the example above, it will be in the "teamB" directory.

## Special notes if hosting on AFS
AFS's current Python version 2.6.6 breaks the smtplib libary, which we are
using to send emails to users when requested job is finished. To circumvent
this problem without having root access to install a newer version of Python,
please use Enthought Python Distribution.  
For your convienence, instead of `$ python`, execute:

    $ ./afs/andrew.cmu.edu/usr23/lleung/epd_free-7.3-2-rh5-x86_64/bin/python

This is Python version 2.7.3  

To allow automated emailing when requested job is completed, a possible
solution is to add an alias to your .bashrc to execute this path when calling
`$ python`:

    $ echo 'alias python="./afs/andrew.cmu.edu/usr23/lleung/epd_free-7.3-2-rh5-x86_64/bin/python"' >> ~/.bashrc
    $ source ~/.bashrc

