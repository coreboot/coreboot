#!/usr/bin/env python

# Usage: python NLBConfig.py configfile freebiospath

# Based on the commands in the user-supplied configfile, and several other
# config files known to this program, this program generates a Makefile,
# Makefile.settings, crt0.S file, etc. in the target directory, which is
# specified by the 'target' command in the user's configfile.  For more
# info, see .../freebios/Documentation/Config

# $Id$
# Author:
# Modified by Jan Kok to improve readability of Makefile, etc.

import sys
import os
import re
import string

debug = 0

# device variables
superio_decls = ''
superio_devices = []
numsuperio = 0

# Architecture variables
arch = ''
makebase = ''
crt0base = ''
ldscriptbase = ''

makeoptions = {}
makenooptions = {}

# Key is the rule name. Value is a mkrule object.
makebaserules = {}

# List of targets in the order defined by makerule commands.
makerule_targets = []

treetop = ''
target_dir = ''

objectrules = []
userdefines = []

# this is the absolute base rule, and so is very special. 
mainrulelist = "all"

def add_main_rule_dependency(new_dependency):
	global mainrulelist
	mainrulelist = mainrulelist + ' ' + new_dependency

# function to add an object to the set of objects to be made. 
# this is a tuple, object name, source it depends on, 
# and an optional rule (can be empty) for actually building
# the object
def addobject(object, sourcepath, rule, condition, variable):
	objectrules.append([object, topify(sourcepath),
			    rule, condition, variable])

# OK, let's face it, make sucks. 
# if you have a rule like this: 
# a.o: /some/long/path/a.c
# make won't apply the .c.o rule. Toy!

def addobject_defaultrule(object, sourcepath, condition, variable):
	# defaultrule = "\t $(CC) -c $(CFLAGS) -o $@ $<"
	defaultrule = "\t@echo $(CC) ... -o $@ $<\n\t@$(CC) -c $(CFLAGS) -o $@ $<"
	addobject(object, sourcepath, defaultrule, condition, variable)


# -----------------------------------------------------------------------------
#                    Class for storing and printing make rules
# -----------------------------------------------------------------------------

class mkrule:

	# This defines the function mkrule(target, depends, action).
	# It creates a makerule object and records it for later recall.
	def __init__(self, target, depends, actions):
		self.whence = current_config_file()
		self.comments = []       # list of strings
		self.target = target     # string
		self.depends = depends   # string of dependency names
		self.actions = actions   # list of strings
		if makebaserules.has_key(target):
			print "===> Warning: makerule for target '%s' in file" % target
			print current_config_file(), "is replacing previous definition in file"
			print makebaserules[target].whence
		else:
			# Keep a list of targets so we can output the rules
			# in the order that they are defined.
			makerule_targets.append(target)
		# I remember myself, therefore I am.
		makebaserules[target] = self

	def adddepend(self, depend):
		self.depends = self.depends + ' ' + depend

	def addaction(self, action):
		self.actions.append(action)

	def write(self, file):
		file.write("\n")

		if 1:	# Put comments before target : dependencies line.
			file.write("# from: %s\n" % self.whence)
			for comment in self.comments:
				file.write("# %s\n" % comment)

		# Write the target : dependencies line.
		file.write("%s: %s\n" % (self.target, self.depends))

		if 0:	# Put comments after target : dependencies line,
			# which causes them to be printed during make.
			file.write("\t# from: %s\n" % self.whence)
			for comment in self.comments:
				file.write("\t# %s\n" % comment)

		# Write out the actions.
		for action in self.actions:
			file.write("\t%s\n" % action)


# -----------------------------------------------------------------------------
#                        Command parsing functions
# -----------------------------------------------------------------------------

# Keep track of nested config files, for error reporting.
config_file_stack = []
def current_config_file():
	return config_file_stack[-1]

# The command being processed, for error reporting.
current_command = ''

# Match a compiled pattern with a string, die if fails, return list of groups.
def match(pattern, string):
	m = pattern.match(string)
	if not m:
		print "\nBad command syntax: ", current_command
		print "in file", current_config_file()
		sys.exit(1)
	return m.groups()

# A common pattern: <nonwhitespace><whitespace><rest of line>
splitargs_re = re.compile(r'(\S*)\s*(.*)')


