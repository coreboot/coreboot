#!/usr/bin/python
import os
import sys
sys.path.insert(0, "../modules")
sys.path.insert(0, "./modules")
import blocks
import cgi

blocks.PrintHeader()

form = cgi.FieldStorage()
treedir = None
if form.has_key("freebios"):
	showtderr = "TRUE"
	treedir = form['freebios'].value
else:
	showtderr = None
	treedir = "/usr/src/freebios/src/mainboard/"

mfr = None
if form.has_key('manufacturer'):
	mfr = form['manufacturer'].value

if mfr:
	print '<form action="config2.py" method="POST">'
else:
	print '<form action="index.py" method="POST">'

dir_fail = None
try:
	os.chdir(treedir)
except:
	dir_fail = "TRUE"

if dir_fail:
	if showtderr:
		print '<p>Cannot change to requested directory %s</p>' % treedir
		print '<p>Please re-enter the location of the tree directory:<br>'
	else:
		print '<p>Please enter path to LinuxBIOS tree directory<br>or simply click "Enter" to accept the default %s</p>' % treedir
	print '<input name="freebios" type="text" size="50" value="%s"></p>' % treedir
else:
	print '<p>Tree directory: %s <input name="freebios" type="hidden" value="%s"></p>' % (treedir,treedir)
	if mfr:
		print '<p>Manufacturer: %s <input name="manufacturer" type="hidden" value="%s"></p>' % (mfr,mfr)
		print 'Motherboard: <select name="motherboard">'
		mblist = os.listdir(mfr)
		print '<option selected label="none" value="none">None</option>'
		for mb_item in mblist:
			if mb_item == "CVS":
				continue
			else:
				print '<option label="%s" value="%s">%s</option>' % (mb_item,mb_item,mb_item)
		print '</select><br>'
	else:
		print 'Manufacturer: <select name="manufacturer">'
		print '<option selected label="none" value="none">None</option>'
		mfrlist = os.listdir(".")
		for mfr_item in mfrlist:
			if mfr_item == "CVS":
				continue
			if mfr_item == mfr:
				print '<option selected label="%s" value="%s">%s</option>' % (mfr_item,mfr_item,mfr_item)
			else:
				print '<option label="%s" value="%s">%s</option>' % (mfr_item,mfr_item,mfr_item)
		print '</select><br>'

print '<input type="submit" value="Enter">'
# print '<input type="reset" value="Reset">'
print '</form>'

blocks.PrintTrailer()
