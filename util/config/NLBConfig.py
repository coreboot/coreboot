#! /usr/bin/python

import sys
import os
import re
import string

debug = 0;

# Architecture variables
arch = '';
makebase = '';
crt0base = '';
ldscriptbase = '';

makeoptions = {};
# rule format. Key is the rule name. value is a list of lists. The first
# element of the list is the dependencies, the rest are actions. 
makebaserules = {};
treetop = '';
outputdir = '';

# config variables for the ldscript
# Initialize the to zero so we get a link error if the
# are not set.
rambase = 0;
linuxbiosbase = 0;

objectrules = [];
userrules = [];
userdefines = [];

# this is the absolute base rule, and so is very special. 
mainrulelist = "all"

def add_main_rule_dependency(new_dependency):
	global mainrulelist
	mainrulelist = mainrulelist + ' ' + new_dependency
# function to add an object to the set of objects to be made. 
# this is a tuple, object name, source it depends on, 
# and an optional rule (can be empty) for actually building
# the object
def addobject(object, sourcepath, rule):
	objectrules.append([object, sourcepath, rule])

# OK, let's face it, make sucks. 
# if you have a rule like this: 
# a.o: /some/long/path/a.c
# make won't apply the .c.o rule. Toy!

def addobject_defaultrule(object, sourcepath):
	defaultrule = "\t $(CC) -c $(CFLAGS) $<"
	objectrules.append([object, sourcepath, defaultrule])

# for all these functions, you need:
# the dir that the Config file is in
# the treetop (so make it global!)
# the name
# Functions used by commands
def top(dir, top_name):
	command_vals['TOP'] = top_name

def target(dir, targ_name):
	global outputdir
	outputdir = targ_name
	if os.path.isdir(outputdir):
		print 'Will place Makefile, crt0.S, ldscript.ld in ', outputdir
	else:
		print outputdir, 'is not a directory. '
		print ' I know I should make this, but I\'m too paranoid'
		print ' To fix this, type'
		print 'mkdir -p ', outputdir
		print ' then run this program again'
		sys.exit(1)

def handleconfig(dir):
	file = os.path.join(dir, 'Config')
	print "Process config file: ", file
	if os.path.isfile(file):
		doconfigfile(dir, file)
	else:
		print "===> Warning: %s not found" % file
	

def buildfullpath(type, name):
	fullpath = os.path.join(type, name)
	command_vals[type] = fullpath
	return fullpath

def common_command_action(dir, type, name):
	fullpath = buildfullpath(type, name)
	realpath = os.path.join('src', fullpath)
	# seems weird, eh? but config file are always done from
	# treetop
	realpath = os.path.join(treetop, realpath)
	handleconfig(realpath)
	return fullpath

def set_arch(dir, my_arch):
	global arch, makebase, crt0base, ldscriptbase
	arch = my_arch
	configpath = os.path.join(treetop, os.path.join("src/arch/", os.path.join(my_arch, "config")))
	makebase = os.path.join(configpath, "make.base")
	crt0base = os.path.join(configpath, "crt0.base")
	ldscriptbase = os.path.join(configpath, "ldscript.base")
	print "Now Process the ", my_arch, " base files"
	if (debug):
		print "Makebase is :", makebase, ":"
	makedefine(dir,  "ARCH="+my_arch)
	doconfigfile(treetop, makebase)

def dir(base_dir, name):
	regexp = re.compile(r"^/(.*)")
	m = regexp.match(name)
	if m and m.group(1):
		# /dir
		fullpath = os.path.join(treetop, m.group(1))
	else:
		# dir
		fullpath = os.path.join(base_dir, name)
	handleconfig(fullpath)

def mainboard(dir, mainboard_name):
	common_command_action(dir, 'mainboard', mainboard_name)

# old legacy PC junk, which will be dying soon. 
def keyboard(dir, keyboard_name):
	if (debug):
		print "KEYBOARD"
	keyboard_dir = os.path.join(treetop, 'src', keyboard_name)
	addobject_defaultrule('keyboard.o', keyboard_dir)

def cpu(dir, cpu_name):
	common_command_action(dir, 'cpu', cpu_name)

def northsouthbridge(dir, northsouthbridge_name):
	fullpath = common_command_action(dir, 'northsouthbridge',
			       northsouthbridge_name)
	command_vals["northbridge"] = [fullpath]
	command_vals["southbridge"] = [fullpath]

	