# -----------------------------------------------------------------------------
#                        Command execution functions
# -----------------------------------------------------------------------------

# For all these functions,
# dir is the directory that the current Config file is in
# the second arg is the remainder of the command line (for most commands
#    any comments are stripped off)

# COMMAND: target <target-name>
# target must be the first command in the file. This command may only be
# used once.  target names the target directory for building this
# instance of the BIOS.  The target directory will be $(TOP)/<target-name>. 
def target(dir, targ_name):
	global target_dir
	target_dir = os.path.join(os.path.dirname(current_config_file()),
				targ_name)
	if not os.path.isdir(target_dir):
		print 'Creating directory', target_dir
		os.makedirs(target_dir)
	print 'Will place Makefile, crt0.S, etc. in ', target_dir

# dir should be the path to a directory containing a Config file.
# If so, process the commands in that config file right now.
def handleconfig(dir):
	file = os.path.join(dir, 'Config')
	print "Process config file: ", file
	if os.path.isfile(file):
		doconfigfile(dir, file)
	else:
		print "===> Warning: %s not found" % file

# type is the command name, e.g. 'northbridge'
# name is the path arg that followed the command
# <treetop>/src/<command>/<name>/Config is executed.
def common_command_action(dir, type, name):
	realpath = os.path.join(treetop, 'src', type, name)
	handleconfig(realpath)
	return realpath

# COMMAND: arch <my_arch>
# <my_arch> is typically i386 or alpha.
# Set various variables and execute a make.base file.
def set_arch(dir, my_arch):
	global arch, makebase, crt0base
	arch = my_arch
	configpath = os.path.join(treetop, "src/arch/", my_arch, "config")
	makebase = os.path.join(configpath, "make.base")
	crt0base = os.path.join(configpath, "crt0.base")
	print "Now Process the ", my_arch, " base files"
	if (debug):
		print "Makebase is :", makebase, ":"
	makedefine(dir, "ARCH="+my_arch)
	doconfigfile(treetop, makebase)

# COMMAND: dir <name>
# Execute the config file at <name>/Config
# If <name> begins with "/", it is interpreted as relative to $TOP,
# otherwise it is interpreted as relative to the directory of the config
# file that invoked this dir command.
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

# The mainboard directory, as an absolute path.  We need to remember this
# because the docipl command generates Makefile code that includes files
# from this directory.
mainboard_dir = None

# COMMAND: mainboard <mainboard name>
# The second command in a top-level config file is the mainboard command.
# This command may only be used once. The mainboard command names a mainboard
# source directory to use.  The file $(TOP)/src/<mainboard name>/Config
# is executed. 
def mainboard(dir, mainboard_name):
	global mainboard_dir
	mainboard_dir = common_command_action(dir, 'mainboard', mainboard_name)

# COMMAND: etherboot <network_device_name>
# Execute the file $(TOP)/src/etherboot/Config and set some Makefile variables.
def etherboot(dir, net_name):
	common_command_action(dir, 'etherboot', '')
	option(dir,'OLD_KERNEL_HACK')
	option(dir,'USE_TFTP')
	option(dir,'TFTP_INITRD')
	option(dir,'PYRO_SERIAL')

# COMMAND: keyboard ...
# old legacy PC junk, which will be dying soon. 
def keyboard(dir, keyboard_name):
	if (debug):
		print "KEYBOARD"
	keyboard_dir = os.path.join(treetop, 'src', keyboard_name)
	addobject_defaultrule('keyboard.o', keyboard_dir, '', 'OBJECTS')

# COMMAND: cpu <cpu name>
#
# This command may only be used once for a given CPU name.  Executes the
# config file $(TOP)/src/cpu/<cpu name>/Config
#
def cpu(dir, cpu_name):
	common_command_action(dir, 'cpu', cpu_name)

# COMMAND: northsouthbridge <vendor>/<northsouthbridge name>
#
# This command may only be used once.  Executes the config file
# $(TOP)/src/northsouthbridge/<vendor>/<northsouthbridge name>/Config
#
def northsouthbridge(dir, northsouthbridge_name):
	common_command_action(dir, 'northsouthbridge', northsouthbridge_name)
	
# COMMAND: northbridge <vendor>/<northbridge name>
#
# This command may only be used once.  Executes the config file
# $(TOP)/src/northbridge/<vendor>/<northbridge name>/Config
#
def northbridge(dir, northbridge_name):
	common_command_action(dir, 'northbridge', northbridge_name)

