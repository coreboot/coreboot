import sys
import os
import re
import string

debug = 0
warnings = 0
errors = 0

arch = ''
ldscriptbase = ''
payloadfile = ''

makeoptions = {}
makeexpressions = []

# Key is the rule name. Value is a mkrule object.
makebaserules = {}

# List of targets in the order defined by makerule commands.
makerule_targets = {}

treetop = ''
target_dir = ''

sources = {}
objectrules = {}
# make these a hash so they will be unique.
driverrules = {}
ldscripts = []
userdefines = []
curpart = 0
root = 0

globalvars = {}       # We will globals here
parts = {}

options = {}
# options in order defined. These will be unique due to use of hash 
# for options. 
options_by_order = []
crt0includes = []
partinstance = 0
curdir = '' 
dirstack = []
config_file_list = []

local_path = re.compile(r'^\.')

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

class makerule:
	def __init__ (self, target):
		self.target = target
		self.dependency = []
		self.actions = []

	def addaction(self, action):
		self.actions.append(action)

	def adddependency(self, dependency):
		self.dependency.append(dependency)

	def gtarget(self):
		return self.target

	def gdependency(self):
		return self.dependency

	def gaction(self):
		return self.actions

class option:
	def __init__ (self, name):
		self.name = name	# name of option
		self.loc = 0		# current location
		self.value = 0		# option value
		self.set = 0		# option has been set
		self.used = 0		# option has been set
		self.default = 0	# option has default value (otherwise
					# it is undefined)
		self.comment = ''	# description of option
		self.exportable = 0	# option is able to be exported
		self.exported = 0	# option is exported
		self.defined = 0	# option has a value
		self.format = '%s'	# option print format

	def setvalue(self, value, loc):
		if (self.set):
			print "Option %s: " % self.name
			print "Attempt to set %s at %s" % (value, loc.at()) 
			print "Already set to %s at %s" % \
					(self.value, self.loc.at())
			sys.exit(1)
		self.set = 1
		self.value = value
		self.defined = 1
		self.loc = loc

	def getvalue(self, part):
		global curpart
		if (not (type(self.value) is str)):
			return self.value
		if (self.value == '' or self.value[0] != '{'):
			return self.value
		# save curpart so we can evaluate expression
		# in context of part
		s = curpart
		curpart = part
		v = parse('delexpr', self.value)
		# TODO: need to check for parse errors!
		curpart = s
		return v

	def setdefault(self, value, loc):
		if (self.default):
			print "%s: " % self.name
			print "Attempt to default %s at %s" % (value, loc) 
			print "Already defaulted to %s at %s" %  \
					(self.value, self.loc.at())
			print "Warning only"
		if (self.set):
			print "%s: " % self.name
			print "Attempt to default %s at %s" % (value, loc) 
			print "Already set to %s at %s" % \
					(self.value, self.loc.at())
			print "Warning only"
			return
		self.value = value
		self.defined = 1
		self.default = 1
		self.loc = loc

	def setnodefault(self, loc):
		self.default = 1
		self.defined = 0
		self.loc = loc

	def where(self):
		return self.loc

	def setcomment(self, value, loc):
		if (self.comment != ''):
			print "%s: " % self.name
			print "Attempt to modify comment at %s" % loc 
			return
		self.comment = value

	def setexportable(self):
		self.exportable = 1

	def setexported(self):
		self.exportable = 1
		self.exported = 1

	def setnoexport(self):
		self.exportable = 0
		self.exported = 0

	def setformat(self, fmt):
		self.format = fmt

	def getformat(self):
		return self.format

	def setused(self):
		if (self.exportable):
			self.exported = 1
		self.used = 1

	def isexported(self):
		return (self.exported and self.defined)

	def isdefined(self):
		return (self.defined)

	def isset(self):
		return (self.set)

	def isused(self):
		return (self.used)

