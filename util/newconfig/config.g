import sys
import os
import re
import string

debug = 0
warnings = 0
errors = 0

target_dir = ''
target_name = ''
treetop = ''
global_options = {}
global_options_by_order = []
global_option_values = {}
global_uses_options = {}
romimages = {}
buildroms = []
curimage = 0
curpart = 0
curdir = '' 
dirstack = []
alloptions = 0 # override uses at top level

local_path = re.compile(r'^\.')
include_pattern = re.compile(r'%%([^%]+)%%')

# -----------------------------------------------------------------------------
#                    Error Handling
# -----------------------------------------------------------------------------

# Used to keep track of our current location while parsing
# configuration files
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

# Print error message
def error(string):      
        global errors, loc
	errors = errors + 1
        size = len(loc.stack)
        i = 0
        while(i < size -1): 
                print loc.stack[i].at()
                i = i + 1
        print "%s: %s"% (loc.at(), string)

# Print error message and exit
def fatal(string):      
	error(string)
        exitiferrors()

# Print warning message
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

# Exit parser if an error has been encountered
def exitiferrors():
	if (errors != 0):
		sys.exit(1)

# -----------------------------------------------------------------------------
#                    Main classes
# -----------------------------------------------------------------------------

# A rom image is the ultimate goal of linuxbios
class romimage:
	def __init__ (self, name):
		self.name = name
		self.arch = ''
		self.payload = ''
		self.initfile = ''
		self.makebaserules = {}
		self.objectrules = {}
		self.initobjectrules = {}
		self.driverrules = {}
		self.ldscripts = []
		self.userdefines = []
		self.initincludes = {}
		self.useinitincludes = 0 # transitional
		self.partinstance = 0
		self.root = 0
		self.target_dir = ''
		self.values = {}

	def getname(self):
		return self.name

	def setarch(self, arch):
		self.arch = arch

	def setpayload(self, payload):
		self.payload = payload

	def setinitfile(self, initfile):
		self.initfile = initfile

	def getinitfile(self):
		return self.initfile

	def addmakerule(self, id):
		o = getdict(self.makebaserules, id)
		if (o):
			print "Warning, rule %s previously defined" % id
		o = makerule(id)
		setdict(self.makebaserules, id, o)

	def getmakerules(self):
		return self.makebaserules

	def getmakerule(self, id):
		o = getdict(self.makebaserules, id)
		if (o):
			return o
		fatal("No such make rule \"%s\"" % id);

	def addmakeaction(self, id, str):
		o = getdict(self.makebaserules, id)
		if (o):
			a = dequote(str)
			o.addaction(a)
			return
		fatal("No such rule \"%s\" for addmakeaction" % id);

	def addmakedepend(self, id, str):
		o = getdict(self.makebaserules, id)
		if (o):
			a = dequote(str)
			o.adddependency(a)
			return
		fatal("No such rule \"%s\" for addmakedepend" % id);

	# this is called with an an object name. 
	# the easiest thing to do is add this object to the current 
	# component.
	# such kludgery. If the name starts with '.' then make the 
	# dependency be on ./thing.x gag me.
	def addobjectdriver(self, dict, object_name):
		global curdir
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
		l = getdict(dict, base)
		if (l):
			print "Warning, object/driver %s previously defined" % base
		setdict(dict, base, [object, source])

	def addinitobjectrule(self, name):
		self.addobjectdriver(self.initobjectrules, name)

	def addobjectrule(self, name):
		self.addobjectdriver(self.objectrules, name)

	def adddriverrule(self, name):
		self.addobjectdriver(self.driverrules, name)

	def getinitobjectrules(self):
		return self.initobjectrules

	def getinitobjectrule(self, name):
		o = getdict(self.initobjectrules, name)
		if (o):
			return o
		fatal("No such init object rule \"%s\"" % name);

	def getobjectrules(self):
		return self.objectrules

	def getobjectrule(self, name):
		o = getdict(self.objectrules, name)
		if (o):
			return o
		fatal("No such object rule \"%s\"" % name);

	def getdriverrules(self):
		return self.driverrules

	def getdriverrule(self, name):
		o = getdict(self.driverrules, name)
		if (o):
			return o
		fatal("No such driver rule \"%s\"" % name);

	def addldscript(self, path):
		self.ldscripts.append(path)

	def getldscripts(self):
		return self.ldscripts

	def adduserdefine(self, str):
		self.userdefines.append(str)

	def getuserdefines(self):
		return self.userdefines

	def addinitinclude(self, str, path):
		if (str != 0):
			self.useinitincludes = 1

		if (debug > 2):
			print "ADDCRT0: %s -> %s" % str, path
		o = getdict(self.initincludes, path)
		if (o):
			print "Warning, init include for %s previously defined" % path
		o = initinclude(str, path)
		setdict(self.initincludes, path, o)

	def getinitincludes(self):
		return self.initincludes

	def getinitinclude(self, path):
		o = getdict(self.initincludes, path)
		if (o):
			return o
		fatal("No such init include \"%s\"" % path);

	def getincludefilename(self):
		if (self.useinitincludes):
			return "crt0.S"
		else:
			return "crt0_include.h"
	
	def newformat(self):
		return self.useinitincludes

	def setpartinstance(self, val):
		self.partinstance = val

	def getpartinstance(self):
		return self.partinstance

	def setroot(self, part):
		self.root = part

	def getroot(self):
		return self.root

	def settargetdir(self, path):
		self.targetdir = path

	def gettargetdir(self):
		return self.targetdir

	def getvalues(self):
		return self.values

