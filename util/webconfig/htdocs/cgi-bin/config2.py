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
	print '<p> here I am, %s</p>' % mbdir
	print '<p> Please select your options:<br>'
	LBDefOptions = [
		'CONFIGURE_L2_CACHE',
		'HAVE_FRAMEBUFFER',
		'PROTECTED_MODE_STARTUP',
		'SERIAL_CONSOLE',
		'SROM_CONSOLE',
		'UPDATE_MICROCODE',
		'USE_DOC_MIL'
		]
	LBBoolOptions = [
		'USE_CACHE_RAM',
		'USE_ELF_BOOT',
		'USE_GENERIC_ROM',
		'USE_TSUNAMI_TIGBUS_ROM'
		]
	LBHexOptions = [
		'TIG_KERNEL_START'
		]
	LBTextOptions = [
		'CMD_LINE'
		]
	for Opt in LBDefOptions:
		print '<input type="checkbox" name="%s">%s</br>' % (Opt,Opt)
	for Opt in LBBoolOptions:
		print '<input type="checkbox" name="%s">%s</br>' % (Opt,Opt)
	print '<hr>'
	for Opt in LBHexOptions:
		print '<input type="checkbox" name="%s">%s<br>' % (Opt,Opt)
		print '<input type="text" name="%s_val"><br> (enter Hex address e.g. 0x20000)<br>' % Opt
	print '<hr>'
	for Opt in LBTextOptions:
		print '<input type="checkbox" name="%s">%s<br>' % (Opt,Opt)
		print '<input type="text" name="%s_val"><br> (enter Text arguments e.g. "root=/dev/hda1")<br>' % Opt
	print '<hr>'

print '<input type="submit" value="Enter">'
# print '<input type="reset" value="Reset">'
print '</form>'

blocks.PrintTrailer()