class partobj:
	def __init__ (self, dir, parent, type):
		global partinstance
		if (debug):
			print "partobj dir %s parent %s type %s" %(dir,parent,type)
		self.children = []
		self.initcode = []
		self.registercode = []
		self.siblings = 0
		self.type = type
		self.objects = []
		self.dir = dir
		self.irq = 0
		self.instance = partinstance + 1
		partinstance = partinstance + 1
		self.devfn = 0	
		self.private = 0	
		self.options = {}
		if (parent):
			if (debug):
				print "add to parent"
			self.parent   = parent
			parent.children.append(self)
		else:
			self.parent = self

	def dumpme(self, lvl):
		print "%d: type %s" % (lvl, self.type)
		print "%d: dir %s" % (lvl,self.dir)
		print "%d: parent %s" % (lvl,self.parent.type)
		print "%d: parent dir %s" % (lvl,self.parent.dir)
		print "%d: initcode " % lvl
		for i in self.initcode:
			print "  %s" % i
		print "%d: registercode " % lvl
		for i in self.registercode:
			print "  %s" % i

	def gencode(self):
		print "struct cdev dev%d = {" % self.instance
		print "/* %s %s */" % (self.type, self.dir)
		print "  .devfn = %d" % self.devfn
		if (self.siblings):
			print "  .next = &dev%d" % self.sibling.instance
		if (self.children):
			print "  .children = &dev%d" %  \
					self.children[0].instance
		if (self.private):
			print "  .private = private%d" % self.instance
		print "};"

		
	def irq(self, irq):
		self.irq = irq
       
    	def addinit(self, code):
        	self.initcode.append(code)
		
    	def addregister(self, code):
        	self.registercode.append(code)

	def usesoption(self, name):
		o = getvalue(options, name)
		if (o == 0):
			fatal("Error: can't use undefined option %s" % name)
		o.setused()
		setvalue(self.options, name, o)
		if (debug):
			print "option %s used in %s " % (name, self)

class partsstack:
	def __init__ (self):
		self.stack = []

	def push(self, part):
		self.stack.append(part)

	def pop(self):
		return self.stack.pop()

	def tos(self):
		return self.stack[-1]
pstack = partsstack()

def error(string):      
        global errors, loc
	errors = errors + 1
        size = len(loc.stack)
        i = 0
        while(i < size -1): 
                print loc.stack[i].at()
                i = i + 1
        print "%s: %s"% (loc.at(), string)

def exitiferrors():
	if (errors != 0):
		sys.exit(1)

def fatal(string):      
	error(string)
        exitiferrors()

def warning(string):
        global warnings, loc
	warnings = warnings + 1
        print "===> Warning:"
        size = len(loc.stack)
        i = 0
        while(i < size -1):
                print loc.stack[i].at()
                i = i + 1
        print "%s: %s"% (loc.at(), string)


# -----------------------------------------------------------------------------
#                    statements 
# -----------------------------------------------------------------------------

def getvalue(dict, name):
	if name not in dict.keys(): 
		if (debug >1):
			print 'Undefined:', name
		return 0
	v = dict.get(name, 0)
	if (debug > 1):
		print "getvalue %s returning %s" % (name, v)
	return v

def setvalue(dict, name, value):
	if name in dict.keys(): 
		print "Warning, %s previously defined" % name
    	if (debug > 1):
		print "setvalue sets %s to %s" % (name, value)
	dict[name] = value

# options. 
# to create an option, it has to not exist. 
# When an option value is fetched, the fact that it was used is 
# remembered. 
# Legal things to do:
# set a default value, then set a real value before the option is used.
# set a value, try to set a default, default silently fails.
# Illegal:
# use the value, then try to set the value

def newoption(name):
	o = getvalue(options, name)
	if (o):
		print "option %s already defined" % name
		sys.exit(1)
	o = option(name)
	setvalue(options, name, o)
	options_by_order.append(name)

# option must be declared before being used in a part
# if we're not processing a part, then we must
# be at the top level where all options are available
def getoption(name, part):
	global options
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o == 0 or not o.defined):
		error("Error: Option %s undefined (missing use command?)." % name)
		return
	v = o.getvalue(part)
	if (debug > 2):
		print "getoption returns %s" % v
		print "%s" % o.where()
	return v

# setoptionstmt only allowed at top level
def setoptionstmt(name, value):
	global curpart
	if (curpart != root):
		fatal("Error: options can only be set in target configuration file")
	setoption(name, value)

def setoption(name, value):
	global loc
	o = getvalue(options, name)
	if (o == 0):
		fatal("Error: attempt set nonexistent option %s" % name)
	o.setvalue(value, loc)

