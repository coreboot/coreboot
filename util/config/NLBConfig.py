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
ldscriptbase = ''

makeoptions = {}
makeexpressions = []

# Key is the rule name. Value is a mkrule object.
makebaserules = {}

# List of targets in the order defined by makerule commands.
makerule_targets = []

treetop = ''
target_dir = ''

objectrules = []
userdefines = []


# -----------------------------------------------------------------------------
#                    Error Handling
# -----------------------------------------------------------------------------

class location:
	class place:
		def __init__(self, file, line, command):
			self.file = file
			self.line = line
			self.command = command
		def next_line(self, command):
			self.line = self.line + 1
			self.command = command
		def at(self):
			return "%s:%d" % (self.file, self.line)
	
	def __init__ (self):
		self.stack = []

	def file(self):
		return self.stack[-1].file
	def line(self):
		 return self.stack[-1].line
	def command(self):
		return self.stack[-1].command

	def push_file(self, file):
		self.stack.append(self.place(file, 0, ""))
	def pop_file(self):
		self.stack.pop()
	def next_line(self, command):
		self.stack[-1].next_line(command)
	def at(self):
		return self.stack[-1].at()
loc = location()

def fatal(string):	
	global loc
	size = len(loc.stack)
	i = 0
	while(i < size -1):
		print loc.stack[i].at()
		i = i + 1
	print "%s: %s"% (loc.at(), string)
	sys.exit(1)

def warning(string):
	global loc
	print "===> Warning:"
	size = len(loc.stack)
	i = 0
	while(i < size -1):
		print loc.stack[i].at()
		i = i + 1
	print "%s: %s"% (loc.at(), string)

# -----------------------------------------------------------------------------
#                    Rules
# -----------------------------------------------------------------------------

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
		self.whence = loc.file()
		self.comments = []       # list of strings
		self.target = target     # string
		self.depends = depends   # string of dependency names
		self.actions = actions   # list of strings
		if makebaserules.has_key(target):
			warning("makerule for target '%s' is replacing previous defintion in file %s" % 
			(target, makebaserules[target].whence))
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
		
# Match a compiled pattern with a string, die if fails, return list of groups.
def match(pattern, string):
	m = pattern.match(string)
	if not m:
		fatal("Bad command sytax")
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
	target_dir = os.path.join(os.path.dirname(loc.file()), targ_name)
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
		warning("%s not found" % file)

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
	global arch, makebase
	arch = my_arch
	configpath = os.path.join(treetop, "src/arch/", my_arch, "config")
	makebase = os.path.join(configpath, "make.base")
	print "Now Process the ", my_arch, " base files"
	if (debug):
		print "Makebase is :", makebase, ":"
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
	set_option('MAINBOARD_PART_NUMBER', os.path.basename(mainboard_name))
	set_option('MAINBOARD_VENDOR', os.path.dirname(mainboard_name))
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
	option(dir, "USE_NEW_SUPERIO_INTERFACE=1")
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
	if (debug):
		print "makerule " , rule
	(target, dependencies, action) = match(makerule_re, rule)
	# Each rule created by makerule will be printed to the Makefile
	# with a comment which shows the config file from whence it came.
	mkrule(target, dependencies, [action])

# COMMAND: addaction <target> <action>
#
# Add an action to an existing rule designated by <target>.
#
def addaction(dir, rule):
	if (debug):
		print "addaction " , rule
	(target, action) = match(splitargs_re, rule)
	if not makebaserules.has_key(target):
		warning("Need 'makerule %s ...' before addaction" % (target))
        makebaserules[target].addaction(action)

# COMMAND: adddepend <target> <dependency>
#
# Add a dependency to an existing rule designated by <target>.
#
def adddepend(dir, rule):
	(target, depend) = match(splitargs_re, rule)
	if not makebaserules.has_key(target):
		warning("Need 'makerule %s ...' before adddepend" % (target))
        makebaserules[target].adddepend(depend)

# COMMAND: makedefine <stuff>
# <stuff> is printed out to the Makefile.  <stuff> is usually of the form
# <VAR>=<value>
def makedefine(dir, rule): 
	userdefines.append(rule)