# COMMAND: southbridge <vendor>/<southbridge name>
#
# This command may only be used once.  Executes the config file
# $(TOP)/src/southbridge/<vendor>/<southbridge name>/Config
#
def southbridge(dir, southbridge_name):
	common_command_action(dir, 'southbridge', southbridge_name)

# COMMAND: superio <vendor> <superio name>
#
# This command may be used as many times as needed.
# I don't see a need yet to read in a config file for
# superio. Don't bother.
#
def superio(dir,  superio_name):
	# note that superio is w.r.t. treetop
	dir = os.path.join(treetop, 'src', 'superio', superio_name)
	addobject_defaultrule('superio.o', dir, '', 'OBJECTS')

# COMMAND: nsuperio ...
# commands are of the form: 
# superio_name [name=val]*
def nsuperio(dir, superio_commands):
	global superio_decls, superio_devices, numsuperio, target_dir
	# note that superio is w.r.t. treetop
	(superio_name, rest) = match(splitargs_re, superio_commands)
	superio_decl_name = re.sub("/", "_", superio_name)
	dir = os.path.join(treetop, 'src', 'superio', superio_name)
	object="superio_%s.o" % superio_decl_name
	superio_source = dir + "/superio.c"
	addobject_defaultrule(object, superio_source, '', 'OBJECTS')
	addobject_defaultrule('nsuperio.o', "", '', 'OBJECTS')

	superio_cmds = '';
	m = splitargs_re.match(rest)
	(cmd, rest) = m.groups()
	while (cmd):
		superio_cmds = superio_cmds + ", ." + cmd
		m = splitargs_re.match(rest)
		(cmd, rest) = m.groups()
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


# A list of files for initializing a motherboard.
mainboardinit_files = []

# COMMAND: mainboardinit <path_to_inc_file> [<condition>]
# Include the assembly language .inc file at $(TOP)/src/<path_to_inc_file>
# in the crt0.s file.
def mainboardinit(dir, command):
	(file, condition) = match(splitargs_re, command)
	mainboardinit_files.append( (file, condition) )
	print "Added mainboard init file: ", file

# COMMAND: raminit <path_to_inc_file>
# Include the assembly language .inc file at $(TOP)/src/<path_to_inc_file>
# in the crt0.s file. This command is essentially a duplicate of mainboardinit.
def raminit(dir, file):
	mainboardinit_files.append( (file, '') )
	print "Added ram init file: ", file

# Set of files we build the linker script from.
ldscripts = []

# COMMAND: ldscript <path_to_lds_file> <condition>
# $(TOP)/src/<path_to_lds_file> will be added to the Makefile variable
# LDSCRIPTS-1 if the <condition> is true or omitted.
def ldscript(dir, command):
	(file, condition) = match(splitargs_re, command)
	filepath = os.path.join(treetop, 'src', file);
	ldscripts.append( (filepath, condition) )
	print "Added ldscript init file: ", filepath

# COMMAND: object <path-to-source> [<condition>]
#
# <path-to-source> is a pathname to a .c or .S file rooted at
# $(TOP)/<path-to-source>.
# <condition> is a condition such as HAVE_PIRQ_TABLE.
# This command adds a rule to the Makefile for
# the target such that:
# 1) Basename of <path-to-source>.o depends on $(TOP)/<path-to-source>
# 2) romimage has a dependency on the .o
# 3) includepath for compiling the .o includes the directory containing the
#    source
# An alternative explanation: The basename of path-to-source, minus the
# .c or .S suffix, plus a .o suffix, is added to the OBJECTS-1 Makefile
# variable if the <condition> is true or omitted.  The OBJECTS-1 list is
# used (by a makrule in $(TOP)/src/config/Config) to make linuxbios.a,
# thus these objects are linked in only if needed to satisfy some reference.
#
def object(dir, command):
	(obj_name, condition) = match(splitargs_re, command)
	addobject_defaultrule(obj_name, dir, condition, 'OBJECTS')