def setdefault(name, value):
	global loc
	o = getvalue(options, name)
	if (not o):
		return
	if (o.default):
		print "setdefault: attempt to duplicate default for %s" % name
		return
	o.setdefault(value, loc)

def setnodefault(name):
	global loc
	o = getvalue(options, name)
	if (not o):
		return
	if (o.default):
		print "setdefault: attempt to duplicate default for %s" % name
		return
	o.setnodefault(loc)

def setcomment(name, value):
	o = getvalue(options, name)
	if (not o):
		fatal("setcomment: %s not here" % name)
	o.setcomment(value, loc)

def setexported(name):
	o = getvalue(options, name)
	if (not o):
		fatal("setexported: %s not here" % name)
	o.setexported()

def setnoexport(name):
	o = getvalue(options, name)
	if (not o):
		fatal("setnoexport: %s not here" % name)
	o.setnoexport()

def setexportable(name):
	o = getvalue(options, name)
	if (not o):
		fatal("setexportable: %s not here" % name)
	o.setexportable()

def setformat(name, fmt):
	o = getvalue(options, name)
	if (not o):
		fatal("setformat: %s not here" % name)
	o.setformat(fmt)

def getformated(name, part):
	global options
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o == 0 or not o.defined):
		fatal( "Error: Option %s undefined (missing use command?)." % name)
	v = o.getvalue(part)
	f = o.getformat()
	return (f % v)

def isexported(name, part):
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o):
		return o.isexported()
	return 0

def isdefined(name, part):
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o):
		return o.isdefined()
	return 0

def isset(name, part):
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o):
		return o.isset()
	return 0

def isused(name, part):
	if (part):
		o = getvalue(part.options, name)
	else:
		o = getvalue(options, name)
	if (o):
		return o.isused()
	return 0

def usesoption(name):
	global curpart
	curpart.usesoption(name)

def validdef(name, defval):
	o = getvalue(options, name)
	if (not o):
		fatal("validdef: %s not here" % name)
	if ((defval & 1) != 1):
	    fatal("Error: must specify default value for option %s" % name)
	if ((defval & 2) != 2):
	    fatal("Error: must specify export for option %s" % name)
	if ((defval & 4) != 4):
	    fatal("Error: must specify comment for option %s" % name)

def loadoptions():
	global treetop
	optionsfile = os.path.join(treetop, 'src', 'config', 'Options.lb')
	loc.push_file(optionsfile)
	if (not parse('options', open(optionsfile, 'r').read())):
		fatal("Error: Could not parse file")
	loc.pop_file()

# we do the crt0include as a dictionary, so that if needed we
# can trace who added what when. Also it makes the keys
# nice and unique. 
def addcrt0include(path):
	global crt0includes
	#fullpath = os.path.join(curdir, path)
	#fullpath = path
	#setvalue(crt0includes, fullpath, loc)
	# oh shoot. Order matters. All right, we'll worry about this 
	# later. 
	fullpath = path
	if (debug > 2):
		print "ADDCRT0: %s" % fullpath
	crt0includes.append(fullpath)

def addldscript(path):
	global ldscripts
	if (path[0] == '/'):
		fullpath =  treetop + '/src/' + path
	else:
		fullpath = curdir + '/' + path
	#fullpath = os.path.join(curdir, path)
	#setvalue(ldscripts, fullpath, loc)
	if (debug):
		print "fullpath :%s: curdir :%s: path :%s:" % (fullpath, curdir, path)
	ldscripts.append(fullpath)

def payload(path):
	global payloadfile
	adduserdefine("PAYLOAD:=%s"%path)
#	addrule('payload')
#	adddep('payload', path)
#	addaction('payload', 'cp $< $@')

# this is called with an an object name. 
# the easiest thing to do is add this object to the current 
# component.
# such kludgery. If the name starts with '.' then make the 
# dependency be on ./thing.x gag me.
def addobjectdriver(dict, object_name):
	suffix = object_name[-2:]
	if (suffix == '.o'):
		suffix = '.c'
	base = object_name[:-2]
	if (object_name[0] == '.'):
		source = base + suffix
	else:
		source = os.path.join(curdir, base + suffix)
	object = base + '.o'
	if (debug):
		print "add object %s source %s" % (object_name, source)
	setvalue(dict, base, [object, source])

def addobject(object_name):
	addobjectdriver(objectrules, object_name)