class mkexpr:
	class identifier:
		def __init__ (self, name):
			self.name = name
		def bc(self):
			return "($(" + self.name + "))"
		def perl(self):
			return "($(" + self.name + "))"

	class constant:
		def __init__ (self, value):
			self.value = value
		def bc(self):
			return "(" + self.value + ")"
		def perl(self):
			return "(" + self.value + ")"

	class unary:
		def __init__ (self, op, right):
			self.op = op
			self.right = right

		def bc(self):
			rstring = self.right.bc()
			if (self.op == "!"):
				result = "!" + rstring
			elif (self.op == "-"):
				result = "-" + rstring
			return "(" + result + ")"

		def perl(self):
			rstring = self.right.perl()
			if (self.op == "!"):
				result = "!" + rstring
			elif (self.op == "-"):
				result = "-" + rstring
			return "(" + result + ")"
		
	class binary:
		def __init__(self, op, left, right):
			self.op = op
			self.left = left
			self.right = right

		def bc(self):
			lstring = self.left.bc()
			rstring = self.right.bc()
			if (self.op == "&"):
				result = lstring + "&&" + rstring
			elif (self.op == "|"):
				result = lstring + "||" + rstring
			elif (self.op == "+"):
				result = lstring + "+" + rstring
			elif (self.op == "-"):
				result = lstring + "-" + rstring
			elif (self.op == "*"):
				result = lstring + "*" + rstring
			elif (self.op == "/"):
				result = lstring + "/" + rstring
			elif (self.op == "<<"):
				result = lstring + "*(2^" + rstring + ") "
			elif (self.op == ">>"):
				result = lstring + "/(2^" + rstring + ") "
			return "(" + result + ")"


		def perl(self):
			lstring = self.left.perl()
			rstring = self.right.perl()
			if (self.op == "&"):
				result = lstring + "&&" + rstring
			elif (self.op == "|"):
				result = lstring + "||" + rstring
			elif (self.op == "+"):
				result = lstring + "+" + rstring
			elif (self.op == "-"):
				result = lstring + "-" + rstring
			elif (self.op == "*"):
				result = lstring + "*" + rstring
			elif (self.op == "/"):
				result = lstring + "/" + rstring
			elif (self.op == "<<"):
				result = lstring + "<<" + rstring
			elif (self.op == ">>"):
				result = lstring + ">>" + rstring
			elif (self.op == ">"):
				result = lstring + ">" + rstring
			elif (self.op == ">="):
				result = lstring + ">=" + rstring
			elif (self.op == "<"):
				result = lstring + "<" + rstring
			elif (self.op == "<="):
				result = lstring + "<=" + rstring
			elif (self.op == "=="):
				result = lstring + "==" + rstring
			elif (self.op == "=="):
				result = lstring + "!=" + rstring
			return "(" + result + ")"

	class expression:
		def __init__(self, expr):
			self.expr = expr

		def bc(self):
			string = self.expr.bc()
			return "${shell echo '" + string + "' | bc}"

		def perl(self):
			string = self.expr.perl()
			return "${shell perl -e 'printf(\"%u\\n\", " + string + ");' }"
	
	# Tokens: ( ) ! | & + - * / << >> == <= >= < > != option )
	start_re     = re.compile(r"^\s*(([A-Za-z_][A-Za-z_0-9]*)|((0x[0-9A-Za-z]+)|([0-9]+))|(\()|(!))(.*)$")
	close_re     = re.compile(r"^\s*\)(.*)$")