# COMMAND: driver <path-to-source> [<condition>]
#
# Similar to object command, but these objects go into the Makefile variable
# DRIVERS-1 if the <condition> is true or omitted, and the objects listed
# in DRIVERS-1 are always linked in (via $(TOP)/src/config/Config) to
# linuxbios.o
#
def driver(dir, command):
	(obj_name, condition) = match(splitargs_re, command)
	addobject_defaultrule(obj_name, dir, condition, 'DRIVERS')

# COMMAND: makerule <target> : [<dependencies>] ; [<action>]
#
# Lets you add make rules to the Makefile.
#
makerule_re = re.compile(r'([^:\s]+)\s*:\s*([^;]*?)\s*;\s*(.*)')
def makerule(dir, rule): 
	(target, dependencies, action) = match(makerule_re, rule)
	# Each rule created by makerule will be printed to the Makefile
	# with a comment which shows the config file from whence it came.
	mkrule(target, dependencies, [action])

# COMMAND: addaction <target> <action>
#
# Add an action to an existing rule designated by <target>.
#
def addaction(dir, rule):
	(target, action) = match(splitargs_re, rule)
	if not makebaserules.has_key(target):
	    print "===> Warning: Need 'makerule %s ...' before addaction in %s" % (target, current_config_file())
        makebaserules[target].addaction(action)

# COMMAND: adddepend <target> <dependency>
#
# Add a dependency to an existing rule designated by <target>.
#
def adddepend(dir, rule):
	(target, depend) = match(splitargs_re, rule)
	if not makebaserules.has_key(target):
	    print "===> Warning: Need 'makerule %s ...' before adddepend in %s" % (target, current_config_file())
        makebaserules[target].adddepend(depend)

# COMMAND: makedefine <stuff>
# <stuff> is printed out to the Makefile.  <stuff> is usually of the form
# <VAR>=<value>
def makedefine(dir, rule): 
	userdefines.append(rule)

# Put "<option>:=<value>" and add <option> to VARIABLES in Makefile.settings
def set_option(option, value):
	if makenooptions.has_key(option):
		del makenooptions[option]
	makeoptions[option] = value

# Add <option> to VARIABLES in Makefile.settings
def clear_option(option):
	if makeoptions.has_key(option):
		del makeoptions[option]
	makenooptions[option] = ""

# COMMAND: option <option-name> [=<optional value>]
#
# This command affects the CFLAGS for the generated Makefile. If there is
# no optional value, then the option should appear in CFLAGS as:
# CFLAGS += -D<option-name>
# If there is a value, then the option should appear as:
# CFLAGS += -D<option-name>=<optional value>
# Also, put "<option>:=<value>" and add <option> to VARIABLES in
# Makefile.settings
#
option_re = re.compile(r"^([^=]*)=(.*)$")
def option(dir, option):
	m = option_re.match(option)
	key=option
	value= ""
	if m and m.group(1):
		(key, value) = m.groups()
	set_option(key, value)

# COMMAND: nooption <option-name>
# Add <option-namee> to VARIABLES and "-U<option-name>" to CFLAGS.
def nooption(dir, option):
	clear_option(option)

# COMMAND: commandline <stuff>
# Equivalent to "option CMD_LINE="<stuff>".
def commandline(dir, command):
	set_option("CMD_LINE", "\"" + command + "\"")

# COMMAND: docipl <ipl_name>
# We do all these rules by hand because docipl will always be special.
# It's more or less a stand-alone bootstrap.

def docipl(dir, ipl_name):

	# add the docipl rule
	add_main_rule_dependency('docipl')
	mkrule('docipl', 'ipl.o',
		['objcopy -O binary -R .note -R .comment -S ipl.o docipl'])

	# now, add the ipl.o rule
	iplpath = os.path.join(treetop, 'src', ipl_name)
	# Now we need a mainboard-specific include path
	mkrule('ipl.o', iplpath,
		['$(CC) $(CFLAGS) -I%s -c $<' % mainboard_dir])

	# now set new values for the ldscript.ld.  Should be a script? 
	set_option("_RAMBASE", "0x4000")
	set_option("_ROMBASE", "0x80000")

# COMMAND: payload <payload_name>
def payload(dir, payload_name):
	payloadrule = 'PAYLOAD=' + payload_name
	makedefine(dir, payloadrule)

# COMMAND: linux <linux_dir>
def linux(dir, linux_name):
	payload(dir, linux_name + '/vmlinux')

# COMMAND: rambase <address>
def rambase(dir, address):
	set_option("_RAMBASE", address)