def adddriver(driver_name):
	addobjectdriver(driverrules, driver_name)

def target(targ_name):
        global target_dir
	global curpart
	global root
	print "Configuring TARGET %s" % targ_name
        target_dir = os.path.join(os.path.dirname(loc.file()), targ_name)
        if not os.path.isdir(target_dir):
                print "Creating directory %s" % target_dir
                os.makedirs(target_dir)
        print "Will place Makefile, crt0.S, etc. in %s" % target_dir
	root = partobj(target_dir, 0, 'board')
	curpart = root

def part(name, path, file):
	global curpart,curdir,treetop
	dirstack.append(curdir)
	curdir = os.path.join(treetop, 'src', name, path)
	newpart = partobj(curdir, curpart, name)
	print "Configuring PART %s" % name
	if (debug):
		print "PUSH part %s %s" % (name, curpart.dir)
	pstack.push(curpart)
	curpart = newpart
	doconfigfile(curdir, file)

def partpop():
	global curpart,curdir
	print "End PART %s" % curpart.type
	# Warn if options are used without being set in this part
	for i in curpart.options.keys():
		if (not isset(i, curpart)):
			print "WARNING: Option %s using default value %s" % (i, getformated(i, curpart))
	curpart = pstack.pop()
	curdir = dirstack.pop()

# dodir is like part but there is no new part
def dodir(path, file):
	global curdir,  treetop
	# if the first char is '/', it is relative to treetop, 
	# else relative to curdir
	# os.path.join screws up if the name starts with '/', sigh.
	if (path[0] == '/'):
		fullpath = treetop + '/src/' + path 
	else:
		fullpath = os.path.join(curdir,  path)
	if (debug):
		print "DODIR: path %s, fullpath %s" % (path, fullpath)
		print "DODIR: curdis %s treetop %s" % (curdir, treetop)
	print "Configuring DIR %s" % os.path.join(path, file)
	dirstack.append(curdir)
	curdir = fullpath
	file = os.path.join(fullpath, file)
	config_file_list.append(file)
	doconfigfile(fullpath, file)
	curdir = dirstack.pop()

def lookup(name):
    global curpart
    v = getoption(name, curpart)
    exitiferrors()
    return v

def ternary(val, yes, no):
	if (debug):
		print "ternary %s" % expr
	if (debug):
		print "expr %s a %d yes %d no %d"% (expr, a, yes, no)
        if (val == 0):
		if (debug):
		    print "Ternary returns %d" % yes
		return yes
        else:
		if (debug):
			print "Ternary returns %d" % no
		return no

# atoi is in the python library, but not strtol? Weird!
def tohex(name):
	return eval('int(%s)' % name)

def IsInt( str ):
	""" Is the given string an integer?"""
	try:
		num = int(str)
		return 1
	except ValueError:
		return 0

def addrule(id):
	o = makerule(id)
	setvalue(makebaserules, id, o)
	
def adduserdefine(str):
	userdefines.append(str)

def dequote(str):
	a = re.sub("^\"", "", str)
	a = re.sub("\"$", "", a)
	# highly un-intuitive, need four \!
	a = re.sub("\\\\\"", "\"", a)
	return a

def addaction(id, str):
	o = getvalue(makebaserules, id)
	a = dequote(str)
	o.addaction(a)

def adddep(id, str):
	o = getvalue(makebaserules, id)
	a = dequote(str)
	o.adddependency(a)

# If the first part of <path> matches treetop, replace that part with "$(TOP)"
def topify(path):
	global treetop
	if path[0:len(treetop)] == treetop:
		path = path[len(treetop):len(path)]
		if (path[0:1] == "/"):
			path = path[1:len(path)]
		path = "$(TOP)/" + path
	return path

# arch is 'different' ... darn it.
def set_arch(my_arch):
	global arch
	global curdir
	arch = my_arch
	setoption('ARCH', my_arch)
	part('arch', my_arch, 'config/make.base.lb')


def mainboard(path):
	global mainboard_dir, treetop
	mainboard_dir = path
	full_mainboard_dir = os.path.join(treetop, 'src/mainboard', path)
	setoption('MAINBOARD', full_mainboard_dir)
        vendor = re.sub("/.*", "", path)
        mainboard_part_number = re.sub("[^/]/", "", path)
	setoption('MAINBOARD_VENDOR', vendor)
	setoption('MAINBOARD_PART_NUMBER', mainboard_part_number)
	part('mainboard', path, 'Config.lb')