# A buildrom statement
class buildrom:
	def __init__ (self, size, roms):
		self.size = size
		self.roms = roms

# this is called with an an object name. 
# the easiest thing to do is add this object to the current 
# component.
# such kludgery. If the name starts with '.' then make the 
# dependency be on ./thing.x gag me.
def addobjectdriver(dict, object_name):
	global curimage, curdir
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
	l = getdict(dict, base)
	if (l):
		print "Warning, object/driver %s previously defined" % base
	setdict(dict, base, [object, source])

# include file for initialization code
class initinclude:
	def __init__ (self, str, path):
		self.string = str
		self.path = path

	def getstring(self):
		return self.string

	def getpath(self):
		return self.path

# Rule to be included in Makefile
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

# Configuration option
class option:
	def __init__ (self, name):
		self.name = name	# name of option
		self.loc = 0		# current location
		#self.value = 0		# option value
		self.set = 0		# option has been set
		self.used = 0		# option has been set
		self.default = 0	# option has default value (otherwise
					# it is undefined)
		self.comment = ''	# description of option
		self.exportable = 0	# option is able to be exported
		self.exported = 0	# option is exported
		self.defined = 0	# option has a value
		self.format = '%s'	# option print format

	def setvalue(self, value, values, loc):
		#if (self.set):
		#	fatal("Error: option %s already set" % self.name)
		self.set = 1
		self.defined = 1
		self.loc = loc
		#self.value = value
		setdict(values, self.name, value)

	def getvalue(self, values):
		#global curpart
		v = getdict(values, self.name)
		if (not (type(v) is str)):
			return v
		if (v == '' or v[0] != '{'):
			return v
		v = parse('delexpr', v)
		# TODO: need to check for parse errors!
		return v

	def setdefault(self, value, loc):
		global global_option_values
		if (self.default):
			fatal("Error: default value for %s already set" % self.name)
		#self.value = value
		setdict(global_option_values, self.name, value)
		self.defined = 1
		self.default = 1
		self.loc = loc

	def setnodefault(self, loc):
		self.default = 1
		self.defined = 0
		self.loc = loc

	def where(self):
		return self.loc

	def setcomment(self, comment, loc):
		if (self.comment != ''):
			print "%s: " % self.name
			print "Attempt to modify comment at %s" % loc 
			return
		self.comment = comment

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

#	def isdefined(self):
#		return (self.defined)

	def isset(self):
		return (self.set)

#	def isused(self):
#		return (self.used)

# A configuration part
class partobj:
	def __init__ (self, image, dir, parent, type, name):
		if (debug):
			print "partobj dir %s parent %s type %s" %(dir,parent,type)
		self.image = image
		self.children = 0
		self.initcode = []
		self.registercode = []
		# sibling is not a list. 
		self.siblings = 0
		self.type = type
		self.objects = []
		self.dir = dir
		self.irq = 0
		self.instance = image.getpartinstance() + 1
		self.flatten_name = flatten_name(type + "/" + name)
		if (debug):
			print "INSTANCE %d" % self.instance
		image.setpartinstance(image.getpartinstance() + 1)
		self.devfn = 0	
		self.private = 0	
		self.uses_options = {}
		# chip initialization. If there is a chip.h in the 
		# directory, generate the structs etc. to 
		# initialize the code
		self.chipconfig = 0
		if (os.path.exists(dir + "/" + "chip.h")): 
			self.chipconfig = 1
		
		if (parent):
			if (debug):
				print "add to parent"
			self.parent   = parent
			# add current child as my sibling, 
			# me as the child.
			if (debug):
				if (parent.children):
					print "add %s (%d) as sibling" % (parent.children.dir, parent.children.instance)
			self.siblings = parent.children
			parent.children = self
		else:
			self.parent = self

	def dumpme(self, lvl):
		print "%d: type %s" % (lvl, self.type)
		print "%d: instance %d" % (lvl, self.instance)
		print "%d: dir %s" % (lvl,self.dir)
		print "%d: flatten_name %s" % (lvl,self.flatten_name)
		print "%d: parent %s" % (lvl,self.parent.type)
		print "%d: parent dir %s" % (lvl,self.parent.dir)
		if (self.children):
			print "%d: child %s" % (lvl, self.children.dir)
		if (self.siblings):
			print "%d: siblings %s" % (lvl, self.siblings.dir)
		print "%d: initcode " % lvl
		for i in self.initcode:
			print "  %s" % i
		print "%d: registercode " % lvl
		for i in self.registercode:
			print "  %s" % i
		print "\n"

	def gencode(self, file):
		if (self.chipconfig):
			file.write("struct %s_config %s_config_%d" % (\
					self.flatten_name ,\
					self.flatten_name , \
					self.instance))
			if (self.registercode):
				file.write("\t= {\n")
				for i in self.registercode:
					file.write( "\t  %s" % i)
				file.write("\t}\n")
			else:
				file.write(";")
			file.write("\n");
		file.write("struct chip dev%d = {\n" % self.instance)
		file.write("/* %s %s */\n" % (self.type, self.dir))
		#file.write("  .devfn = %d,\n" % self.devfn)
		if (self.siblings):
			file.write("  .next = &dev%d,\n" % self.siblings.instance)
		if (self.children):
			file.write("  .children = &dev%d,\n" % \
					self.children.instance)
		if (self.private):
			file.write("  .private = private%d,\n" % self.instance)
		if (self.chipconfig):
			# set the pointer to the structure for all this
			# type of part
			file.write("  .control= &%s_control,\n" % \
					self.flatten_name )
			# generate the pointer to the isntance
			# of the chip struct
			file.write("  .chip_config = (void *) &%s_config_%d,\n" %\
					(self.flatten_name, self.instance ))
		file.write("};\n")
					

		
	def irq(self, irq):
		self.irq = irq
       
    	def addinit(self, code):
        	self.initcode.append(code)
		
    	def addregister(self, code):
		code = dequote(code)
        	self.registercode.append(code)

	def usesoption(self, name):
		global global_options
		o = getdict(global_options, name)
		if (o == 0):
			fatal("Error: can't use undefined option %s" % name)
		o.setused()
		o1 = getdict(self.uses_options, name)
		if (o1):
			return
		setdict(self.uses_options, name, o)