# COMMAND: biosbase <address>
#
# Set the LinuxBIOS code base.  Default is 0xf0000.
#
def biosbase(dir, address):
	set_option("_ROMBASE", address)


# -----------------------------------------------------------------------------
#                            Config file processing
# -----------------------------------------------------------------------------

# This table translates command name strings into execution functions.

# NOTE: rather than creating new high-level commands like northbridge,
# etherboot, docipl, nsuperio, commandline, etc., consider using the
# lower-level commands such as dir, option, object, mainboardinit, etc.
# That would move the LinuxBIOS specialized knowledge out of this utility
# and into Config scripts, which would be more modular.

command_actions = {
	'arch'        : set_arch,
	'target'      : target,
	'mainboard'   : mainboard,
	'cpu'         : cpu,
	'northbridge' : northbridge,
	'southbridge' : southbridge,
	'northsouthbridge' : northsouthbridge,
	'superio'     : superio,
	'nsuperio'    : nsuperio,
	'object'      : object,
	'driver'      : driver,
	'linux'       : linux,
	'payload'     : payload,
	'raminit'     : raminit,
	'mainboardinit' : mainboardinit,
	'ldscript'    : ldscript,
	'dir'         : dir,
	'keyboard'    : keyboard, 
	'docipl'      : docipl,
	'etherboot'   : etherboot,
	'makedefine'  : makedefine,
	'makerule'    : makerule,
	'addaction'   : addaction,
	'option'      : option,
	'nooption'    : nooption,
	'rambase'     : rambase,
	'biosbase'    : biosbase,
	'commandline' : commandline
	}

# Open file, extract lines, and close.  For reading config and "base" files.
def readfile(filename): 
	if not os.path.isfile(filename):
		print filename, "is not a file \n"
		sys.exit()
	fileobject = open(filename, "r")
	filelines = fileobject.readlines()
	fileobject.close()
	return filelines

# Pattern for getting command and rest of line (if any).  Note that blank
# lines and pure comment lines cause the "verb" group to be ''.
# Trailing comments are matched and passed through to the "args" group.
# The pattern should always match, so we don't need to check that.
command_re = re.compile(r'\s*([^#\s]*)\s*(.*\S)?\s*')

# Strip whitespace and comment if any from end of line.
stripcomment_re = re.compile(r'([^#]*[^#\s])?.*')

# List of all config files processed.  This is used for making a list
# of dependencies for Makefile itself.
config_file_list = []

# Process config files.
def doconfigfile(dir, filename):
	global current_command

	if (debug):
		print "doconfigfile" , filename

	config_file_list.append(filename)

	# Keep track of current config file, for error reporting.
	config_file_stack.append(filename)

	if (debug):
		print "doconfigfile: config_file_list ", config_file_list
	filelines = readfile(filename)
	if (debug > 1):
		print "doconfigfile: filelines", filelines

	for line in filelines:
		current_command = line

		(verb, args) = command_re.match(line).groups()
		if not verb:
			# Line was blank or pure comment
			continue

		if not verb in ('makerule', 'addaction'):
			# The commands above can accept comments as regular
			# arguments.  For all other commands, strip
			# final whitespace and comments.
			args = stripcomment_re.match(args).group(1)

		if ((arch == '') and not verb in
					('arch', 'mainboard', 'target')):
			print "arch, target, or mainboard must be "
			print "the first commands, not ", verb
			sys.exit()

		if command_actions.has_key(verb):
			# Call the command execution function.
			command_actions[verb](dir, args)
		else:
			print verb, "is not a valid command! \n"
			sys.exit()

	config_file_stack.pop()


# -----------------------------------------------------------------------------
#                        Functions for writing output files
# -----------------------------------------------------------------------------

# Write out crt0.S (top-level assembly language) file.
def writecrt0(path):
	crt0filepath = os.path.join(path, "crt0.S")
	print "Creating", crt0filepath
	file = open(crt0filepath, 'w+')

	crt0lines = readfile(crt0base)
	if (debug):
		print "CRT0 ", crt0lines
        for line in crt0lines:
		if (string.strip(line) <> "CRT0_PARAMETERS"):
			file.write(line)
		else:
			for mbifile in mainboardinit_files:
				if mbifile[1]:
					file.write("#if %s == 1\n" %mbifile[1])
				file.write("#include <%s>\n" % mbifile[0])
				if mbifile[1]:
					file.write("#endif\n")

	file.close();