#=============================================================================
#		FILE OUTPUT 
#=============================================================================
def writemakefileheader(file, fname):
	file.write("# File: %s\n" % fname)
	file.write("# This file was generated by '%s %s %s'\n\n"
		% (sys.argv[0], sys.argv[1], sys.argv[2]))


def writemakefilesettings(path):
	global treetop, arch, mainboard_dir, target_dir, options_by_order, root
	# Write Makefile.settings to seperate the settings
	# from the actual makefile creation
	# In practice you need to rerun NLBConfig.py to change
	# these but in theory you shouldn't need to.

	filename = os.path.join(path, "Makefile.settings")
	print "Creating", filename
	file = open(filename, 'w+')
	writemakefileheader(file, filename)
	file.write("TOP:=%s\n" % (treetop))
	#file.write("ARCH:=%s\n" % (arch))
	#file.write("MAINBOARD:=%s\n" % (mainboard_dir))
	file.write("TARGET_DIR:=%s\n" % (target_dir))
	for i in options_by_order:
		if (isexported(i, 0)):
			file.write("export %s:=%s\n" % (i, getformated(i, 0)))
	file.write("export VARIABLES := ")
	for i in options.keys():
		if (isexported(i, 0)):
			file.write("%s " % i)
	file.write("\n")
#	file.write("CPUFLAGS := ")
#	for i in options.keys():
#		o = options[i]
#		file.write("-D%s=%s " % (i, o.getvalue()))
#	file.write("\n")
	file.close()


# write the makefile
# let's try the Makefile
# first, dump all the -D stuff