# Used to keep track of the current part
class partsstack:
	def __init__ (self):
		self.stack = []

	def push(self, part):
		self.stack.append(part)

	def pop(self):
		return self.stack.pop()

	def tos(self):
		return self.stack[-1]

	def empty(self):
		return (len(self.stack) == 0)
pstack = partsstack()

# -----------------------------------------------------------------------------
#                    statements 
# -----------------------------------------------------------------------------

def getdict(dict, name):
	if name not in dict.keys(): 
		if (debug >1):
			print 'Undefined:', name
		return 0
	v = dict.get(name, 0)
	if (debug > 1):
		print "getdict %s returning %s" % (name, v)
	return v

def setdict(dict, name, value):
    	if (debug > 1):
		print "setdict sets %s to %s" % (name, value)
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
	global global_options, global_options_by_order
	o = getdict(global_options, name)
	if (o):
		print "option %s already defined" % name
		sys.exit(1)
	o = option(name)
	setdict(global_options, name, o)
	global_options_by_order.append(name)

# option must be declared before being used in a part
# if we're not processing a part, then we must
# be at the top level where all options are available
def getoption(name, part):
	global global_uses_options, global_option_values
	if (part):
		o = getdict(part.uses_options, name)
	elif (alloptions):
		o = getdict(global_options, name)
	else:
		o = getdict(global_uses_options, name)
	if (o == 0 or not o.defined):
		error("Error: Option %s undefined (missing use command?)." % name)
		return
	v = 0
	if (part):
		v = o.getvalue(part.image.getvalues())
	if (v == 0):
		v = o.getvalue(global_option_values)
	return v

def setoption(name, value):
	global loc, global_options, global_option_values, curimage
	o = getdict(global_options, name)
	if (o == 0):
		fatal("Error: attempt to set nonexistent option %s" % name)
	if (curimage):
		o.setvalue(value, curimage.getvalues(), loc)
	else:
		o.setvalue(value, global_option_values, loc)

def setdefault(name, value):
	global loc, global_options
	o = getdict(global_options, name)
	if (not o):
		return
	if (o.default):
		print "setdefault: attempt to duplicate default for %s" % name
		return
	o.setdefault(value, loc)

def setnodefault(name):
	global loc, global_options
	o = getdict(global_options, name)
	if (not o):
		return
	if (o.default):
		print "setdefault: attempt to duplicate default for %s" % name
		return
	o.setnodefault(loc)

def setcomment(name, value):
	global loc, global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setcomment: %s not here" % name)
	o.setcomment(value, loc)

def setexported(name):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setexported: %s not here" % name)
	o.setexported()

def setnoexport(name):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setnoexport: %s not here" % name)
	o.setnoexport()

def setexportable(name):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setexportable: %s not here" % name)
	o.setexportable()

def setformat(name, fmt):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setformat: %s not here" % name)
	o.setformat(fmt)

def getformated(name, values):
	global global_options, global_option_values
	o = getdict(global_options, name)
	if (o == 0 or not o.defined):
		fatal( "Error: Option %s undefined." % name)
	v = 0
	if (values):
		v = o.getvalue(values)
	if (v == 0):
		v = o.getvalue(global_option_values)
	f = o.getformat()
	return (f % v)

def isexported(name):
	global global_options
	o = getdict(global_options, name)
	if (o):
		return o.isexported()
	return 0

#def isdefined(name, part):
#	global global_options
#	if (part):
#		o = getdict(part.uses_options, name)
#	else:
#		o = getdict(global_options, name)
#	if (o):
#		return o.isdefined()
#	return 0

def isset(name, part):
	global global_uses_options
	if (part):
		o = getdict(part.uses_options, name)
	else:
		o = getdict(global_uses_options, name)
	if (o):
		return o.isset()
	return 0

