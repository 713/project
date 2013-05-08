#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# enable debugging
import cgitb; cgitb.enable()

import cgi
import os, urllib, subprocess as sub


print "Content-Type: text/plain;charset=utf-8"
print
name = "Luigi"
print "Hello World!"
print """\
Content-Type: text/html\n
<html><body>
<p>The submitted name was "%s"</p>
</body></html>
""" % name

# form
form = cgi.FieldStorage()
name = form.getfirst('name', 'empty')

# Avoid script injection escaping the user input
name = cgi.escape(name)

print """\
Content-Type: text/html\n
<html><body>
<p>The submitted name was "%s"</p>
</body></html>
""" % name

# shell command to start our .py script
# Retrieve the command from the query string
# and unencode the escaped %xx chars

#str_command = urllib.unquote(os.environ['QUERY_STRING'])
str_command = "echo Hello" 

p = sub.Popen(['/bin/bash', '-c', str_command], 
    stdout=sub.PIPE, stderr=sub.STDOUT)
output = urllib.unquote(p.stdout.read())

print """\
Content-Type: text/html\n
<html><body>
<pre>
$ %s
%s
</pre>
</body></html>
""" % (str_command, output)