# Write doxygen file.
def writedoxygenfile(path):
	global objectrules, doxyscriptbase

	doxyfilepath = os.path.join(path, "LinuxBIOSDoc.config")
	print "Creating", doxyfilepath
	file = open(doxyfilepath, 'w+')

# FIXME. Should be computing .c once. Fix this in objectrules.append
	file.write("INPUT= \\\n")
	for objrule in objectrules:
		source = objrule[1]
		if (source[-2:] != '.c'): # no suffix. Build name.
			base = objrule[0][:-2]
			source = os.path.join(source, base) + ".c"
		file.write("%s \\\n" % source)

	doxylines = readfile(doxyscriptbase)
	if (debug):
		print "DOXYLINES ",doxylines
        for line in doxylines:
		file.write(line)
	file.close();

# If the first part of <path> matches treetop, replace that part with "$(TOP)"
def topify(path):
	global treetop
	if path[0:len(treetop)] == treetop:
		path = path[len(treetop):len(path)]
		if (path[0:1] == "/"):
			path = path[1:len(path)]
		path = "$(TOP)/" + path
	return path

def writemakefileheader(file, fname):
	file.write("# File: %s\n" % fname)
	file.write("# This file was generated by '%s %s %s'\n\n"
		% (sys.argv[0], sys.argv[1], sys.argv[2]))

def writemakefilesettings(path):
	# Write Makefile.settings to seperate the settings
	# from the actual makefile creation
	# In practice you need to rerun NLBConfig.py to change
	# these but in theory you shouldn't need to.

	filename = os.path.join(path, "Makefile.settings")
	print "Creating", filename
	file = open(filename, 'w+')
	writemakefileheader(file, filename)

	file.write("TOP=%s\n" % (treetop))

	keys = makeoptions.keys()
	keys.sort()
	for key in keys:
		file.write("export %s:=%s\n" % (key, makeoptions[key]))
	file.write("export VARIABLES := ");
	for key in keys:
		file.write("%s " % key)
	for key in makenooptions.keys(): 
		file.write("%s " % (key))
	file.write("\n");


# write the makefile
# we're not sure whether to write crt0.S yet. We'll see. 
# let's try the Makefile
# first, dump all the -D stuff