#def isused(name, part):
#	global global_options
#	if (part):
#		o = getdict(part.uses_options, name)
#	else:
#		o = getdict(global_options, name)
#	if (o):
#		return o.isused()
#	return 0

def usesoption(name):
	global curpart, global_options, global_uses_options
	if (curpart):
		curpart.usesoption(name)
		return
	o = getdict(global_options, name)
	if (o == 0):
		fatal("Error: can't use undefined option %s" % name)
	o.setused()
	o1 = getdict(global_uses_options, name)
	if (o1):
		return
	setdict(global_uses_options, name, o)

def validdef(name, defval):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("validdef: %s not here" % name)
	if ((defval & 1) != 1):
	    fatal("Error: must specify default value for option %s" % name)
	if ((defval & 2) != 2):
	    fatal("Error: must specify export for option %s" % name)
	if ((defval & 4) != 4):
	    fatal("Error: must specify comment for option %s" % name)

def loadoptions():
	optionsfile = os.path.join(treetop, 'src', 'config', 'Options.lb')
	loc.push_file(optionsfile)
	if (not parse('options', open(optionsfile, 'r').read())):
		fatal("Error: Could not parse file")
	loc.pop_file()

def addinit(path):
	global curimage, curdir
	if (path[0] == '/'):
		curimage.setinitfile(treetop + '/src/' + path)
	else:
		curimage.setinitfile(curdir + '/' + path)
	print "Adding init file: %s" % path

# we do the crt0include as a dictionary, so that if needed we
# can trace who added what when. Also it makes the keys
# nice and unique. 
def addcrt0include(path):
	global curimage
	curimage.addinitinclude(0, path)

def addinitinclude(str, path):
	global curimage
	curimage.addinitinclude(dequote(str), path)

def addldscript(path):
	global curimage, curdir
	if (path[0] == '/'):
		fullpath =  treetop + '/src/' + path
	else:
		fullpath = curdir + '/' + path
	if (debug):
		print "fullpath :%s: curdir :%s: path :%s:" % (fullpath, curdir, path)
	curimage.addldscript(fullpath)

def payload(path):
	global curimage
	adduserdefine("PAYLOAD:=%s"%path)
	curimage.addpayload(path)
#	addrule('payload')
#	adddep('payload', path)
#	addaction('payload', 'cp $< $@')

def addromimage(name):
	global romimages, curimage, curpart, target_dir, target_name
	print "Configuring ROMIMAGE %s" % name
	curimage = romimage(name)
	curimage.settargetdir(os.path.join(target_dir, name))
	o = partobj(curimage, target_dir, 0, 'board', target_name)
	curimage.setroot(o)
	curpart = o
	o = getdict(romimages, name)
	if (o):
		fatal("romimage %s previously defined" % name)
	setdict(romimages, name, curimage)

def addbuildrom(size, roms):
	global buildroms
	print "Build ROM size %d" % size
	b = buildrom(size, roms)
	buildroms.append(b)

def addinitobject(object_name):
	global curimage
	curimage.addinitobjectrule(object_name)

def addobject(object_name):
	global curimage
	curimage.addobjectrule(object_name)

def adddriver(driver_name):
	global curimage
	curimage.adddriverrule(driver_name)

def target(name):
        global target_dir, target_name
	print "Configuring TARGET %s" % name
	target_name = name
	target_dir = os.path.join(os.path.dirname(loc.file()), name)
	if not os.path.isdir(target_dir):
		print "Creating directory %s" % target_dir
		os.makedirs(target_dir)
	print "Will place Makefile, crt0.S, etc. in %s" % target_dir


def part(name, path, file):
	global curimage, curpart, curdir, dirstack, pstack
	dirstack.append(curdir)
	curdir = os.path.join(treetop, 'src', name, path)
	newpart = partobj(curimage, curdir, curpart, name, path)
	print "Configuring PART %s, path %s" % (name,path)
	if (debug):
		print "PUSH part %s %s" % (name, curdir)
	pstack.push(curpart)
	curpart = newpart
	doconfigfile(curdir, file)

def partpop():
	global curpart, curdir, dirstack, pstack
	print "End PART %s" % curpart.type
	# Warn if options are used without being set in this part
	for i in curpart.uses_options.keys():
		if (not isset(i, curpart)):
			print "WARNING: Option %s using default value %s" % (i, getformated(i, curpart.image.getvalues()))
	curpart = pstack.pop()
	if pstack.empty():
		curpart = 0
	curdir = dirstack.pop()

# dodir is like part but there is no new part
def dodir(path, file):
	global curimage, curpart, curdir, dirstack
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
	doconfigfile(fullpath, file)
	curdir = dirstack.pop()

def lookup(name):
	global curimage, curpart
	if (curpart):
		v = getoption(name, curpart)
	else:
		v = getoption(name, 0)
	exitiferrors()
	return v

def addrule(id):
	global curimage
	curimage.addmakerule(id)
	
def adduserdefine(str):
	global curimage
	curimage.adduserdefine(str)

def addaction(id, str):
	global curimage
	curimage.addmakeaction(id, str)

def adddep(id, str):
	global curimage
	curimage.addmakedepend(id, str)

# arch is 'different' ... darn it.
def set_arch(my_arch):
	global curimage
	curimage.setarch(my_arch)
	setoption('ARCH', my_arch)
	part('arch', my_arch, 'Config.lb')

