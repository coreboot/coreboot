import sys
import os
import re
import string

debug = 3

arch = ''
makebase = ''
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
		self.name = name
		self.loc = 0
		self.value = 0
		self.set = 0
		self.default = 0

	def setvalue(self, value, loc):
		if (self.set):
			print "Option %s: \n" % self.name
			print "Attempt to set %s at %s\n" % (value, loc.at()) 
			print "Already set to %s at %s\n" % \
					(self.value, self.loc.at())
			sys.exit(1)
		self.set = 1
		self.value = value
		self.loc = loc


	def getvalue(self):
		self.set = 1
		return self.value

	def setdefault(self, value, loc):
		if (self.default):
			print "%s: " % self.name
			print "Attempt to default %s at %s\n" % (value, loc) 
			print "Already defaulted to %s at %s\n" %  \
					(self.value, self.loc.at())
			print "Warning only\n"
		if (self.set):
			print "%s: " % self.name
			print "Attempt to default %s at %s\n" % (value, loc) 
			print "Already set to %s at %s\n" % \
					(self.value, self.loc.at())
			print "Warning only\n"
			return
		self.default = 1
		self.value = value
		self.loc = loc
	def where(self):
		return self.loc


class partobj:
	def __init__ (self, dir, parent, type):
		global partinstance
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
		if (parent):
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
			print "  %s\n" % i
		print "%d: registercode " % lvl
		for i in self.registercode:
			print "  %s\n" % i

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
#                    statements 
# -----------------------------------------------------------------------------

def getvalue(dict, name):
    if name not in dict.keys(): print 'Undefined:', name
    v = dict.get(name, 0)
    if (debug > 1):
	print "getvalue %s returning %s\n" % (name, v)
    return v

def setvalue(dict, name, value):
	print "name %s value %s" % (name, value)
	if name in dict.keys(): print "Warning, %s previously defined" % name
    	if (debug > 1):
		print "setvalue sets %s to %s\n" % (name, value)
	dict[name] = value

# options. 
# to create an option, it has to no exist. 
# When an option value is fetched, the fact that it was used is 
# remembered. 
# Legal things to do:
# set a default value, then set a real value before the option is used.
# set a value, try to set a default, default silently fails.
# Illegal:
# use the value, then try to set the value

def getoption(name):
	print "getoption %s" % name
	o = getvalue(options, name)
	if (o == 0):
		fatal( "Error. Option %s Undefined. Fix me.\n" % name)
	if (debug > 2):
		print "returns %s" % o
		print "%s" % o.where()
	return o.getvalue()

# stupid code, refactor later. 
def setoption(name, value):
	o = getvalue(options, name)
	if (o):
		o.setvalue(value, loc)
		return
	o = option(name)
	o.setvalue(value, loc)
	setvalue(options, name, o)
	options_by_order.append(name)

def setdefault(name, value):
	o = getvalue(options, name)
	if (o):
		o.setdefault(value, loc)
		return
	print "setdefault: %s not here, setting to %s" % \
			(name, value)
	o = option(name)
	o.setdefault(value, loc)
	setvalue(options, name, o)
	options_by_order.append(name)

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
		print "ADDCRT0: %s\n" % fullpath
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
		print "fullpath :%s: curdir :%s: path :%s:\n" % (fullpath, curdir, path)
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
	print "add object object_name %s" % object_name
	suffix = object_name[-2:]
	if (suffix == '.o'):
		suffix = '.c'
	base = object_name[:-2]
	if (object_name[0] == '.'):
		source = base + suffix
	else:
		source = os.path.join(curdir, base + suffix)
	object = base + '.o'
	print "addobject %s source %s\n" % (object, source)
	setvalue(dict, base, [object, source])

def addobject(object_name):
	addobjectdriver(objectrules, object_name)

def adddriver(driver_name):
	addobjectdriver(driverrules, driver_name)

def target(targ_name):
        global target_dir
	global curpart
	print "TARGET loc.file is %s\n" % loc.file()
        target_dir = os.path.join(os.path.dirname(loc.file()), targ_name)
	print "TARGET dir.loc.file is %s\n" % os.path.dirname(loc.file())
        if not os.path.isdir(target_dir):
                print 'Creating directory', target_dir
                os.makedirs(target_dir)
        print 'Will place Makefile, crt0.S, etc. in ', target_dir
	print '%s\n' % loc.file()
	board = partobj(target_dir, 0, 'board')
	curpart = board



def part(name, path, file):
	global curpart,curdir
	if (debug):
		print "%s " % name
	dirstack.append(curdir)
	curdir = os.path.join(treetop, 'src', name, path)
	newpart = partobj(curdir, curpart, name)
	print "PUSH part " , curpart.dir
	pstack.push(curpart)
	curpart = newpart
	#	option(parts, name, path)
	# open the file, and parse it. 
