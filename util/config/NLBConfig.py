#! /usr/bin/python

import sys
import os
import re
import string

debug = 0;

# device variables
superio_decls = '';
superio_devices = [];
numsuperio = 0;

# Architecture variables
arch = '';
makebase = '';
crt0base = '';
ldscriptbase = '';

makeoptions = {};
makenooptions = {};
# rule format. Key is the rule name. value is a list of lists. The first
# element of the list is the dependencies, the rest are actions. 
makebaserules = {};
treetop = '';
outputdir = '';

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
def addobject(object, sourcepath, rule, condition):
	objectrules.append([object, sourcepath, rule, condition])

# OK, let's face it, make sucks. 
# if you have a rule like this: 
# a.o: /some/long/path/a.c
# make won't apply the .c.o rule. Toy!

def addobject_defaultrule(object, sourcepath, condition):
	defaultrule = "\t $(CC) -c $(CFLAGS) -o $@ $<"
	addobject(object, sourcepath, defaultrule, condition)

# for all these functions, you need:
# the dir that the Config file is in
# the treetop (so make it global!)
# the name
# Functions used by commands
def top(dir, top_name):
	command_vals['TOP'] = top_name

def target(dir, targ_name):
	global outputdir
	outputdir = os.path.join(config_dir(), targ_name)
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

def etherboot(dir, net_name):
	common_command_action(dir, 'etherboot', '')
	option(dir,'OLD_KERNEL_HACK')
	option(dir,'USE_TFTP')
	option(dir,'TFTP_INITRD')
	option(dir,'PYRO_SERIAL')


# old legacy PC junk, which will be dying soon. 
def keyboard(dir, keyboard_name):
	if (debug):
		print "KEYBOARD"
	keyboard_dir = os.path.join(treetop, 'src', keyboard_name)
	addobject_defaultrule('keyboard.o', keyboard_dir,'')

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
	addobject_defaultrule('superio.o', dir,'')

# commands are of the form: 
# superio_name [name=val]*
def nsuperio(dir, superio_commands):
	global superio_decls,  superio_devices, numsuperio, outputdir
	# need python code to bust this into separate words ...
	wspc = string.whitespace
	rest = "(.*)"
	w = "[" + wspc + "]*"
	name = "([^" + wspc + "]*)"
	# break into name + commands
	pat = name + w + rest + w
	#	print "pat :", pat, ":", rule
	command_re = re.compile(pat)
	m = command_re.match(superio_commands)
	# note that superio is w.r.t. treetop
	superio_name = m.group(1);
	superio_decl_name = re.sub("/", "_", superio_name)
	buildfullpath('superio', superio_name)
	dir = os.path.join(treetop, 'src', 'superio', superio_name)
	object="superio_%s.o" % superio_decl_name
	superio_source = dir + "/superio.c"
	addobject_defaultrule(object, superio_source,'')
	addobject_defaultrule('nsuperio.o', "", '')
	rest = m.group(2)
	superio_cmds = '';
	m = command_re.match(rest)
	cmd = m.group(1)
	rest = m.group(2)
	while (cmd):
		superio_cmds = superio_cmds + ", ." + cmd
		m = command_re.match(rest)
		cmd = m.group(1)
		rest = m.group(2)
	# now build the declaration
	decl = '';
	decl = "extern struct superio_control superio_"
	decl = decl + superio_decl_name  + "_control; \n"
	decl = decl + "struct superio superio_" + superio_decl_name 
	decl = decl + "= { " 
	decl = decl + "&superio_" + superio_decl_name+ "_control"
	decl = decl + superio_cmds + "};\n"
	superio_decls = superio_decls + decl;
	superio_devices.append("&superio_" + superio_decl_name);
	# note that we're using the new interface
	option(dir, "USE_NEW_SUPERIO_INTERFACE")
	numsuperio = numsuperio + 1


# arg, what's a good way to do this ...
# basically raminit can have a whole list of files. That's why
# this is a list. 
def raminit(dir, file):
	ramfilelist = command_vals['mainboardinit']
	ramfilelist.append(file)
	print "Added ram init file: ", file