def mainboard(path):
	full_path = os.path.join(treetop, 'src/mainboard', path)
	setoption('MAINBOARD', full_path)
        vendor = re.sub("/.*", "", path)
        part_number = re.sub("[^/]/", "", path)
	setoption('MAINBOARD_VENDOR', vendor)
	setoption('MAINBOARD_PART_NUMBER', part_number)
	dodir('/config', 'Config.lb')
	part('mainboard', path, 'Config.lb')

def doconfigfile(path, file):
	filename = os.path.join(path, file)
	loc.push_file(filename)
	if (not parse('cfgfile', open(filename, 'r').read())):
		fatal("Error: Could not parse file")
	exitiferrors()

#=============================================================================
#		MISC FUNCTIONS
#=============================================================================
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

def dequote(str):
	a = re.sub("^\"", "", str)
	a = re.sub("\"$", "", a)
	# highly un-intuitive, need four \!
	a = re.sub("\\\\\"", "\"", a)
	return a

def flatten_name(str):
	a = re.sub("/", "_", str)
	return a

# If the first part of <path> matches treetop, replace that part with "$(TOP)"
def topify(path):
	if path[0:len(treetop)] == treetop:
		path = path[len(treetop):len(path)]
		if (path[0:1] == "/"):
			path = path[1:len(path)]
		path = "$(TOP)/" + path
	return path

%%
parser Config:
    ignore:			r'\s+'
    ignore:			"#.*?\r?\n"

    # less general tokens should come first, otherwise they get matched
    # by the re's
    token ACTION:		'action'
    token ADDACTION:		'addaction'
    token ALWAYS:		'always'
    token ARCH:			'arch'
    token BUILDROM:		'buildrom'
    token COMMENT:		'comment'
    token CPU:			'cpu'
    token DEFAULT:		'default'
    token DEFINE:		'define'
    token DEPENDS:		'depends'
    token DIR:			'dir'
    token DRIVER:		'driver'
    token ELSE:			'else'
    token END:			'end'
    token EOF:			'$'
    token EQ:			'='
    token EXPORT:		'export'
    token FORMAT:		'format'
    token IF:			'if'
    token INIT:			'init'
    token INITOBJECT:		'initobject'
    token INITINCLUDE:		'initinclude'
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
    token ROMIMAGE:		'romimage'
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
    token DIRPATH:		r'[-a-zA-Z0-9_$()./]+'
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

    rule partend<<C>>:	(stmt<<C>>)* END 	{{ if (C): partpop()}}

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

    rule initinclude<<C>>: 
			INITINCLUDE 
			STR 
			DIRPATH			{{ if (C): addinitinclude(STR, DIRPATH)}}

    rule initobject<<C>>:
			INITOBJECT DIRPATH	{{ if (C): addinitobject(DIRPATH)}}

    rule object<<C>>:	OBJECT DIRPATH		{{ if (C): addobject(DIRPATH)}}

    rule driver<<C>>:	DRIVER DIRPATH		{{ if (C): adddriver(DIRPATH)}}

    rule dir<<C>>:	DIR DIRPATH 		{{ if (C): dodir(DIRPATH, 'Config.lb') }}

    rule ldscript<<C>>:	LDSCRIPT DIRPATH	{{ if (C): addldscript(DIRPATH) }}

    rule payload<<C>>:	PAYLOAD DIRPATH		{{ if (C): payload(DIRPATH) }}

# if is a bad id ....
# needs to be C and ID, but nested if's are, we hope, not going to 
# happen. IF so, possibly ID && C could be used.
    rule iif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(stmt<<C and c>>)* 
			[ ELSE (stmt<<C and not c>>)* ]
			END

    rule depsacts<<ID, C>>:
			( DEPENDS STR		{{ if (C): adddep(ID, STR) }}
			| ACTION STR		{{ if (C): addaction(ID, STR) }}
			)*

    rule makerule<<C>>:	MAKERULE DIRPATH	{{ if (C): addrule(DIRPATH) }} 
			depsacts<<DIRPATH, C>> 
			END

    rule makedefine<<C>>:
			MAKEDEFINE RAWTEXT	{{ if (C): adduserdefine(RAWTEXT) }}

    rule addaction<<C>>:
			ADDACTION ID STR	{{ if (C): addaction(ID, STR) }}

    rule init<<C>>:	INIT DIRPATH		{{ if (C): addinit(DIRPATH) }}

    rule register<<C>>:	REGISTER STR		{{ if (C): curpart.addregister(STR) }}

    rule prtval:	expr			{{ return str(expr) }}
		|	STR			{{ return STR }}

    rule prtlist:	prtval			{{ el = "%(" + prtval }}
			( "," prtval 		{{ el = el + "," + prtval }}
			)*			{{ return el + ")" }}	

    rule prtstmt<<C>>:	PRINT STR 		{{ val = STR }}
			[ "," prtlist 		{{ val = val + prtlist }}
			]			{{ if (C): print eval(val) }}

