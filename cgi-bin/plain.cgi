#!/usr/bin/python
import cgi
test = "hello"
print "Content-type: text/html"
print
print "<pre>"
print ("<strong>Python %s</strong>" %test)
print "</pre>"