def northbridge(dir, northbridge_name):
	fullpath = common_command_action(dir, 'northbridge', northbridge_name)
	command_vals["northbridge"] = [fullpath]

def southbridge(dir, southbridge_name):
	fullpath = common_command_action(dir, 'southbridge', southbridge_name)
	command_vals["southbridge"] = [fullpath]

def pcibridge(dir, pcibridge_name):
	common_command_action(dir, 'picbridge', picbridge_name)

# I don't see a need yet to read in a config file for 
# superio. Don't bother. 
def superio(dir,  superio_name):
	# note that superio is w.r.t. treetop
	buildfullpath('superio', superio_name)
	dir = os.path.join(treetop, 'src', 'superio', superio_name)
	addobject_defaultrule('superio.o', dir)

# arg, what's a good way to do this ...
# basically raminit can have a whole list of files. That's why
# this is a list. 
def raminit(dir, file):
	ramfilelist = command_vals['raminit']
	ramfilelist.append(file)
	print "Added ram init file: ", file

def object(dir, obj_name):
	addobject_defaultrule(obj_name, dir)

# for eventual user-defined rules. 
# pattern is name : deps ; rule
# we'll look at adding rules later
def makerule(dir, rule): 
	wspc = string.whitespace
	rest = "(.*)"
	w = "[" + wspc + "]*"
	cmd = "([^" + wspc + "]*)"
	namepat = "([^;]*)"
	pat = cmd  + w + ":" + w + namepat + w + ";" + w + rest + w
	#	print "pat :", pat, ":", rule
	command_re = re.compile(pat)
	m = command_re.match(rule)
	if (not m):
		print "\nBadly formed rule: ", rule, "\n"
		sys.exit()
	rulename = m.group(1)
	dependencies = m.group(2)
	actions = m.group(3)
	# print "rulename :", rulename, ": deps:", dependencies,":"
	# print "    actions ", actions
	makebaserules[rulename] = [dependencies]
	makebaserules[rulename].append(actions)

def addaction(dir, rule):
        wspc = string.whitespace
        rest = "(.*)"
        w = "[" + wspc + "]*"
        namepat = "(.*)"
	cmd = "([^" + wspc + "]*)"
        pat = cmd  + w + rest + w
        # print "pat :", pat, ":", rule
        command_re = re.compile(pat)
        m = command_re.match(rule)
        rulename = m.group(1)
        actions = m.group(2)
        # print "rulename :", rulename
        # print "    actions ", actions, "\n"
	# print "rules[rulename]=", makebaserules[rulename], "\n"
        makebaserules[rulename].append(actions)
	
# add a dependency
def adddepend(dir, rule):
        wspc = string.whitespace
        rest = "(.*)"
        w = "[" + wspc + "]*"
        namepat = "(.*)"
        cmd = "([^" + wspc + "]*)"
        pat = cmd  + w + rest + w
        # print "pat :", pat, ":", rule
        command_re = re.compile(pat)
        m = command_re.match(rule)
        rulename = m.group(1)
        depend = m.group(2)
        # print "rulename :", rulename
        # print "    depend ", depend
        makebaserules[rulename][0] = makebaserules[rulename][0] + " " + depend

def makedefine(dir, rule): 
	userdefines.append(rule)

def option(dir, option):
	makeoptions[option] = "-D" + option

def nooption(dir, option):
	makeoptions[option] = "-U" + option

def commandline(dir, command):
	makeoptions["CMD_LINE"] = "-DCMD_LINE=\'\"" + command + "\"\'"

# we do all these rules by hand because docipl will always be special
# it's more or less a stand-alone bootstrap
def docipl(dir, ipl_name):
	global rambase, linuxbiosbase
	mainboard = command_vals['mainboard']
	mainboard_dir = os.path.join(treetop, 'src', mainboard)
	# add the docipl rule
	add_main_rule_dependency('docipl')
	userrules.append("docipl: ipl.o")
	userrules.append("\tobjcopy -O binary -R .note -R .comment -S ipl.o docipl")
	# now, add the ipl.o rule
	iplpath = os.path.join(treetop, 'src', ipl_name)
	userrules.append("ipl.o: " + iplpath)
	# Now we need a mainboard-specific include path
	userrules.append("\tcc $(CPUFLAGS) -I%s -c $<" % mainboard_dir)
	# now set new values for the ldscript.ld.  Should be a script? 
	rambase = 0x4000
	linuxbiosbase = 0x80000