# to make if work without 2 passses, we use an old hack from SIMD, the 
# context bit. If the bit is 1, then ops get done, otherwise
# ops don't get done. From the top level, context is always
# 1. In an if, context depends on eval of the if condition
    rule stmt<<C>>:	addaction<<C>>		{{ return addaction }}
		|	arch<<C>>		{{ return arch}}
    		|	cpu<<C>>		{{ return cpu}}
		|	dir<<C>>		{{ return dir}}
		|	driver<<C>>		{{ return driver }}
		|	iif<<C>>		{{ return iif }}
		|	init<<C>>	 	{{ return init }}
		|	initinclude<<C>>	{{ return initinclude }}
		|	initobject<<C>>		{{ return initobject }}
		|	ldscript<<C>>		{{ return ldscript}}
		|	mainboardinit<<C>>	{{ return mainboardinit }}
		|	makedefine<<C>> 	{{ return makedefine }}
		|	makerule<<C>>		{{ return makerule }}
		|	northbridge<<C>>	{{ return northbridge }}
		|	object<<C>>		{{ return object }}
		|	payload<<C>>		{{ return payload}}
		|	pmc<<C>>		{{ return pmc}}
		| 	prtstmt<<C>>		{{ return prtstmt}}
		|	register<<C>> 		{{ return register}}
		|	southbridge<<C>>	{{ return southbridge }}
		|	superio<<C>>		{{ return superio }}

    # ENTRY for parsing Config.lb file
    rule cfgfile:	(uses<<1>>)* 
    			(stmt<<1>>)*
			EOF			{{ return 1 }}

    rule usesid<<C>>:	ID			{{ if (C): usesoption(ID) }}

    rule uses<<C>>:	USES (usesid<<C>>)+

    rule value:		STR			{{ return dequote(STR) }} 
		| 	expr			{{ return expr }}
		|	DELEXPR			{{ return DELEXPR }}

    rule option<<C>>:	OPTION ID EQ value	{{ if (C): setoption(ID, value) }}

    rule opif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(opstmt<<C and c>>)* 
			[ ELSE (opstmt<<C and not c>>)* ] 
			END

    rule opstmt<<C>>:	option<<C>>
		|	opif<<C>>
		|	prtstmt<<C>>


    rule romimage:	ROMIMAGE STR		{{ addromimage(dequote(STR)) }}
			(option<<1>>)*
			MAINBOARD PATH		{{ mainboard(PATH) }}
			END			{{ partpop(); print "End ROMIMAGE" }}

    rule roms:		STR			{{ s = '(' + STR }}
			( STR			{{ s = s + "," + STR }}
			)*			{{ return eval(s + ')') }}

    rule buildrom:	BUILDROM expr roms	{{ addbuildrom(expr, roms) }}

    rule romstmts:	(romimage)* 
			buildrom

    # ENTRY for parsing root part
    rule board:		LOADOPTIONS		{{ loadoptions() }}
	    		TARGET DIRPATH		{{ target(DIRPATH) }}
			(uses<<1>>)*
			(opstmt<<1>>)*
			(romstmts)*		
			EOF			{{ return 1 }}

    # ENTRY for parsing a delayed value
    rule delexpr:	"{" expr "}" EOF	{{ return expr }}

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
    rule options:	(define)* EOF		{{ return 1 }}
%%

#=============================================================================
#		FILE OUTPUT 
#=============================================================================
def writemakefileheader(file, fname):
	file.write("# File: %s\n" % fname)
	file.write("# This file was generated by '%s %s %s'\n\n"
		% (sys.argv[0], sys.argv[1], sys.argv[2]))

def writemakefilesettings(path):
	global treetop, target_dir
	# Write Makefile.settings to seperate the settings
	# from the actual makefile creation
	# In practice you need to rerun NLBConfig.py to change
	# these but in theory you shouldn't need to.

	filename = os.path.join(path, "Makefile.settings")
	print "Creating", filename
	file = open(filename, 'w+')
	writemakefileheader(file, filename)
	file.write("TOP:=%s\n" % (treetop))
	file.write("TARGET_DIR:=%s\n" % target_dir)
#	for i in global_options_by_order:
#		if (isexported(i, 0)):
#			file.write("export %s:=%s\n" % (i, getformated(i, 0)))
#	file.write("export VARIABLES := ")
#	for i in global_options.keys():
#		if (isexported(i, 0)):
#			file.write("%s " % i)
	file.write("\n")
	file.close()

def writeimagesettings(image):
	global treetop
	global global_options_by_order
	# Write Makefile.settings to seperate the settings
	# from the actual makefile creation
	# In practice you need to rerun NLBConfig.py to change
	# these but in theory you shouldn't need to.

	filename = os.path.join(image.gettargetdir(), "Makefile.settings")
	print "Creating", filename
	file = open(filename, 'w+')
	writemakefileheader(file, filename)
	file.write("TOP:=%s\n" % (treetop))
	file.write("TARGET_DIR:=%s\n" % (image.gettargetdir()))
	for i in global_options_by_order:
		if (isexported(i)):
			file.write("export %s:=%s\n" % (i, getformated(i, image.getvalues())))
	file.write("export VARIABLES := ")
	for i in global_options_by_order:
		if (isexported(i)):
			file.write("%s " % i)
	file.write("\n")
	file.close()

# write the romimage makefile
# let's try the Makefile
# first, dump all the -D stuff