#        curpart.option('MAINBOARD_PART_NUMBER', 
#			os.path.basename(lookup(parts, 'mainboard')))
#        option(options, 'MAINBOARD_VENDOR', os.path.dirname(getvalue(parts, 'mainboard')))

	doconfigfile(curdir, file)

def partpop():
	global curpart,curdir
	curpart = pstack.pop()
	curdir = dirstack.pop()
	print "POP PART %s\n" % curpart.dir

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
		print "DODIR: path %s, fullpath %s\n" % (path, fullpath)
		print "DODIR: curdis %s treetop %s\n" % (curdir, treetop)
	dirstack.append(curdir)
	curdir = fullpath
	file = os.path.join(fullpath, file)
	config_file_list.append(file)
	doconfigfile(fullpath, file)
	curdir = dirstack.pop()

def ternary(expr, yes, no):
	print "ternary %s" % expr
        a = tohex(expr) # expr # eval(expr)
        print "expr %s a %d yes %d no %d\n"% (expr, a, yes, no)
        if (a == 0):
	    print "Ternary returns %d\n" % yes
            return yes
        else:
	    print "Ternary returns %d\n" % no
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
	global arch, makebase
	global curdir
	arch = my_arch
	setdefault('ARCH', my_arch)
	part('arch', my_arch, 'config/make.base.lb')


def mainboard(path):
	global mainboard_dir
	mainboard_dir = path
	full_mainboard_dir = os.path.join(treetop, 'src/mainboard', path)
	setdefault('MAINBOARD', full_mainboard_dir)
        vendor = re.sub("/.*", "", path)
        mainboard_part_number = re.sub("[^/]/", "", path)
	setdefault('MAINBOARD_VENDOR', vendor)
	setdefault('MAINBOARD_PART_NUMBER', mainboard_part_number)
	part('mainboard', path, 'Config.lb')
#=============================================================================
#		FILE OUTPUT 
#=============================================================================
def writemakefileheader(file, fname):
	file.write("# File: %s\n" % fname)
	file.write("# This file was generated by '%s %s %s'\n\n"
		% (sys.argv[0], sys.argv[1], sys.argv[2]))


def writemakefilesettings(path):
	global treetop, arch, mainboard_dir, target_dir, options
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
		o = options[i]
		# get the number in hex
		v =  o.getvalue()
		if IsInt(v):
			vi = int(v)
			s = "export %s:=0x%x\n"% (i, vi)
			file.write(s)
		else:
			file.write("export %s:=%s\n" % (i, v))
	file.write("export VARIABLES := ");
	for i in options.keys():
		file.write("%s " % i)
	file.write("\n");
#	file.write("CPUFLAGS := ");
#	for i in options.keys():
#		o = options[i]
#		file.write("-D%s=%s " % (i, o.getvalue()))
#	file.write("\n");
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

	for i in userdefines:
		file.write("%s\n" %i)
	file.write("\n");

	# print out all the object dependencies
	file.write("\n# object dependencies (objectrules:)\n")
	file.write("OBJECTS :=\n")
	file.write("DRIVERS :=\n")
	for objrule in objectrules.keys():
		obj = objectrules[objrule]
		obj_name = obj[0]
		obj_source = obj[1]
		file.write("OBJECTS-1 += %s\n" % (obj_name))

	for driverrule in driverrules.keys():
		driver = driverrules[driverrule]
		obj_name = driver[0]
		obj_source = driver[1]
		file.write("DRIVER += %s\n" % (obj_name))

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



		
	file.write("\nSOURCES=\n")
	#for source in sources:
		#file.write("SOURCES += %s\n" % source)
	
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

	keys = makeoptions.keys()
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
	file.close();

# Write out crt0_includes.h (top-level assembly language) include file.
def writecrt0_includes(path):
	crt0filepath = os.path.join(path, "crt0_includes.h")
	print "Creating", crt0filepath
	file = open(crt0filepath, 'w+')

	for i in crt0includes:
		file.write("#include <%s>\n" % i)

	file.close();


%%
parser Config:
    ignore:      r'\s+'
    ignore:      "#.*?\r?\n"
    token NUM:   r'[0-9]+'
    token XNUM:   r'0x[0-9a-fA-F]+'
# Why is path separate? Because paths to resources have to at least
# have a slash, we thinks
    token PATH:    r'[a-zA-Z0-9_.][a-zA-Z0-9/_.]+[a-zA-Z0-9_.]+'