# A list of files for initializing a motherboard
def mainboardinit(dir, file):
	mainboardfilelist = command_vals['mainboardinit']
	mainboardfilelist.append(file)
	print "Added mainboard init file: ", file


# A set of linker scripts needed by linuxBIOS.
def ldscript(dir, file):
	ldscripts = command_vals['ldscripts']
	filepath = os.path.join(treetop, 'src');
	filepath = os.path.join(filepath, file);
	ldscripts.append(filepath)
	print "Added ldscript init file: ", filepath

def object(dir, command):
	wspc = string.whitespace
	command_re = re.compile("([^" + wspc + "]+)([" + wspc + "]([^" + wspc + "]*)|)")
	m = command_re.match(command)
	obj_name = m.group(1)
	condition = m.group(3)
	addobject_defaultrule(obj_name, dir, condition)

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
	#        print "rulename :", rulename
	#        print "    actions ", actions, "\n"
	#	print "rules[rulename]=", makebaserules[rulename], "\n"
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

def set_option(option, value):
	if makenooptions.has_key(option):
		del makenooptions[option]
	makeoptions[option] = value
	#print "option %s = %s\n" % (option, value)

def clear_option(option):
	if makeoptions.has_key(option):
		del makeoptions[option]
	makenooptions[option] = ""
	#print "nooption %s \n" % (option)
	
def option(dir, option):
	regexp = re.compile(r"^([^=]*)=(.*)$")
	m = regexp.match(option)
	key=option
	value= ""
	if m and m.group(1):
		key = m.group(1)
		value = m.group(2)
	set_option(key, value)

def nooption(dir, option):
	clear_option(option)

def commandline(dir, command):
	set_option("CMD_LINE", "\"" + command + "\"")

# we do all these rules by hand because docipl will always be special
# it's more or less a stand-alone bootstrap
def docipl(dir, ipl_name):
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
	userrules.append("\tcc $(CFLAGS) -I%s -c $<" % mainboard_dir)
	# now set new values for the ldscript.ld.  Should be a script? 
	set_option("_RAMBASE", "0x4000")
	set_option("_ROMBASE", "0x80000")

def linux(dir, linux_name):
	linuxrule = 'LINUX=' + linux_name
	makedefine(dir, linuxrule)

def setrambase(dir, address):
	set_option("_RAMBASE", address)

def setlinuxbiosbase(dir, address):
	set_option("_ROMBASE", address)

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
	'mainboardinit'     : [],   # set of files to include for mainboard init
	'config_files'      : [],   # set of files we built the makefile from
	'ldscripts'         : [],   # set of files we build the linker script from
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
	'nsuperio'    : nsuperio,
	'object'      : object,
	'linux'       : linux,
	'raminit'     : raminit,
	'mainboardinit'     : mainboardinit,
	'ldscript'    : ldscript,
	'dir'         : dir,
	'keyboard'    : keyboard, 
	'docipl'      : docipl,
	'etherboot'   : etherboot,
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
	config_file_list = command_vals['config_files']
	config_file_list.append(filename)
	if (debug):
		print "doconfigfile: config_file_list ", config_file_list
	filelines = readfile(filename)
	if (debug > 1):
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
			
			if ((arch == '') and (
			    (verb != 'arch') and (verb != 'mainboard') and 
				(verb != 'target')) ):
				print "arch, target, or mainboard must be "
				print "the first commands not ", verb, "\n"
				sys.exit()
			if command_actions.has_key(verb):
				command_actions[verb](dir, args)
			elif list_vals.has_key(verb):
				list_vals[verb] = list_vals[verb] + [args]
			else:
				print verb, "is not a valid command! \n"
				sys.exit()

def config_dir():
	config_file_list = command_vals['config_files']
	config_file = config_file_list[len(config_file_list) -1]
	return os.path.dirname(config_file)

# output functions
# write crt0
def writecrt0(path):
	crt0filepath = os.path.join(path, "crt0.S")
	mainboardinitfiles = command_vals['mainboardinit']
	#paramfile = os.path.join(treetop, 'src/include', 
	#		command_vals['northbridge'][0], "param.h")
	#paramfileinclude = ''

	print "Trying to create ", crt0filepath