def writeimagemakefile(image):
	makefilepath = os.path.join(image.gettargetdir(), "Makefile")
	print "Creating", makefilepath
	file = open(makefilepath, 'w+')
	writemakefileheader(file, makefilepath)

	#file.write("include cpuflags\n")
	# Putting "include cpuflags" in the Makefile has the problem that the
	# cpuflags file would be generated _after_ we want to include it.
	# Instead, let make do the work of computing CPUFLAGS:
	file.write("# Get the value of TOP, VARIABLES, and several other variables.\n")
	file.write("include Makefile.settings\n\n")
	file.write("# Function to create an item like -Di586 or -DMAX_CPUS='1' or -Ui686\n")
	file.write("D_item = $(if $(subst undefined,,$(origin $1)),-D$1$(if $($1),='$($1)',),-U$1)\n\n")
	file.write("# Compute the value of CPUFLAGS here during make's first pass.\n")
	file.write("CPUFLAGS := $(foreach _var_,$(VARIABLES),$(call D_item,$(_var_)))\n\n")

	for i in image.getuserdefines():
		file.write("%s\n" %i)
	file.write("\n")

	# main rule
	file.write("all: linuxbios.rom")
	# print out all the object dependencies
	file.write("\n# object dependencies (objectrules:)\n")
	file.write("INIT-OBJECTS :=\n")
	file.write("OBJECTS :=\n")
	file.write("DRIVER :=\n")
	file.write("\nSOURCES :=\n")
	for irule in image.getinitobjectrules().keys():
		init = image.getinitobjectrule(irule)
		i_name = init[0]
		i_source = init[1]
		file.write("INIT-OBJECTS += %s\n" % (i_name))
		file.write("SOURCES += %s\n" % (i_source))

	for objrule in image.getobjectrules().keys():
		obj = image.getobjectrule(objrule)
		obj_name = obj[0]
		obj_source = obj[1]
		file.write("OBJECTS-1 += %s\n" % (obj_name))
		file.write("SOURCES += %s\n" % (obj_source))

	for driverrule in image.getdriverrules().keys():
		driver = image.getdriverrule(driverrule)
		obj_name = driver[0]
		obj_source = driver[1]
		file.write("DRIVER += %s\n" % (obj_name))
		file.write("SOURCES += %s\n" % (obj_source))

	# Print out all ldscript.ld dependencies.
	file.write("\n# ldscript.ld dependencies:\n")
	file.write("LDSUBSCRIPTS-1 := \n" )
	for script in image.getldscripts():
		file.write("LDSUBSCRIPTS-1 += %s\n" % topify(script))

	# Print out the dependencies for crt0_includes.h
	file.write("\n# Dependencies for crt0_includes.h\n")
	file.write("CRT0_INCLUDES:=\n")
	for inc in image.getinitincludes().keys():
		if (local_path.match(inc)):
			file.write("CRT0_INCLUDES += %s\n" % inc)
		else:
			file.write("CRT0_INCLUDES += $(TOP)/src/%s\n" % inc)

	# Print out the user defines.
	file.write("\n# userdefines:\n")
	#for udef in image.userdefines:
		#file.write("%s\n" % udef)
		
	# Print out the base rules.
	# Need to have a rule that counts on 'all'.
	file.write("\n# mainrulelist:")
	#file.write("\nmainrule: %s\n" % image.mainrulelist)

	# Print out any user rules.
	file.write("\n# From makerule or docipl commands:\n")
	# Old way (hash order): for target in makebaserules.keys():
	# New way (config file order):
	#for target in image.makerule_targets:
		#image.makebaserules[target].write(file)

	file.write("\n# objectrules:\n")
	for objrule in image.getobjectrules().keys():
		obj = image.getobjectrule(objrule)
		source = topify(obj[1])
		file.write("%s: %s\n" % (obj[0], source))
		file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	for driverrule in image.getdriverrules().keys():
		driver = image.getdriverrule(driverrule)
		source = topify(driver[1])
		file.write("%s: %s\n" % (driver[0], source))
		file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	# Print out the rules that will make cause the files
	# generated by NLBConfig.py to be remade if any dependencies change.

	file.write("\n# Remember the automatically generated files\n")
	file.write("GENERATED:=\n")
	for genfile in [ 'Makefile',
			'nsuperio.c',
			 'chip.c', 
			'LinuxBIOSDoc.config' ]:
		file.write("GENERATED += %s\n" % genfile)
		file.write("GENERATED += %s\n" % image.getincludefilename())

	#file.write("\n# Remake Makefile (and the other files generated by\n")
	#file.write("# NLBConfig.py) if any config dependencies change.\n")

	#for cfile in image.config_file_list:
	#	file.write("$(GENERATED): %s\n" % topify(cfile))

	#for depfile in [ '%s' % top_config_file,    # This a duplicate, remove?
	#		'$(TOP)/util/config/NLBConfig.py',
	#		'$(TOP)/src/arch/$(ARCH)/config/make.base' ]:
	#	file.write("$(GENERATED): %s\n" % depfile)

	#file.write("$(GENERATED):\n")
	#file.write("\tpython $(TOP)/util/config/NLBConfig.py %s $(TOP)\n"
	#		% top_config_file)

	keys = image.getroot().uses_options.keys()
	keys.sort()
	file.write("\necho:\n")
	for key in keys:
		 file.write("\t@echo %s='$(%s)'\n"% (key,key))

	for i in image.getmakerules().keys():
		m = image.getmakerule(i)
		file.write("%s: " %i)
		for i in m.dependency:
			file.write("%s " % i)
		file.write("\n")
		for i in m.actions:
			file.write("\t%s\n" % i)
	file.close()