def linux(dir, linux_name):
	linuxrule = 'LINUX=' + linux_name
	makedefine(dir, linuxrule)

def setrambase(dir, address):
	global rambase
	rambase = string.atol(address,0)

def setlinuxbiosbase(dir, address):
	global linuxbiosbase
	linuxbiosbase = string.atol(address,0)

list_vals = {
#	'option': []
	}
# Commands
# some of these command vals need to change to strings
# we have been evolving this as we go ...
command_vals = {
	'TOP'         : '',   # Top of bios source tree
	'target'      : '',   # Target directory for build 
	'mainboard'   : [],   # Vendor, Mainboard source directory
	'cpu'         : {},   # CPU configuration file
	'northbridge' : [],   # vendor, northbridge name
	'southbridge' : [],   # vendor, southbridge name
	'northsouthbridge' : [], # vendor, northsouthbridge name
	'pcibridge'   : [],   # vendor, bridgename
	'superio'     : [],   # vendor, superio name
	'object'      : {},   # path/filename.[cS]
	'raminit'     : [],   # set of files to include for ram init
	}

command_actions = {
	'arch'        : set_arch,
	'TOP'         : top,
	'target'      : target,
	'mainboard'   : mainboard,
	'cpu'         : cpu,
	'northbridge' : northbridge,
	'southbridge' : southbridge,
	'northsouthbridge' : northsouthbridge,
	'pcibridge'   : pcibridge,
	'superio'     : superio,
	'object'      : object,
	'linux'       : linux,
	'raminit'     : raminit,
	'dir'         : dir,
	'keyboard'    : keyboard, 
	'docipl'      : docipl,
	'makedefine'    : makedefine,
	'makerule'    : makerule,
	'addaction'    : addaction,
	'option'    : option,
	'nooption'    : nooption,
	'rambase': setrambase,
	'biosbase' : setlinuxbiosbase,
	'commandline' : commandline
	}

makeobjects = [];

def readfile(filename): 
	# open file, extract lines, and close
	if not os.path.isfile(filename):
		print filename, "is not a file \n"
		sys.exit()
	fileobject = open(filename, "r")
	filelines = fileobject.readlines()
	fileobject.close()
	return filelines

def doconfigfile(dir, filename):
	if (debug):
		print "doconfigfile" , filename
	filelines = readfile(filename)
	if (debug):
		print "doconfigfile: filelines", filelines
	# parse out command arguments /(.*)(#.*)/ for comments
	regexp = re.compile(r"([^#]*)(.*)")
	
	wspc = string.whitespace
	command_re = re.compile(r"([^" + wspc + "]*)([" + wspc + "]+)(.*)")
	
	for line in filelines:
		line = string.strip(line)
		m = regexp.match(line)
		command_txt = m.group(1)
# Need to run commands as they come in.		
		if command_txt:
			command = command_re.match(command_txt)
			verb = command.group(1)
			args = command.group(3)
			
			if ((arch == '') and (verb != 'arch')):
				print "arch must be the first command not ", verb, "\n"
				sys.exit()
			if command_actions.has_key(verb):
				command_actions[verb](dir, args)
			elif list_vals.has_key(verb):
				list_vals[verb] = list_vals[verb] + [args]
			else:
				print verb, "is not a valid command! \n"
				sys.exit()

# output functions
# write crt0
def writecrt0(path):
	crt0filepath = os.path.join(path, "crt0.S")
	raminitfiles = command_vals["raminit"]
	paramfile = os.path.join(treetop, 'src/include', 
			command_vals['northbridge'][0], "param.h")
	paramfileinclude = ''

	print "Trying to create ", crt0filepath
#	try: 
	file = open(crt0filepath, 'w+')

	print "Check for crt0.S param file:", paramfile
	if os.path.isfile(paramfile):
		ipfile = os.path.join(command_vals['northbridge'][0],"param.h")
		paramfileinclude = "#include <%s>\n" %  ipfile
		print " Adding include to crt0.S for this parameter file:"
		print " ", paramfileinclude

	# serial for superio
	superioserial = "#include <%s/setup_serial.inc>\n" % command_vals['superio']
	crt0lines = readfile(crt0base)

	if (debug):
		print "CRT0 ", crt0lines
        for line in crt0lines:
		if (string.strip(line) <> "CRT0_PARAMETERS"):
			file.write(line)
		else:
			file.write(paramfileinclude)
			# we will do this better at some point. 
			# possible we need a 'console' command.
			file.write("\n");
			file.write("#ifdef SERIAL_CONSOLE\n");
			file.write(superioserial)
			file.write("#include <pc80/serial.inc>\n");
			file.write("TTYS0_TX_STRING($ttyS0_test)\n")
			file.write("#endif /* SERIAL_CONSOLE */\n")
			file.write("\n");

			for i in range(len(raminitfiles)):
				file.write("#include <%s>\n" % raminitfiles[i])

	file.close();