#	try: 
	file = open(crt0filepath, 'w+')

	#print "Check for crt0.S param file:", paramfile
	#if os.path.isfile(paramfile):
	#	 ipfile = os.path.join(command_vals['northbridge'][0],"param.h")
	#	 paramfileinclude = "#include <%s>\n" %  ipfile
	#	 print " Adding include to crt0.S for this parameter file:"
	#	 print " ", paramfileinclude
	#
	## serial for superio
	#superioserial = "#include <%s/setup_serial.inc>\n" % command_vals['superio']

	crt0lines = readfile(crt0base)

	if (debug):
		print "CRT0 ", crt0lines
        for line in crt0lines:
		if (string.strip(line) <> "CRT0_PARAMETERS"):
			file.write(line)
		else:
			#file.write(paramfileinclude)
			## we will do this better at some point. 
			## possible we need a 'console' command.
			#file.write("\n");
			#file.write("#ifdef SERIAL_CONSOLE\n");
			#file.write(superioserial)
			#file.write("#include <pc80/serial.inc>\n");
			#file.write("TTYS0_TX_STRING($ttyS0_test)\n")
			#file.write("#endif /* SERIAL_CONSOLE */\n")
			#file.write("\n");

			for i in range(len(mainboardinitfiles)):
				file.write("#include <%s>\n" % mainboardinitfiles[i])

	file.close();

# write ldscript
def writeldscript(path):
	ldfilepath = os.path.join(path, "ldscript.ld")
	ldscripts = command_vals['ldscripts']
	print "Trying to create ", ldfilepath
#	try: 
	file = open(ldfilepath, 'w+')

	keys = makeoptions.keys()
	keys.sort()
	for key in keys:
		value = makeoptions[key]
		regexp = re.compile(r"^(0x[0-9a-fA-F]+|0[0-7]+|[0-9]+)$")
		if value and regexp.match(value):
			file.write("%s = %s;\n" % (key, value))
		
	ldlines = readfile(ldscriptbase)
	if (debug):
		print "LDLINES ",ldlines
        for line in ldlines:
		file.write(line)

	for i in range(len(ldscripts)):
		file.write("INCLUDE %s\n" % ldscripts[i])
		
	file.close();

# write doxygen file
def writedoxygenfile(path):
	global objectrules, doxyscriptbase
	doxyfilepath = os.path.join(path, "LinuxBIOSDoc.config")
	print "Trying to create ", doxyfilepath
#	try: 
	file = open(doxyfilepath, 'w+')
# FIXME. Should be computing .c once. Fix this in objectrules.append
	file.write("INPUT= \\\n")
	for i in range(len(objectrules)):
		source = objectrules[i][1]
		if (source[-2:] != '.c'): # no suffix. Build name. 
			base = objectrules[i][0]
			base = base[0:len(base)-2]
			source = os.path.join(objectrules[i][1], base)
			source = source + ".c"
		file.write("%s \\\n" % source)

	doxylines = readfile(doxyscriptbase)
	if (debug):
		print "DOXYLINES ",doxylines
        for line in doxylines:
		file.write(line)
	file.close();



# write the makefile
# we're not sure whether to write crt0.S yet. We'll see. 
# let's try the Makefile
# first, dump all the -D stuff

def writemakefile(path):
	makefilepath = os.path.join(path, "Makefile")
	mainboardinitfiles = command_vals['mainboardinit']
	config_file_list = command_vals['config_files']
	ldscripts = command_vals['ldscripts']

	print "Trying to create ", makefilepath
	keys = makeoptions.keys()
	keys.sort()
#	try: 
	file = open(makefilepath, 'w+')
	file.write("TOP=%s\n" % (treetop))
	for key in keys:
		if makeoptions[key] :
			file.write("%s=%s\n" % (key, makeoptions[key]))

	file.write("CPUFLAGS :=\n")
	for key in keys: 