#	middle_re    = re.compile(r"^\s*((\|)|(&)|(\+)|(-)|(\*)|(/)|(<<)|(>>)|(==)|(<=)|(>=)|(<)|(>))(.*)$")
	middle_re    = re.compile(r"^\s*((\|)|(&)|(\+)|(-)|(\*)|(/)|(<<)|(>>)|>=|<=|>|<|==|!=)(.*)$")

	def __init__(self, expr):
		self.orig_expr = expr
		self.expr = ""

	def prec(self, op):
		if (op == "|"):
			result = 1
		elif (op == "&"):
			result = 2
		elif (op == "<") or (op == ">") or \
			(op == ">=") or (op == "<=") or \
			(op == "==") or (op == "!="):
			result = 3
		elif (op == "+") or (op == "-"):
			result = 4
		elif (op == "*") or (op == "/"):
			result = 5
		elif (op == "<<") or (op == ">>"):
			result = 6
		else:
			fatal("Unknown operator: %s" % op)


	def _parse_start(self):
		#print("start expr: %s"%(self.expr))
		m = self.start_re.match(self.expr)
		if m:
			(token, self.expr) = (m.group(1), m.group(8))
			if token == "(":
				left = self._parse()
				#print("close expr: %s"%(self.expr))
				m = self.close_re.match(self.expr)
				if  m:
					self.expr = m.group(1)
				else:
					fatal("No Matching )");
			elif (token == "!"):
				right = self._parse()
				left = self.unary(token, right)
			elif m.group(2):
				if not makeoptions.has_key(token):
					fatal("Option %s not defined" % token)
				left = self.identifier(token)
			elif m.group(3):
				left = self.constant(token)
		else: 
			fatal("Invalid expression: %s" % self.expr)
		return left

	def _parse_middle(self, left):
		#print("middle expr: %s"%(self.expr))
		m = self.middle_re.match(self.expr)
		while(m):
			(op, self.expr) = (m.group(1),m.group(10))
			right = self._parse_start()
			m = self.middle_re.match(self.expr)
			if m and (self.prec(op) < self.prec(m.group(1))):
				right = self._parse_middle()
			left = self.binary(op, left, right)
		return left
		
	def _parse(self):
		left = self._parse_start()
		return self._parse_middle(left)

	def parse(self):
		self.expr = self.orig_expr
		result = self._parse()
		if self.expr != "":
			fatal("Extra tokens: %s"% self.expr)
		return self.expression(result)

# Put "<option>:=<value>" and add <option> to VARIABLES in Makefile.settings
def set_option(option, value):
	if makeoptions.has_key(option) and (makeoptions[option][0] == "expr"):
		fatal("Cannot replace expression %s" % option)
	makeoptions[option] = ("option", value)

# Add <option> to VARIABLES in Makefile.settings
# And remove any value <option> may already have.
def clear_option(option):
	if makeoptions.has_key(option) and (makeoptions[option][0] == "expr"):
		fatal("Cannot clear expression %s" % option)
	makeoptions[option] = ("nooption", "")

# Put "<option>:=<value>" and add <option> to VARIABLES in Makefile.settings
def set_expr(option, value):
	if makeoptions.has_key(option) and (makeoptions[option][0] == "expr"):
		fatal("Cannot replace expression %s" % option)
	makeexpressions.append(option);
	makeoptions[option] = ("expr", mkexpr(value).parse().perl())


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
option_re = re.compile(r"^([A-Za-z_][A-Za-z_0-9]+)\s*$")
option_arg_re = re.compile(r"^([A-Za-z_][A-Za-z_0-9]+)\s*=(.*)$")
def option(dir, option):
	m = option_arg_re.match(option)
	key = option
	value = ""
	if m and m.group(1):
		(key, value) = m.groups()
	else:
		warning("Invalid option specifcation: %s assuming you meant %s=1" % 
			(option, option) )
		value = "1"
	m = option_re.match(key)
	if not m:
		fatal("Invalid option name: %s" % (key))
	set_option(key, value)

# COMMAND: nooption <option-name>
# Add <option-name> to VARIABLES and "-U<option-name>" to CFLAGS.
def nooption(dir, option):
	clear_option(option)

# COMMAND: expr
def expr(dir, option):
	m = option_arg_re.match(option)
	value= ""
	if m and m.group(1):
		(key, value) = m.groups()
	else:
		fatal("Invalid expression: %s" % (option))
	set_expr(key, value)
	
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
	linuxrule = 'LINUX=' + linux_name + '/vmlinux'
	makedefine(dir, linuxrule)

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
	'commandline' : commandline,
	'expr'        : expr,
	}