# write ldscript
def writeldscript(path):
	global rambase, linuxbiosbase
	ldfilepath = os.path.join(path, "ldscript.ld")
	print "Trying to create ", ldfilepath
#	try: 
	file = open(ldfilepath, 'w+')
	# print out the ldscript rules
	# print out the values of defined variables
	file.write('_ROMBASE	= 0x%lx;\n' % linuxbiosbase)
	file.write('_RAMBASE	= 0x%lx;\n' % rambase)

	ldlines = readfile(ldscriptbase)
	if (debug):
		print "LDLINES ",ldlines
        for line in ldlines:
		file.write(line)
	file.close();



# write the makefile
# we're not sure whether to write crt0.S yet. We'll see. 
# let's try the Makefile
# first, dump all the -D stuff

def writemakefile(path):
	makefilepath = os.path.join(path, "Makefile")
	print "Trying to create ", makefilepath
#	try: 
	file = open(makefilepath, 'w+')
	file.write("TOP=%s\n" % (treetop))
	file.write("CPUFLAGS=\n")
	for z in makeoptions.keys(): 
#		print "key is %s, val %s\n" % (z, makeoptions[z])
		file.write("CPUFLAGS += %s\n" % (makeoptions[z]))
				
	# print out all the object dependencies
	# There is ALWAYS a crt0.o
	file.write("OBJECTS=crt0.o\n")
	for i in range(len(objectrules)):
		file.write("OBJECTS += %s\n" % (objectrules[i][0]))
		
	
	# print out the user defines
	for i in range(len(userdefines)):
#		print "rule %d is %s" % (i, userdefines[i])
		file.write("%s\n" % userdefines[i])
		
	# print out the base rules
	# need to have a rule that counts on 'all'
	file.write("mainrule: %s\n" % mainrulelist)
#	for i in range(len(baserules)):
#		file.write("%s\n" % baserules[i])

	for i in range(len(userrules)):
#		print "rule %d is %s" % (i, userrules[i])
		file.write("%s\n" % userrules[i])
		
	# print out any user rules
	for z in makebaserules.keys(): 
		file.write("%s: %s\n" % (z, makebaserules[z][0]))
		for i in range(len(makebaserules[z]) - 1):
			file.write("\t%s\n" % makebaserules[z][i+1])
	for i in range(len(objectrules)):
		base = objectrules[i][0]
		base = base[0:len(base)-2]
		source = os.path.join(objectrules[i][1], base)
		source = source + ".c"
		file.write( "%s: %s\n" % (objectrules[i][0], source))
		file.write( "%s\n" % objectrules[i][2])

	# print out the linux rules
	# these go here because some pieces depend on user rules
	
#	for i in range(len(linuxrules)):
#		file.write("%s\n" % linuxrules[i])

	file.close();
#	except IOError:
#		print "File open and write failed for ", makefilepath

# ---------------------------------------------------------------------
#                        MAIN
# ---------------------------------------------------------------------
# Retrieve config filename from command line
if len(sys.argv) != 3:
	print "LBConfig <Config Filename> <src-path>\n"
	sys.exit()

config_path, config_file = os.path.split(sys.argv[1])

# determine current directory and set default TOP
command_vals['TOP'] = sys.argv[2]
treetop = command_vals['TOP']
	
# set the default locations for config files
makebase = os.path.join(treetop, "util/config/make.base")
crt0base = os.path.join(treetop, "arch/i386/config/crt0.base")
ldscriptbase = os.path.join(treetop, "arch/alpha/config/ldscript.base")

## now read in the base files. 
#print "Now Process the base files"
#print "Makebase is :", makebase, ":"
#doconfigfile(treetop, makebase)

# now read in the customizing script
doconfigfile(treetop, sys.argv[1])

# print out command values
#print "Command Values:"
#for key,val in command_vals.items():
#	print key, val

writemakefile(outputdir)
writeldscript(outputdir)
writecrt0(outputdir)