def writemakefile(path):
	global root
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

	for i in userdefines:
		file.write("%s\n" %i)
	file.write("\n")

	# print out all the object dependencies
	file.write("\n# object dependencies (objectrules:)\n")
	file.write("OBJECTS :=\n")
	file.write("DRIVER :=\n")
	file.write("\nSOURCES :=\n")
	for objrule in objectrules.keys():
		obj = objectrules[objrule]
		obj_name = obj[0]
		obj_source = obj[1]
		file.write("OBJECTS-1 += %s\n" % (obj_name))
		file.write("SOURCES += %s\n" % (obj_source))

	for driverrule in driverrules.keys():
		driver = driverrules[driverrule]
		obj_name = driver[0]
		obj_source = driver[1]
		file.write("DRIVER += %s\n" % (obj_name))
		file.write("SOURCES += %s\n" % (obj_source))

	# Print out all ldscript.ld dependencies.
	file.write("\n# ldscript.ld dependencies:\n")
	file.write("LDSUBSCRIPTS-1 := \n" )
	for script in ldscripts:
		file.write("LDSUBSCRIPTS-1 += %s\n" % topify(script))

	# Print out the dependencies for crt0_includes.h
	file.write("\n# Dependencies for crt0_includes.h\n")
	file.write("CRT0_INCLUDES:=\n")
	for i in crt0includes:
		if (local_path.match(i)):
			file.write("CRT0_INCLUDES += %s\n" % i)
		else:
			file.write("CRT0_INCLUDES += $(TOP)/src/%s\n" % i)

	# Print out the user defines.
	file.write("\n# userdefines:\n")
	#for udef in userdefines:
		#file.write("%s\n" % udef)
		
	# Print out the base rules.
	# Need to have a rule that counts on 'all'.
	file.write("\n# mainrulelist:")
	#file.write("\nmainrule: %s\n" % mainrulelist)

	# Print out any user rules.
	file.write("\n# From makerule or docipl commands:\n")
	# Old way (hash order): for target in makebaserules.keys():
	# New way (config file order):
	#for target in makerule_targets:
		#makebaserules[target].write(file)

	file.write("\n# objectrules:\n")
	for objrule in objectrules.keys():
		obj = objectrules[objrule]
		source = topify(obj[1])
		file.write("%s: %s\n" % (obj[0], source))
		file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	for driverrule in driverrules.keys():
		driver = driverrules[driverrule]
		source = topify(driver[1])
		file.write("%s: %s\n" % (driver[0], source))
		file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	# Print out the rules that will make cause the files
	# generated by NLBConfig.py to be remade if any dependencies change.

	file.write("\n# Remember the automatically generated files\n")
	file.write("GENERATED:=\n")
	for genfile in [ 'Makefile',
			'Makefile.settings',
			'crt0_includes.h',
			'nsuperio.c',
			 'chip.c', 
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

	keys = root.options.keys()
	keys.sort()
	file.write("\necho:\n")
	for key in keys:
		 file.write("\t@echo %s='$(%s)'\n"% (key,key))
	

	for i in makebaserules.keys():
		m = makebaserules[i]
		file.write("%s: " %i)
		for i in m.dependency:
			file.write("%s " % i)
		file.write("\n")
		for i in m.actions:
			file.write("\t%s\n" % i)
	file.close()

# Write out crt0_includes.h (top-level assembly language) include file.
def writecrt0_includes(path):
	crt0filepath = os.path.join(path, "crt0_includes.h")
	print "Creating", crt0filepath
	file = open(crt0filepath, 'w+')

	for i in crt0includes:
		file.write("#include <%s>\n" % i)

	file.close()


%%
parser Config:
    ignore:			r'\s+'
    ignore:			"#.*?\r?\n"

    # less general tokens should come first, otherwise they get matched
    # by the re's
    token ACT:			'act'
    token ADDACTION:		'addaction'
    token ALWAYS:		'always'
    token ARCH:			'arch'
    token COMMENT:		'comment'
    token CPU:			'cpu'
    token DEFAULT:		'default'
    token DEFINE:		'define'
    token DEP:			'dep'
    token DIR:			'dir'
    token DRIVER:		'driver'
    token ELSE:			'else'
    token END:			'$|end'
    token EQ:			'='
    token EXPORT:		'export'
    token FORMAT:		'format'
    token IF:			'if'
    token INIT:			'init'
    token LDSCRIPT:		'ldscript'
    token LOADOPTIONS:		'loadoptions'
    token MAINBOARD:		'mainboard'
    token MAINBOARDINIT:	'mainboardinit'
    token MAKEDEFINE:		'makedefine'
    token MAKERULE:		'makerule'
    token NEVER:		'never'
    token NONE:			'none'
    token NORTHBRIDGE:		'northbridge'
    token OBJECT:		'object'
    token OPTION:		'option'
    token PAYLOAD:		'payload'
    token PMC:			'pmc'
    token PRINT:		'print'
    token REGISTER:		'register'
    token SOUTHBRIDGE:		'southbridge'
    token SUPERIO:		'superio'
    token TARGET:		'target'
    token USED:			'used'
    token USES:			'uses'
    token NUM:			r'[0-9]+'
    token XNUM:			r'0x[0-9a-fA-F]+'
    # Why is path separate? Because paths to resources have to at least
    # have a slash, we thinks
    token PATH:			r'[a-zA-Z0-9_.][a-zA-Z0-9/_.]+[a-zA-Z0-9_.]+'
    # Dir's on the other hand are abitrary
    # this may all be stupid.
    token DIRPATH:		r'[a-zA-Z0-9_$()./]+'
    token ID:			r'[a-zA-Z_.]+[a-zA-Z0-9_.]*'
    token DELEXPR:		r'{([^}]+|\\.)*}'
    token STR:			r'"([^\\"]+|\\.)*"'
    token RAWTEXT:		r'.*'

    rule expr:		logical			{{ l = logical }}
			( "&&" logical		{{ l = l and logical }}
			| "||"  logical		{{ l = l or logical }}
			)*			{{ return l }}

    rule logical:	factor			{{ n = factor }}
			( "[+]" factor		{{ n = n+factor }}
			| "-"  factor		{{ n = n-factor }}
			)*			{{ return n }}

    rule factor:	term			{{ v = term }}
			( "[*]" term		{{ v = v*term }}
			| "/"  term		{{ v = v/term }}
			| "<<"  term		{{ v = v << term }}
			| ">=" term		{{ v = (v < term)}}
			)*			{{ return v }}

    # A term is a number, variable, or an expression surrounded by parentheses
    rule term:		NUM			{{ return atoi(NUM) }}
		|	XNUM			{{ return tohex(XNUM) }}
		|	ID			{{ return lookup(ID) }}
		|	unop			{{ return unop }}
		|	"\\(" expr "\\)"	{{ return expr }}

    rule unop:		"!" expr		{{ return not(expr) }}

    rule partend<<C>>:	(stmt<<C>>)* END 	{{ partpop()}}

    rule mainboard:	MAINBOARD PATH		{{ mainboard(PATH) }}
			partend<<1>>
			
    rule northbridge<<C>>:
			NORTHBRIDGE PATH	{{ if (C): part('northbridge', PATH, 'Config.lb') }}
			partend<<C>>

    rule superio<<C>>:	SUPERIO PATH		{{ if (C): part('superio', PATH, 'Config.lb') }}
			partend<<C>>

    rule cpu<<C>>:	CPU ID			{{ if (C): part('cpu', ID, 'Config.lb') }}
			partend<<C>>

    rule pmc<<C>>:	PMC PATH		{{ if (C): part('pmc', PATH, 'Config.lb') }}
			partend<<C>>

    rule arch<<C>>:	ARCH ID			{{ if (C): set_arch(ID) }}
			partend<<C>>

    rule southbridge<<C>>:
			SOUTHBRIDGE PATH	{{ if (C): part('southbridge', PATH, 'Config.lb')}}
			partend<<C>>

    rule mainboardinit<<C>>: 
			MAINBOARDINIT DIRPATH	{{ if (C): addcrt0include(DIRPATH)}}

    rule object<<C>>:	OBJECT DIRPATH		{{ if (C): addobject(DIRPATH)}}

    rule driver<<C>>:	DRIVER DIRPATH		{{ if (C): adddriver(DIRPATH)}}

    rule dir<<C>>:	DIR DIRPATH 		{{ if (C): dodir(DIRPATH, 'Config.lb') }}

    rule ldscript<<C>>:	LDSCRIPT DIRPATH	{{ if (C): addldscript(DIRPATH) }}

    rule payload<<C>>:	PAYLOAD DIRPATH		{{ if (C): payload(DIRPATH) }}

# if is a bad id ....
# needs to be C and ID, but nested if's are, we hope, not going to 
# happen. IF so, possibly ID && C could be used.
    rule iif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(stmt<<c>>)* [ ELSE (stmt<<not c>>)* ] END

    rule depsacts<<ID, C>>:
			( DEP STR		{{ if (C): adddep(ID, STR) }}
			| ACT STR		{{ if (C): addaction(ID, STR) }}
			)*

    rule makerule<<C>>:	MAKERULE DIRPATH	{{ if (C): addrule(DIRPATH) }} 
			depsacts<<DIRPATH, C>> 

    rule makedefine<<C>>:
			MAKEDEFINE RAWTEXT	{{ if (C): adduserdefine(RAWTEXT) }}

    rule addaction<<C>>:
			ADDACTION ID STR	{{ if (C): addaction(ID, STR) }}

    rule init<<C>>:	INIT STR		{{ if (C): curpart.addinit(STR) }}

    rule register<<C>>:	REGISTER STR		{{ if (C): curpart.addregister(STR) }}

# to make if work without 2 passses, we use an old hack from SIMD, the 
# context bit. If the bit is 1, then ops get done, otherwise
# ops don't get done. From the top level, context is always
# 1. In an if, context depends on eval of the if condition
    rule stmt<<C>>:	cpu<<C>>		{{ return cpu}}
		|	pmc<<C>>		{{ return pmc}}
		|	arch<<C>>		{{ return arch}}
		|	northbridge<<C>>	{{ return northbridge }}
		|	southbridge<<C>>	{{ return southbridge }}
		|	superio<<C>>		{{ return superio }}
		|	object<<C>>		{{ return object }}
		|	driver<<C>>		{{ return driver }}
		|	mainboardinit<<C>>	{{ return mainboardinit }}
		|	makerule<<C>>		{{ return makerule }}
		|	makedefine<<C>> 	{{ return makedefine }}
		|	addaction<<C>>		{{ return addaction }}
		|	init<<C>>	 	{{ return init }}
		|	register<<C>> 		{{ return register}}
		|	iif<<C>>		{{ return iif }}
		|	dir<<C>>		{{ return dir}}
		|	ldscript<<C>>		{{ return ldscript}}
		|	payload<<C>>		{{ return payload}}
		| 	pprint<<C>>		{{ return pprint}}

    # ENTRY for parsing Config.lb file
    rule cfgfile:	(uses<<1>>)* (stmt<<1>>)*
						{{ return 1 }}

    rule usesid<<C>>:	ID			{{ if (C): usesoption(ID) }}

    rule uses<<C>>:	USES (usesid<<C>>)+

    rule value:		STR			{{ return dequote(STR) }} 
		| 	expr			{{ return expr }}
		|	DELEXPR			{{ return DELEXPR }}

    rule option<<C>>:	OPTION ID EQ value	{{ if (C): setoptionstmt(ID, value) }}

    rule opif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(opstmt<<C and c>>)* 
			[ ELSE (opstmt<<C and not c>>)* ] END

    rule opstmt<<C>>:	option<<C>>
		|	opif<<C>>
		|	pprint<<C>>

    # print clashes with a python keyword
    rule pprint<<C>>:   PRINT 
			( STR		{{ if (C): print "%s" % dequote(STR) }}
			| ID		{{ if (C): print "%s" % getformated(ID, curpart) }}
			)

    # ENTRY for parsing a delayed value
    rule delexpr:	"{" expr "}"		{{ return expr }}

    # ENTRY for parsing root part
    rule board:		LOADOPTIONS		{{ loadoptions() }}
	    		TARGET DIRPATH		{{ target(DIRPATH) }}
			(uses<<1>>)*
			(opstmt<<1>>)*
			mainboard		{{ return 1 }}

    rule defstmts<<ID>>:			{{ d = 0 }}
			( DEFAULT
			  ( value		{{ setdefault(ID, value) }}
			  | NONE		{{ setnodefault(ID) }}
			  )			{{ d = d | 1 }}
			| FORMAT STR		{{ setformat(ID, dequote(STR)) }}
			| EXPORT 
			  ( ALWAYS		{{ setexported(ID) }}
			  | USED		{{ setexportable(ID) }}
			  | NEVER		{{ setnoexport(ID) }}
			  )			{{ d = d | 2 }}
			| COMMENT STR 		{{ setcomment(ID, dequote(STR)); d = d | 4 }}
			)+			{{ return d }}
		
    rule define:	DEFINE ID 		{{ newoption(ID) }}
			defstmts<<ID>> END	{{ validdef(ID, defstmts) }}

    # ENTRY for parsing Options.lb file
    rule options:	(define)* END		{{ return 1 }}
%%

def dumptree(part, lvl):
	if (debug):
		print "DUMPTREE ME is"
	part.dumpme(lvl)
	# dump the siblings -- actually are there any? not sure
	# dump the kids
	if (debug):
		print "DUMPTREE KIDS are"
	for kid in part.children:
		dumptree(kid, lvl+1)
	if (debug):
		print "DONE DUMPTREE"

def gencode(part):
	if (debug):
		print "GENCODE ME is"
	if (debug):
		part.gencode()
	# dump the siblings -- actually are there any? not sure
	# dump the kids
	if (debug):
		print "GENCODE KIDS are"
	for kid in part.children:
		gencode(kid)
	if (debug):
		print "DONE GENCODE"
	

def doconfigfile(path, file):
	filename = os.path.join(path, file)
	loc.push_file(filename)
	if (not parse('cfgfile', open(filename, 'r').read())):
		fatal("Error: Could not parse file")
	exitiferrors()

if __name__=='__main__':
	from sys import argv
	if (len(argv) < 3):
		print 'Args: <file> <path to linuxbios>'
		sys.exit(1)

	top_config_file = os.path.abspath(sys.argv[1])

	treetop = os.path.abspath(sys.argv[2])

	# Now read in the customizing script...
	loc.push_file(argv[1])
	if (not parse('board', open(argv[1], 'r').read())):
		fatal("Error: Could not parse file")

	if (debug):
		print "DEVICE TREE:"
		dumptree(root, 0)

	gencode(root)

	# crt0 includes
	if (debug):
		for i in crt0includes:
			print "crt0include file %s" % (i)
		for i in driverrules.keys():
			print "driver file %s" % (i)
		for i in ldscripts:
			print "ldscript file %s" % (i)
		for i in makebaserules.keys():
			m = makebaserules[i]
			print " makerule %s dep %s act %s" % (i, m.dependency, m.actions)

	writemakefilesettings(target_dir)
	writecrt0_includes(target_dir)
	writemakefile(target_dir)