#
def writemakefile(path):
	makefilepath = os.path.join(path, "Makefile")
	print "Creating", makefilepath
	file = open(makefilepath, 'w+')
	writemakefileheader(file, makefilepath)

	# main rule
	file.write("all: ")
	for i in romimages.keys():
		file.write("%s-rom " % i)
	file.write("\n\n")
	for i in romimages.keys():
		o = getdict(romimages, i)
		file.write("%s-rom:\n" % o.getname())
		file.write("\tif (cd %s; \\\n" % o.getname())
		file.write("\t\tmake linuxbios.rom)\\\n");
		file.write("\tthen true; else exit 1; fi;\n\n")
	file.close()

def writeinitincludes(image):
	global include_pattern
	filepath = os.path.join(image.gettargetdir(), image.getincludefilename())
	print "Creating", filepath
	outfile = open(filepath, 'w+')
	if (image.newformat()):
		infile = open(image.getinitfile(), 'r')

		line = infile.readline()
		while (line):
			p = include_pattern.match(line)
			if (p):
				for i in image.getinitincludes().keys():
					inc = image.getinitinclude(i)
					if (inc.getstring() == p.group(1)):
						outfile.write("#include \"%s\"\n" % inc.getpath())
			else:
				outfile.write(line);
			line = infile.readline()

		infile.close()
	else:
		for i in image.getinitincludes().keys():
			outfile.write("#include <%s>\n" % i)
	outfile.close()

def writeldoptions(image):
	# Write Makefile.settings to seperate the settings
	# from the actual makefile creation
	# In practice you need to rerun NLBConfig.py to change
	# these but in theory you shouldn't need to.

	filename = os.path.join(image.gettargetdir(), "ldoptions")
	print "Creating", filename
	file = open(filename, 'w+')
	for i in global_options.keys():
		if (isexported(i) and IsInt(getoption(i, 0))):
			file.write("%s = %s;\n" % (i, getformated(i, image.getvalues())))
	file.close()

# Add any run-time checks to verify that parsing the configuration
# was successful
def verifyparse(image):
	if (image.newformat() and image.getinitfile() == ''):
		fatal("An init file must be specified")

def dumptree(part, lvl):
	if (debug):
		print "DUMPTREE ME is"
	part.dumpme(lvl)
	# dump the siblings -- actually are there any? not sure
	# siblings are:
	if (debug):
		print "DUMPTREE SIBLINGS are"
	kid = part.siblings
	while (kid):
		kid.dumpme(lvl)
		kid = kid.siblings
	# dump the kids
	if (debug):
		print "DUMPTREE KIDS are"
	#for kid in part.children:
	if (part.children):
		dumptree(part.children, lvl+1)
	if (debug):
		print "DONE DUMPTREE"

def gencode(part, file):
	if (debug):
		print "GENCODE ME is"
	part.gencode(file)
	# dump the siblings -- actually are there any? not sure
	# dump the kids
	if (debug):
		print "GENCODE SIBLINGS are"
	kid = part.siblings
	while (kid):
		kid.gencode(file)
		kid = kid.siblings
	if (debug):
		print "GENCODE KIDS are"
	#for kid in part.children:
	if (part.children):
		gencode(part.children, file)
	if (debug):
		print "DONE GENCODE"

#=============================================================================
#		MAIN PROGRAM
#=============================================================================
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

	for r in romimages.keys():
		verifyparse(getdict(romimages, r))

	# no longer need to check if an options has been used
	alloptions = 1

	for r in romimages.keys():
		image = getdict(romimages, r)

		if (debug):
			print "DEVICE TREE:"
			dumptree(image.getroot(), 0)

		img_dir = image.gettargetdir()
		if not os.path.isdir(img_dir):
			print "Creating directory %s" % img_dir
			os.makedirs(img_dir)
		filename = os.path.join(img_dir, "chips.c")
		print "Creating", filename
		file = open(filename, 'w+')
		# gen all the forward references

		i = 0
		file.write("struct chip ")
		while (i <= image.getpartinstance()):
			file.write("cdev%d "% i)
			i = i + 1
		file.write(";\n")
		gencode(image.getroot(), file)
		file.close()

		# crt0 includes
		if (debug):
			for i in image.getinitincludes().keys():
				print "crt0include file %s" % (i)
			for i in image.getdriverrules().keys():
				print "driver file %s" % (i)
			for i in image.getldscripts():
				print "ldscript file %s" % (i)
			for i in image.getmakerules().keys():
				m = image.getmakerule(i)
				print " makerule %s dep %s act %s" % (i, m.dependency, m.actions)

		writeimagesettings(image)
		writeinitincludes(image)
		writeimagemakefile(image)
		writeldoptions(image)

	writemakefilesettings(target_dir)
	writemakefile(target_dir)

	sys.exit(0)