#		print "key is %s, val %s\n" % (key, makeoptions[key])
#		file.write("CPUFLAGS += %s\n" % (makeoptions[key]))
		if makeoptions[key] :
			file.write("CPUFLAGS += -D%s=\'%s'\n" % (key, makeoptions[key]))
		else:
			file.write("CPUFLAGS += -D%s\n" % (key))

	for key in makenooptions.keys(): 
		file.write("CPUFLAGS += -U%s\n" % (key))
				
	# print out all the object dependencies
	# There is ALWAYS a crt0.o
	file.write("OBJECTS-1 := crt0.o\n")
	for i in range(len(objectrules)):
		obj_name = objectrules[i][0]
		obj_cond = objectrules[i][3]
		if not obj_cond :
			file.write("OBJECTS-1 += %s\n" % (obj_name))
		else:
			file.write("OBJECTS-$(%s) += %s\n" % (obj_cond, obj_name))
		
	file.write("SOURCES=\n")
	
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
		source = objectrules[i][1]
		if (source[-2:] != '.c'): # no suffix. Build name. 
			base = objectrules[i][0]
			base = base[0:len(base)-2]
			source = os.path.join(objectrules[i][1], base)
			source = source + ".c"
		file.write( "%s: %s\n" % (objectrules[i][0], source))
		file.write( "%s\n" % objectrules[i][2])
		file.write("SOURCES += %s\n" % source)

	# print out the linux rules
	# these go here because some pieces depend on user rules
	
#	for i in range(len(linuxrules)):
#		file.write("%s\n" % linuxrules[i])

	# print out the dependencies for crt0.s
	for i in range(len(mainboardinitfiles)):
		file.write("crt0.s: $(TOP)/src/%s\n" % mainboardinitfiles[i])

	# print out the dependencis for ldscript.ld
	for i in range(len(ldscripts)):
		file.write("ldscript.ld: %s\n" % ldscripts[i])

	# print out the dependencies for Makefile
	file.write("Makefile crt0.S ldscript.ld nsuperio.c: %s $(TOP)/util/config/NLBConfig.py $(TOP)/src/arch/$(ARCH)/config/make.base $(TOP)/src/arch/$(ARCH)/config/ldscript.base $(TOP)/src/arch/$(ARCH)/config/crt0.base \n\tpython $(TOP)/util/config/NLBConfig.py %s $(TOP)\n" 
		% (config_file, config_file))
	for i in range(len(config_file_list)):
		file.write("Makefile: %s\n" % config_file_list[i])

	file.close();
#	except IOError:
#		print "File open and write failed for ", makefilepath

def writesuperiofile(path):
	superiofile = os.path.join(path, "nsuperio.c")
	file = open(superiofile, 'w+')
	file.write("#include <pci.h>\n")
	file.write(superio_decls)
        file.write("struct superio *all_superio[] = {");
	for i in range(len(superio_devices)):
		file.write("%s,\n" %superio_devices[i])
        file.write("};\n")
	file.write("unsigned long nsuperio = %d;\n" % numsuperio)

# ---------------------------------------------------------------------
#                        MAIN
# ---------------------------------------------------------------------
# Retrieve config filename from command line
if len(sys.argv) != 3:
	print "LBConfig <Config Filename> <src-path>\n"
	sys.exit()

config_file = os.path.abspath(sys.argv[1])

# determine current directory and set default TOP
command_vals['TOP'] = os.path.abspath(sys.argv[2])
treetop = command_vals['TOP']
	
# set the default locations for config files
makebase = os.path.join(treetop, "util/config/make.base")
crt0base = os.path.join(treetop, "arch/i386/config/crt0.base")
ldscriptbase = os.path.join(treetop, "arch/alpha/config/ldscript.base")
doxyscriptbase = os.path.join(treetop, "src/config/doxyscript.base")

## now read in the base files. 
#print "Now Process the base files"
#print "Makebase is :", makebase, ":"
#doconfigfile(treetop, makebase)

# now read in the customizing script
doconfigfile(treetop, config_file)

# print out command values
#print "Command Values:"
#for key,val in command_vals.items():
#	print key, val

writemakefile(outputdir)
writeldscript(outputdir)
writecrt0(outputdir)
writesuperiofile(outputdir)
writedoxygenfile(outputdir)
