#!/usr/bin/python
import os
import sys
sys.path.insert(0, "../modules")
sys.path.insert(0, "./modules")
import blocks
import cgi


form = cgi.FieldStorage()
treedir = None

if form.has_key('freebios'):
	treedir = form['freebios'].value
else:
	blocks.IncompleteData(form,"freebios")
	sys.exit()

if form.has_key('manufacturer'):
	mfr = form['manufacturer'].value
else:
	blocks.IncompleteData(form,"manufacturer")
	sys.exit()

if form.has_key('motherboard'):
	mb = form['motherboard'].value
else:
	blocks.IncompleteData(form,"motherboard")
	sys.exit()

blocks.PrintHeader()

print '<form action="build.py" method="POST">'

mbdir = treedir + mfr  + "/" + mb + "/"

dir_fail = None
try:
	os.chdir(mbdir)
except:
	dir_fail = "TRUE"

if dir_fail:
	print '<p>Cannot change to motherboard directory %s</p>' % mbdir
	print '<p>Please <a href=index.py>reselect motherboard</a></p>'
else:
	print '<p>Please fill in the following:</p>'
	print '<select name="arch">'
	print '<option selected label="i386" value="i386">i386</option>'
	print '<option label="alpha" value="alpha">alpha</option>'
	print '</select>'
	print '<select name="target">'


print '<input type="submit" value="Enter">'
# print '<input type="reset" value="Reset">'
print '</form>'

blocks.PrintTrailer()