def writemakefile(path):
	makefilepath = os.path.join(path, "Makefile")
	print "Creating", makefilepath
	file = open(makefilepath, 'w+')
	writemakefileheader(file, makefilepath)

	# file.write("include Makefile.settings\n")
	# file.write("include cpuflags\n")
	# Putting "include cpuflags" in the Makefile has the problem that the
	# cpuflags file would be generated _after_ we want to include it.
	# Instead, let make do the work of computing CPUFLAGS:
	file.write("""\
# Get the value of TOP, VARIABLES, and several other variables.
include Makefile.settings

# Function to create an item like -Di586 or -DMAX_CPUS='1' or -Ui686
D_item = $(if $(subst undefined,,$(origin $1)),-D$1$(if $($1),='$($1)',),-U$1)

# Compute the value of CPUFLAGS here during make's first pass.
CPUFLAGS := $(foreach _var_,$(VARIABLES),$(call D_item,$(_var_)))
""")

	# print out all the object dependencies
	file.write("\n# object dependencies (objectrules:)\n")
	# There is ALWAYS a crt0.o
	file.write("OBJECTS-1 := crt0.o\n")
	file.write("DRIVERS-1 :=\n")
	for objrule in objectrules:
		obj_name = objrule[0]
		obj_cond = objrule[3]
		obj_var  = objrule[4]   # 'OBJECTS' or 'DRIVERS'
		if not obj_cond:
			file.write("%s-1 += %s\n" % (obj_var, obj_name))
		else:
			file.write("%s-$(%s) += %s\n"
					 % (obj_var, obj_cond, obj_name))

	# Print out all ldscript.ld dependencies.
	file.write("\n# ldscript.ld dependencies:\n")
	file.write("LDSUBSCRIPTS-1 := \n" )
	for (script, condition) in ldscripts:
		script = topify(script);
		if condition:
			file.write("LDSUBSCRIPTS-$(%s) += %s\n" % (condition, script))
		else:
			file.write("LDSUBSCRIPTS-1 += %s\n" % (script))

		
	file.write("\nSOURCES=\n")
	
	# Print out the user defines.
	file.write("\n# userdefines:\n")
	for udef in userdefines:
		file.write("%s\n" % udef)
		
	# Print out the base rules.
	# Need to have a rule that counts on 'all'.
	file.write("\n# mainrulelist:")
	file.write("\nmainrule: %s\n" % mainrulelist)

	# Print out any user rules.
	file.write("\n# From makerule or docipl commands:\n")
	# Old way (hash order): for target in makebaserules.keys():
	# New way (config file order):
	for target in makerule_targets:
		makebaserules[target].write(file)

	file.write("\n# objectrules:\n")
	for objrule in objectrules:
		source = objrule[1]
		if (source[-2:] != '.c'): # no suffix. Build name.
			base = objrule[0][:-2]
			source = os.path.join(source, base) + ".c"
		file.write("\nSOURCES += %s\n" % source)
		file.write("%s: %s\n" % (objrule[0], source))
		file.write("%s\n" % objrule[2])

	# Print out the dependencies for crt0.s
	file.write("\n# Dependencies for crt0.s\n")
	for mbifile in mainboardinit_files:
		file.write("crt0.s: $(TOP)/src/%s\n" % mbifile[0])

	# Print out the rules that will cause the files generated by
	# NLBConfig.py to be remade if any dependencies change.
	# It would seem sensible to write a rule with multiple targets, e.g.
	# Makefile Makefile.settings crt0.S nsuperio.c: ...dependencies...
	# but 'make' will run the rule for Makefile.settings (if out of date)
	# and then run it again for Makefile, not realizing that Makefile
	# was updated by the same command that updated Makefile.settings.
	# I haven't found a way to get 'make' to do the right thing, and
	# while it's not really harmful to run NLBConfig.py twice, it seems
	# silly and wasteful.  So the workaround is to let Makefile
	# represent all of the files generated by NLBConfig.py.

	# Note that crt0.base should be a dependency of crt0.S only,
	# but for the above reason, we make it a dependency of Makefile.

	file.write("\n# Remake Makefile (and the other files generated by\n")
	file.write("# NLBConfig.py) if any config dependencies change.\n")

	for cfile in config_file_list:
		file.write("Makefile: %s\n" % topify(cfile))

	for depfile in [ '$(TOP)/src/arch/$(ARCH)/config/crt0.base',
			'%s' % top_config_file,    # This a duplicate, remove?
			'$(TOP)/util/config/NLBConfig.py',
			'$(TOP)/src/arch/$(ARCH)/config/make.base' ]:
		file.write("Makefile:\t%s\n" % depfile)

	file.write("Makefile:\n")
	file.write("\tpython $(TOP)/util/config/NLBConfig.py %s $(TOP)\n"
			% top_config_file)

	file.close();

def writesuperiofile(path):
	superiofile = os.path.join(path, "nsuperio.c")
	print "Creating", superiofile
	file = open(superiofile, 'w+')

	file.write("#include <pci.h>\n")
	file.write(superio_decls)
        file.write("struct superio *all_superio[] = {");
	for sio_dev in superio_devices:
		file.write("%s,\n" % sio_dev)
        file.write("};\n")
	file.write("unsigned long nsuperio = %d;\n" % numsuperio)

# ---------------------------------------------------------------------
#                        MAIN
# ---------------------------------------------------------------------

if len(sys.argv) != 3:
	print "python NLBConfig.py <Config Filename> <src-path>\n"
	sys.exit()

# Retrieve config filename from command line.
top_config_file = os.path.abspath(sys.argv[1])

# Determine current directory and set default TOP.
treetop = os.path.abspath(sys.argv[2])
	
# Set the default locations for config files.
makebase = os.path.join(treetop, "util/config/make.base")
crt0base = os.path.join(treetop, "arch/i386/config/crt0.base")
doxyscriptbase = os.path.join(treetop, "src/config/doxyscript.base")

# Now read in the customizing script...
doconfigfile(treetop, top_config_file)

# ...and write out the Makefile and other files.
writemakefile(target_dir)
writemakefilesettings(target_dir)
writecrt0(target_dir)
writesuperiofile(target_dir)
writedoxygenfile(target_dir)