# Open file, extract lines, and close.  For reading config and "base" files.
def readfile(filename): 
	if not os.path.isfile(filename):
		print filename, "is not a file \n"
		sys.exit(1)
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
	if (debug):
		print "doconfigfile" , filename

	config_file_list.append(filename)

	# Keep track of current config file, for error reporting.
	loc.push_file(filename)

	if (debug):
		print "doconfigfile: config_file_list ", config_file_list
	filelines = readfile(filename)
	if (debug > 1):
		print "doconfigfile: filelines", filelines

	for line in filelines:
		loc.next_line(line)

		(verb, args) = command_re.match(line).groups()
		if not verb:
			# Line was blank or pure comment
			continue

		if not verb in ('makerule', 'addaction'):
			# The commands above can accept comments as regular
			# arguments.  For all other commands, strip
			# final whitespace and comments.
			if args:
				args = stripcomment_re.match(args).group(1)

		if ((arch == '') and not verb in
					('arch', 'mainboard', 'target')):
			fatal("arch, target, or mainboard must be\n"
				"the first commands, not %s"%(verb))

		if command_actions.has_key(verb):
			# Call the command execution function.
			command_actions[verb](dir, args)
		else:
			fatal("%s is not a valid command!"% (verb))

	loc.pop_file()


# -----------------------------------------------------------------------------
#                        Functions for writing output files
# -----------------------------------------------------------------------------

# Write out crt0_includes.h (top-level assembly language) include file.
def writecrt0_includes(path):
	crt0filepath = os.path.join(path, "crt0_includes.h")
	print "Creating", crt0filepath
	file = open(crt0filepath, 'w+')

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

	file.write("TOP:=%s\n" % (treetop))
	file.write("ARCH:=%s\n" % (arch))
	file.write("MAINBOARD:=%s\n" % (mainboard_dir))
	file.write("TARGET_DIR:=%s\n" % (target_dir))

	keys = makeoptions.keys()
	keys.sort()
	for key in keys:
		(type, value) = makeoptions[key]
		if type == "option":
			file.write("export %s:=%s\n" % (key, value))
	
	# Write out the expressions in the order they were defined
	# This ensures expressions that depend on other expressions
	# will execute correctly
	i = 0
	while(i < len(makeexpressions)):
		key = makeexpressions[i]
		(type, value) = makeoptions[key]
		file.write("export %s:=%s\n" % (key, value))
		i = i + 1

	file.write("export VARIABLES := ");
	for key in keys:
		file.write("%s " % key)
	file.write("\n");

	file.close();


# write the makefile
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


	# Print out the dependencies for crt0_includes.h
	file.write("\n# Dependencies for crt0_includes.h\n")
	file.write("CRT0_INCLUDES:=\n")
	for mbifile in mainboardinit_files:
		file.write("CRT0_INCLUDES += $(TOP)/src/%s\n" % mbifile[0])

		
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

	# Print out the rules that will make cause the files
	# generated by NLBConfig.py to be remade if any dependencies change.

	file.write("\n# Remember the automatically generated files\n")
	file.write("GENERATED:=\n")
	for genfile in [ 'Makefile',
			'Makefile.settings',
			'crt0_includes.h',
			'nsuperio.c',
			'LinuxBIOSDoc.config' ]:
		file.write("GENERATED += %s\n" % genfile)

	file.write("\n# Remake Makefile (and the other files generated by\n")
	file.write("# NLBConfig.py) if any config dependencies change.\n")

	for cfile in config_file_list:
		file.write("$(GENERATED): %s\n" % topify(cfile))

	for depfile in [ '%s' % top_config_file,    # This a duplicate, remove?
			'$(TOP)/util/config/NLBConfig.py',
			'$(TOP)/src/arch/$(ARCH)/config/make.base' ]:
		file.write("$(GENERATED): %s\n" % depfile)

	file.write("$(GENERATED):\n")
	file.write("\tpython $(TOP)/util/config/NLBConfig.py %s $(TOP)\n"
			% top_config_file)

	keys = makeoptions.keys()
	keys.sort()
	file.write("\necho:\n")
	for key in keys:
		 file.write("\t@echo %s='$(%s)'\n"% (key,key))
	

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
	sys.exit(1)

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
# All submakfiles (Makefile.settings) must be written
# before the Makefile itself or there is a dependency issue
writemakefilesettings(target_dir)
writemakefile(target_dir)
writecrt0_includes(target_dir)
writesuperiofile(target_dir)
writedoxygenfile(target_dir)