# Dir's on the other hand are abitrary
# this may all be stupid.
    token DIRPATH:    r'[a-zA-Z0-9_$()./]+'
    token ID:    r'[a-zA-Z_.]+[a-zA-Z0-9_.]*'
    token STR:   r'"([^\\"]+|\\.)*"'
    token RAWTEXT: r'.*'
    token OPTION: 'option'
    token MAINBOARD: 'mainboard'
    token MAINBOARDINIT: 'mainboardinit'
    token EQ: '='
    token END: '$|end'
    token TARGET: 'target'
    token OBJECT: 'object'
    token DRIVER: 'driver'
    token NORTHBRIDGE: 'northbridge'
    token SOUTHBRIDGE: 'southbridge'
    token SUPERIO: 'superio'
    token IF: 'if'
    token MAKERULE: 'makerule'
    token DEP: 'dep'
    token ACT: 'act'
    token MAKEDEFINE: 'makedefine'
    token ADDACTION: 'addaction'
    token DEFAULT: 'default'
    token INIT: 'init'
    token REGISTER: 'register'
    token CPU: 'cpu'
    token ARCH: 'arch'
    token DIR: 'dir'
    token LDSCRIPT: 'ldscript'
    token PAYLOAD: 'payload'

    # An expression is the sum and difference of factors
    rule expr<<V>>:   factor<<V>>         {{ n = factor }}
                     ( "[+]" factor<<V>>  {{ n = n+factor }}
                     |  "-"  factor<<V>>  {{ n = n-factor }}
                     )*                   {{ return n }}

    # A factor is the product and division of terms
    rule factor<<V>>: term<<V>>           {{ v = term }}
                     ( "[*]" term<<V>>    {{ v = v*term }}
                     |  "/"  term<<V>>    {{ v = v/term }}
                     |  "<<"  term<<V>>    {{ v = v << term }}
                     |  ">=" term<<V>>    {{ v = (v < term)}}
                     )*                   {{ return v }}

    rule unop<<V>>: "!" ID                     {{ return ternary(getoption(ID), 1, 0)}}
    # A term is a number, variable, or an expression surrounded by parentheses
    rule term<<V>>:
                 NUM                      {{ return atoi(NUM) }}
               | XNUM                      {{ return tohex(XNUM) }}
               | ID                      {{ return tohex(getoption(ID)) }}
               | unop<<V>>                 {{ return unop }}
               | "\\(" expr<<V>> "\\)"         {{ return expr }}

    rule option<<C>>: OPTION ID EQ 
				(
			STR {{ if (C): setoption(ID, dequote(STR)) }} 
			|	term<<[]>> {{ if (C): setoption(ID, term) }}
			)
    rule default<<C>>: DEFAULT ID EQ RAWTEXT {{ if (C): setdefault(ID, RAWTEXT) }}

    rule partend<<C>>: partstmts<<C>> END  
    rule mainboard: MAINBOARD PATH {{mainboard(PATH) }} 
				partend<<1>>
			
    rule northbridge<<C>>: NORTHBRIDGE PATH 
			{{if (C): part('northbridge', PATH, 'Config.lb')}} partend<<C>>
    rule superio<<C>>: SUPERIO PATH 
			{{if (C): part('superio', PATH, 'Config.lb')}} partend<<C>>
    rule cpu<<C>>: CPU ID 
			{{if (C): part('cpu', ID, 'Config.lb')}} partend<<C>>
    rule arch<<C>>: ARCH ID 
			{{if (C): set_arch(ID) }} partend<<C>>
    rule southbridge<<C>>: SOUTHBRIDGE PATH 
			{{if (C): part('southbridge', PATH, 'Config.lb')}} partend<<C>>

    rule mainboardinit<<C>>: 
		 MAINBOARDINIT DIRPATH {{ if (C): addcrt0include(DIRPATH)}}

    rule object<<C>>: OBJECT DIRPATH {{if (C): addobject(DIRPATH)}}
    rule driver<<C>>: DRIVER DIRPATH {{if (C): adddriver(DIRPATH)}}
    rule dir<<C>>:	DIR DIRPATH  {{if (C): dodir(DIRPATH, 'Config.lb') }}

    rule ldscript<<C>>: LDSCRIPT DIRPATH {{if (C): addldscript(DIRPATH) }}
    rule payload<<C>>: PAYLOAD DIRPATH {{if (C): payload(DIRPATH) }}
# if is a bad id ....
    rule iif<<C>>:
# needs to be C and ID, but nested if's are, we hope, not going to 
# happen. IF so, possibly ID && C could be used.
		IF ID {{ c = tohex(getoption(ID)); print "IF %d\n" % c }} (stmt<<c>>)* END

    rule depsacts<<ID, C>>: (
			DEP  STR {{ if (C): adddep(ID, STR) }}
			| ACT STR {{ if (C): addaction(ID, STR) }}
			)*
    rule makerule<<C>>: MAKERULE DIRPATH {{ if (C): addrule(DIRPATH) }} 
			depsacts<<DIRPATH, C>> 
    rule makedefine<<C>>: MAKEDEFINE RAWTEXT
			 {{ if (C): adduserdefine(RAWTEXT) }}
    rule addaction<<C>>: ADDACTION ID STR
			 {{ if (C): addaction(ID, STR) }}
    rule init<<C>>:   INIT  STR  {{ if (C): curpart.addinit(STR) }}
    rule register<<C>>:   REGISTER  STR  {{ if (C): curpart.addregister(STR) }}

# to make if work without 2 passses, we use an old hack from SIMD, the 
# context bit. If the bit is 1, then ops get done, otherwise
# ops don't get done. From the top level, context is always
# 1. In an if, context depends on eval of the if condition
    rule stmt<<C>>:   
		  option<<C>>		  {{ return option }}
		| default<<C>>	  {{ return default }}
        	| cpu<<C>>          {{ return cpu}}
        	| arch<<C>>          {{ return arch}}
		| northbridge<<C>>	  {{ return northbridge }}
		| southbridge<<C>>	  {{ return southbridge }}
		| superio<<C>>	  {{ return superio }}
		| object<<C>>	  {{ return object }}
		| driver<<C>>	  {{ return driver }}
		| mainboardinit<<C>>   {{ return mainboardinit }}
		| makerule<<C>> 	  {{ return makerule }}
		| makedefine<<C>> 	  {{ return makedefine }}
		| addaction<<C>> 	  {{ return addaction }}
		| init<<C>> 		  {{ return init }}
		| register<<C>> 		  {{ return register}}
		| iif<<C>>	  {{ return iif }}
		| dir<<C>>		{{ return dir}}
		| ldscript<<C>>		{{ return ldscript}}
		| payload<<C>>		{{ return payload}}

    rule stmts<<C>>: (stmt<<C>>)* 		{{  }}

    rule partstmts<<C>>: (stmt<<C>>)* 		{{  partpop()}}
# need this to get from python to the rules, I think.

    rule pstmts: stmts<<1>> 			{{ }}
    rule target: TARGET DIRPATH            {{target(DIRPATH)}}
    rule board: target mainboard   {{ }}
%%

def dumptree(part, lvl):
	print "DUMPTREE ME is"
	part.dumpme(lvl)
	# dump the siblings -- actually are there any? not sure
	# dump the kids
	print "DUMPTREE KIDS are"
	for kid in part.children:
		dumptree(kid, lvl+1)
	print "DONE DUMPTREE"

def gencode(part):
	print "GENCODE ME is"
	part.gencode()
	# dump the siblings -- actually are there any? not sure
	# dump the kids
	print "GENCODE KIDS are"
	for kid in part.children:
		gencode(kid)
	print "DONE GENCODE"
	

def doconfigfile(path, file):
	if (debug):
		print "DOCONFIGFILE", path, " ", file
	filename = os.path.join(path, file)
	loc.push_file(filename);
	parse('pstmts', open(filename, 'r').read())

if __name__=='__main__':
    from sys import argv
    if (len(argv) < 3):
	print 'Args: <file> <path to linuxbios>'

    top_config_file = os.path.abspath(sys.argv[1])

    treetop = os.path.abspath(sys.argv[2])

    # Set the default locations for config files.
    makebase = os.path.join(treetop, "util/config/make.base")
    crt0base = os.path.join(treetop, "arch/i386/config/crt0.base")
    doxyscriptbase = os.path.join(treetop, "src/config/doxyscript.base")
    
    # Now read in the customizing script...
    loc.push_file(argv[1]);
    parse('board', open(argv[1], 'r').read())
    dumptree(curpart, 0)
    gencode(curpart)
    for i in options.keys():
	o = options[i]
	print "key %s @%s: val %s" % (i, o.where(), o.getvalue())
    # crt0 includes
    for i in crt0includes:
    	print "crt0include file %s \n" % (i)
    for i in driverrules.keys():
    	print "driver file %s \n" % (i)
    for i in ldscripts:
	print "ldscript file %s\n" % (i)
    for i in makebaserules.keys():
	m = makebaserules[i]
	print " makerule %s dep %s act %s\n" % (i, m.dependency, m.actions)
    writemakefilesettings(target_dir)
    writecrt0_includes(target_dir)
    writemakefile(target_dir)
