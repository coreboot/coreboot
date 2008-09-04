# -*- python -*-
import sys
import os
import re
import string
import types

import traceback

warnings = 0
errors = 0

target_dir = ''
target_name = ''
treetop = ''
full_mainboard_path = ''
mainboard_path = ''
global_options = {}
global_options_by_order = []
global_option_values = {}
global_uses_options = {}
global_exported_options = []
romimages = {}
buildroms = []
curimage = 0
alloptions = 0 # override uses at top level

local_path = re.compile(r'^\.')
include_pattern = re.compile(r'%%([^%]+)%%')

# the cpu type for this mainboard
cpu_type = 0

# -----------------------------------------------------------------------------
#                    Utility Classes
# -----------------------------------------------------------------------------

class stack:
	"""Used to keep track of the current part or dir"""
	class __stack_iter:
		def __init__ (self, stack):
			self.index = 0
			self.len = len(stack)
			self.stack = stack

		def __iter__ (self):
			return self

		def next (self):
			if (self.index < self.len):
				s = self.stack[self.index]
				self.index = self.index + 1
				return s
			raise StopIteration

	def __init__ (self):
		self.stack = []

	def __len__ (self):
		return len(self.stack)

	def __getitem__ (self, i):
		return self.stack[i]

	def __iter__ (self):
		return self.__stack_iter(self.stack)

	def push(self, part):
		self.stack.append(part)

	def pop(self):
		try:
			return self.stack.pop()
		except IndexError:
			return 0

	def tos(self):
		try:
			return self.stack[-1]
		except IndexError:
			return 0

	def empty(self):
		return (len(self.stack) == 0)
partstack = stack()
dirstack = stack()

class debug_info:
	none = 0
	gencode = 1
	dumptree = 2
	object = 3
	dict = 4
	statement = 5
	dump = 6

	def __init__(self, *level):
		self.__level = level

	def setdebug(self, *level):
		self.__level = level

	def level(self, level):
		return level in self.__level

	def info(self, level, str):
		if level in self.__level:
			print str

global debug
debug = debug_info(debug_info.none)

# -----------------------------------------------------------------------------
#                    Error Handling
# -----------------------------------------------------------------------------

class location:
	"""Used to keep track of our current location while parsing
	configuration files"""
	class __place:
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
                self.stack = stack()

	def __str__ (self):
		s = ''
		for p in self.stack:
			if (s == ''):
				s = p.at()
			else:
				s = s + '\n' + p.at()
		return s

        def file(self):
                return self.stack.tos().file

        def line(self):
                return self.stack.tos().line

        def command(self):
                return self.stack.tos().command

        def push(self, file):
                self.stack.push(self.__place(os.path.normpath(file), 0, ""))

        def pop(self):
                self.stack.pop()

        def next_line(self, command):
                self.stack.tos().next_line(command)

        def at(self):
                return self.stack.tos().at()
loc = location()

def error(string):      
	"""Print error message"""
        global errors, loc
	errors = errors + 1
        print "===> ERROR: %s" % string
        print "%s" % loc

def fatal(string):      
	"""Print error message and exit"""
	error(string)
        exitiferrors()

def warning(string):
	"""Print warning message"""
        global warnings, loc
	warnings = warnings + 1
        print "===> WARNING: %s" % string

def notice(string):
	"""Print notice message"""
	#print "===> NOTE: %s" % string

def exitiferrors():
	"""Exit parser if an error has been encountered"""
	if (errors != 0):
		sys.exit(1)

def safe_open(file, mode):
	try:
		return open(file, mode)
	except IOError:
		fatal("Could not open file \"%s\"" % file)

# -----------------------------------------------------------------------------
#                    Main classes
# -----------------------------------------------------------------------------

class romimage:
	"""A rom image is the ultimate goal of coreboot"""
	def __init__ (self, name):
		# name of this rom image
		self.name = name

		# set by 'arch' directive
		self.arch = ''

		# set by 'payload' directive
		self.payload = ''

		# set by 'init' directive
		self.initfile = ''

		# make rules added by 'makerule' directive
		self.makebaserules = {}

		# object files added by 'object' directive
		self.objectrules = {}

		# init object files added by 'initobject' directive
		self.initobjectrules = {}

		# driver files added by 'driver' directive
		self.driverrules = {}

		# loader scripts added by 'ldscript' directive
		self.ldscripts = []

		# user defines added by 'makedefine' directive
		self.userdefines = []

		# files to be included in crt0.S
		self.initincludes = {}

		# as above, but order is preserved
		self.initincludesorder = []

		# transitional flag to support old crtinclude format
		self.useinitincludes = 0

		# instance counter for parts
		self.partinstance = 0

		# chip config files included by the 'config' directive
		self.configincludes = {}

		# root of part tree
		self.root = 0

		# name of target directory specified by 'target' directive
		self.target_dir = ''

		# option values used in rom image
		self.values = {}

		# exported options
		self.exported_options = []

		# Last device built
		self.last_device = 0

	def getname(self):
		return self.name

	def getvalues(self):
		return self.values

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
			warning("rule %s previously defined" % id)
		o = makerule(id)
		setdict(self.makebaserules, id, o)

	def getmakerules(self):
		return self.makebaserules

	def getmakerule(self, id):
		o = getdict(self.makebaserules, id)
		if (o):
			return o
		fatal("No such make rule \"%s\"" % id)

	def addmakeaction(self, id, str):
		o = getdict(self.makebaserules, id)
		if (o):
			a = dequote(str)
			o.addaction(a)
			return
		fatal("No such rule \"%s\" for addmakeaction" % id)

	def addmakedepend(self, id, str):
		o = getdict(self.makebaserules, id)
		if (o):
			a = dequote(str)
			o.adddependency(a)
			return
		fatal("No such rule \"%s\" for addmakedepend" % id)

	# this is called with an an object name. 
	# the easiest thing to do is add this object to the current 
	# component.
	# such kludgery. If the name starts with '.' then make the 
	# dependency be on ./thing.x gag me.
	def addobjectdriver(self, dict, object_name):
		global dirstack
		suffix = object_name[-2:]
		if (suffix == '.o'):
			suffix = '.c'
		base = object_name[:-2]
		type = object_name[-1:]
		if (object_name[0] == '.'):
			source = base + suffix
		else:
			source = os.path.join(dirstack.tos(), base + suffix)
		object = base + '.o'
		debug.info(debug.object, "add object %s source %s" % (object_name, source))
		l = getdict(dict, base)
		if (l):
			warning("object/driver %s previously defined" % base)
		setdict(dict, base, [object, source, type, base])

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
		fatal("No such init object rule \"%s\"" % name)

	def getobjectrules(self):
		return self.objectrules

	def getobjectrule(self, name):
		o = getdict(self.objectrules, name)
		if (o):
			return o
		fatal("No such object rule \"%s\"" % name)

	def getdriverrules(self):
		return self.driverrules

	def getdriverrule(self, name):
		o = getdict(self.driverrules, name)
		if (o):
			return o
		fatal("No such driver rule \"%s\"" % name)

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

		debug.info(debug.object, "ADDCRT0: %s -> %s" % (str, path))
		o = getdict(self.initincludes, path)
		if (o):
			warning("init include for %s previously defined" % path)
		o = initinclude(str, path)
		setdict(self.initincludes, path, o)
		self.initincludesorder.append(path)

	def getinitincludes(self):
		return self.initincludesorder

	def getinitinclude(self, path):
		o = getdict(self.initincludes, path)
		if (o):
			return o
		fatal("No such init include \"%s\"" % path)

	def addconfiginclude(self, part, path):
		setdict(self.configincludes, part, path)

	def getconfigincludes(self):
		return self.configincludes

	def getincludefilename(self):
		if (self.useinitincludes):
			return "crt0.S"
		else:
			return "crt0_includes.h"
	
	def newformat(self):
		return self.useinitincludes

	def numparts(self):
		return self.partinstance

	def newpartinstance(self):
		i = self.partinstance
		self.partinstance = self.partinstance + 1
		return i

	def setroot(self, part):
		self.root = part

	def getroot(self):
		return self.root

	def settargetdir(self, path):
		self.targetdir = path

	def gettargetdir(self):
		return self.targetdir

class buildrom:
	"""A buildrom statement"""
	def __init__ (self, filename, size, roms):
		self.name = filename
		self.size = size
		self.roms = roms
	
	def __len__ (self):
		return len(self.roms)

	def __getitem__(self,i):
		return self.roms[i]

class initinclude:
	"""include file for initialization code"""
	def __init__ (self, str, path):
		self.string = str
		self.path = path

	def getstring(self):
		return self.string

	def getpath(self):
		return self.path

class makerule:
	"""Rule to be included in Makefile"""
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
	"""Configuration option"""
	def __init__ (self, name):
		self.name = name	# name of option
		self.loc = 0		# current location
		self.used = 0		# option has been used
					# it is undefined)
		self.comment = ''	# description of option
		self.exportable = 0	# option is able to be exported
		self.format = '%s'	# option print format
		self.write = []		# parts that can set this option

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

	def setnoexport(self):
		self.exportable = 0

	def setformat(self, fmt):
		self.format = fmt

	def getformat(self):
		return self.format

	def setused(self):
		if (self.exportable):
			self.exported = 1
		self.used = 1

	def setwrite(self, part):
		self.write.append(part)

	def isexportable(self):
		return self.exportable

	def iswritable(self, part):
		return (part in self.write)

class option_value:
	"""Value of a configuration option. The option has a default
        value which can be changed at any time. Once an option has been
	set the default value is no longer used."""
	def __init__(self, name, prev):
		self.name = name
		self.value = ''
		self.set = 0
		if (prev):
			self.value   = prev.value
			self.set     = prev.set
		

	def setvalue(self, value):
		if ((self.set & 2) == 2):
			warning("Changing option %s" % self.name)
		else:
			self.set |= 2
		self.value = value

	def setdefault(self, value):
		if ((self.set & 1) == 1):
			notice("Changing default value of %s" % self.name)
		
		if ((self.set & 2) == 0):
			self.value = value
			self.set |= 1

	def contents(self):
		return self.value

	def isset(self):
		return (self.set & 2) == 2


class partobj:
	"""A configuration part"""
	def __init__ (self, image, dir, parent, part, type_name, instance_name, chip_or_device):
		debug.info(debug.object, "partobj dir %s parent %s part %s" \
				% (dir, parent, part))

		# romimage that is configuring this part
		self.image = image

		# links for static device tree
		self.children = 0
		self.prev_sibling = 0
		self.next_sibling = 0
		self.prev_device = 0
		self.next_device = 0
		self.chip_or_device = chip_or_device

		# list of init code files
		self.initcode = []

		# initializers for static device tree
		self.registercode = {}

		# part name
		self.part = part

		# type name of this part
		self.type_name = type_name

		# object files needed to build this part
		self.objects = []

		# directory containg part files
		self.dir = dir

		# instance number, used to distinguish anonymous
		# instances of this part
		self.instance = image.newpartinstance()
		debug.info(debug.object, "INSTANCE %d" % self.instance)

		# Options used by this part
		self.uses_options = {}

		# Name of chip config file (0 if not needed)
		self.chipconfig = 0

		# Flag to indicate that we have generated type
		# definitions for this part (only want to do it once)
		self.done_types = 0

		# Path to the device
		self.path = ""

		# Resources of the device
		self.resoruce = ""
		self.resources = 0

		# Enabled state of the device
		self.enabled = 1

		# Flag if I am a dumplicate device
		self.dup = 0

		# If no instance name is supplied then generate
		# a unique name
		if (instance_name == 0):
			self.instance_name = self.type_name + \
					"_dev%d" % self.instance
			self.chipinfo_name = "%s_info_%d" \
					% (self.type_name, self.instance)
		else:
			self.instance_name = instance_name
			self.chipinfo_name = "%s_info_%d" % (self.instance_name, self.instance)

		# Link this part into the device list
		if (self.chip_or_device == 'device'):
			if (image.last_device):
				image.last_device.next_device = self
			self.prev_device = image.last_device
			image.last_device = self

		# Link this part into the tree
		if (parent and (part != 'arch')):
			debug.info(debug.gencode, "add to parent")
			self.parent   = parent
			# add current child as my sibling, 
			# me as the child.
			if (parent.children):
				debug.info(debug.gencode, "add %s (%d) as sibling" % (parent.children.dir, parent.children.instance))
				youngest = parent.children
				while(youngest.next_sibling):
					youngest = youngest.next_sibling
				youngest.next_sibling = self
				self.prev_sibling = youngest
			else:
				parent.children = self
		else:
			self.parent = self


	def info(self):
		return "%s: %s" % (self.part, self.type)
	def type(self):
		return self.chip_or_device

	def readable_name(self):
		name = ""
		name = "%s_%d" % (self.type_name, self.instance)
		if (self.chip_or_device == 'chip'):
			name = "%s %s %s" % (name, self.part, self.dir)
		else:
			name = "%s %s" % (name, self.path)
		return name
			
	def dumpme(self, lvl):
		"""Dump information about this part for debugging"""
		print "%d: %s" % (lvl, self.readable_name())
		print "%d: part %s" % (lvl, self.part)
		print "%d: instance %d" % (lvl, self.instance)
		print "%d: chip_or_device %s"  %  (lvl, self.chip_or_device)
		print "%d: dir %s" % (lvl,self.dir)
		print "%d: type_name %s" % (lvl,self.type_name)
		print "%d: parent: %s" % (lvl, self.parent.readable_name())
		if (self.children):
			print "%d: child %s" % (lvl, self.children.readable_name())
		if (self.next_sibling):
			print "%d: siblings %s" % (lvl, self.next_sibling.readable_name())
		print "%d: initcode " % lvl
		for i in self.initcode:
			print "\t%s" % i
		print "%d: registercode " % lvl
		for f, v in self.registercode.items():
			print "\t%s = %s" % (f, v)
		print "\n"

	def firstchilddevice(self):
		"""Find the first device in the children link."""
		kid = self.children
		while (kid):
			if (kid.chip_or_device == 'device'):
				return kid
			else:
				kid = kid.children
		return 0

	def firstparentdevice(self):
		"""Find the first device in the parent link."""
		parent = self.parent
		while (parent and (parent.parent != parent) and (parent.chip_or_device != 'device')):
			parent = parent.parent
		if ((parent.parent != parent) and (parent.chip_or_device != 'device')):
			parent = 0
		while(parent and (parent.dup == 1)):
			parent = parent.prev_sibling
		if (not parent):
			fatal("Device %s has no device parent; this is a config file error" % self.readable_name())
		return parent

	def firstparentdevicelink(self):
		"""Find the first device in the parent link and record which link it is."""
		link = 0
		parent = self.parent
		while (parent and (parent.parent != parent) and (parent.chip_or_device != 'device')):
			parent = parent.parent
		if ((parent.parent != parent) and (parent.chip_or_device != 'device')):
			parent = 0
		while(parent and (parent.dup == 1)):
			parent = parent.prev_sibling
			link = link + 1
		if (not parent):
			fatal("Device %s has no device parent; this is a config file error" % self.readable_name())
		return link


	def firstparentchip(self):
		"""Find the first chip in the parent link."""
		parent = self.parent
		while (parent):
			if ((parent.parent == parent) or (parent.chip_or_device == 'chip')):
				return parent
			else:
				parent = parent.parent
		fatal("Device %s has no chip parent; this is a config file error" % self.readable_name())

	def firstsiblingdevice(self):
		"""Find the first device in the sibling link."""
		sibling = self.next_sibling
		while(sibling and (sibling.path == self.path)):
			sibling = sibling.next_sibling
		if ((not sibling) and (self.parent.chip_or_device == 'chip')):
			sibling = self.parent.next_sibling
		while(sibling):
			if (sibling.chip_or_device == 'device'):
				return sibling
			else:
				sibling = sibling.children
		return 0

	def gencode(self, file, pass_num):
		"""Generate static initalizer code for this part. Two passes
		are used - the first generates type information, and the second
		generates instance information"""
		if (pass_num == 0):
			if (self.chip_or_device == 'chip'):
				return;
			else:
				if (self.instance):
					file.write("struct device %s;\n" \
						% self.instance_name)
				else:
					file.write("struct device dev_root;\n")
			return
		# This is pass the second, which is pass number 1
		# this is really just a case statement ...

		if (self.chip_or_device == 'chip'):
			if (self.chipconfig):
				debug.info(debug.gencode, "gencode: chipconfig(%d)" % \
					self.instance)
				file.write("struct %s_config %s" % (self.type_name ,\
					self.chipinfo_name))
				if (self.registercode):
					file.write("\t= {\n")
					for f, v in self.registercode.items():
						file.write( "\t.%s = %s,\n" % (f, v))
					file.write("};\n")
				else:
					file.write(";")
				file.write("\n")

			if (self.instance == 0):
				self.instance_name = "dev_root"
				file.write("struct device **last_dev_p = &%s.next;\n" % (self.image.last_device.instance_name))
				file.write("struct device dev_root = {\n")
				file.write("\t.ops = &default_dev_ops_root,\n")
				file.write("\t.bus = &dev_root.link[0],\n")
				file.write("\t.path = { .type = DEVICE_PATH_ROOT },\n")
				file.write("\t.enabled = 1,\n\t.links = 1,\n")
				file.write("\t.on_mainboard = 1,\n")
				file.write("\t.link = {\n\t\t[0] = {\n")
				file.write("\t\t\t.dev=&dev_root,\n\t\t\t.link = 0,\n")
				file.write("\t\t\t.children = &%s,\n" % self.firstchilddevice().instance_name)
				file.write("\t\t},\n")
				file.write("\t},\n")
				if (self.chipconfig):
					file.write("\t.chip_ops = &%s_ops,\n" % self.type_name)
					file.write("\t.chip_info = &%s_info_%s,\n" % (self.type_name, self.instance))
				file.write("\t.next = &%s,\n" % self.firstchilddevice().instance_name)
				file.write("};\n")
			return

		# Don't print duplicate devices, just print their children
		if (self.dup):
			return

		file.write("struct device %s = {\n" % self.instance_name)
		file.write("\t.ops = 0,\n")
		file.write("\t.bus = &%s.link[%d],\n" % \
			(self.firstparentdevice().instance_name, \
			self.firstparentdevicelink()))
		file.write("\t.path = {%s},\n" % self.path)
		file.write("\t.enabled = %d,\n" % self.enabled)
		file.write("\t.on_mainboard = 1,\n")
		if (self.resources):
			file.write("\t.resources = %d,\n" % self.resources)
			file.write("\t.resource = {%s\n\t },\n" % self.resource)
		file.write("\t.link = {\n");	
		links = 0
		bus = self
		while(bus and (bus.path == self.path)):
			child = bus.firstchilddevice()
			if (child or (bus != self) or (bus.next_sibling and (bus.next_sibling.path == self.path))):
				file.write("\t\t[%d] = {\n" % links)
				file.write("\t\t\t.link = %d,\n" % links)
				file.write("\t\t\t.dev = &%s,\n" % self.instance_name)
				if (child):
					file.write("\t\t\t.children = &%s,\n" %child.instance_name)
				file.write("\t\t},\n")
				links = links + 1
			if (1):	
				bus = bus.next_sibling
			else:
				bus = 0
		file.write("\t},\n")
		file.write("\t.links = %d,\n" % (links))
		sibling = self.firstsiblingdevice(); 
		if (sibling):
			file.write("\t.sibling = &%s,\n" % sibling.instance_name)
		chip = self.firstparentchip()
		if (chip and chip.chipconfig):
			file.write("\t.chip_ops = &%s_ops,\n" % chip.type_name)
			file.write("\t.chip_info = &%s_info_%s,\n" % (chip.type_name, chip.instance))
		if (self.next_device):	
			file.write("\t.next=&%s\n" % self.next_device.instance_name)
		file.write("};\n")
		return

    	def addinit(self, code):
		"""Add init file to this part"""
        	self.initcode.append(code)
		
    	def addconfig(self, path):
		"""Add chip config file to this part"""
        	self.chipconfig = os.path.join(self.dir, path)
		self.image.addconfiginclude(self.type_name, self.chipconfig)

    	def addregister(self, field, value):
		"""Register static initialization information"""
		if (self.chip_or_device != 'chip'):
			fatal("Only chips can have register values")
		field = dequote(field)
		value = dequote(value)
        	setdict(self.registercode, field, value)

	def set_enabled(self, enabled):
		self.enabled = enabled

	def start_resources(self):
		self.resource = ""
		self.resources = 0

	def end_resources(self):
		self.resource = "%s" % (self.resource)

	def add_resource(self, type, index, value):
		""" Add a resource to a device """
		self.resource = "%s\n\t\t{ .flags=%s, .index=0x%x, .base=0x%x}," % (self.resource, type, index, value)
		self.resources = self.resources + 1

	def set_path(self, path):
		self.path = path
		if (self.prev_sibling and (self.prev_sibling.path == self.path)):
			self.dup = 1
			if (self.prev_device):
				self.prev_device.next_device = self.next_device
			if (self.next_device):	
				self.next_device.prev_device = self.prev_device
			if (self.image.last_device == self):
				self.image.last_device = self.prev_device
			self.prev_device = 0
			self.next_device = 0
		
	def addpcipath(self, slot, function):
		""" Add a relative pci style path from our parent to this device """
		if ((slot < 0) or (slot > 0x1f)):
			fatal("Invalid device id")
		if ((function < 0) or (function > 7)):
			fatal("Invalid pci function %s" % function )
		self.set_path(".type=DEVICE_PATH_PCI,.u={.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}" % (slot, function))

	def addpnppath(self, port, device):
		""" Add a relative path to a pnp device hanging off our parent """
		if ((port < 0) or (port > 65536)):
			fatal("Invalid port")
		if ((device < 0) or (device > 0xffff)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_PNP,.u={.pnp={ .port = 0x%x, .device = 0x%x }}" % (port, device))
		
	def addi2cpath(self, device):
		""" Add a relative path to a i2c device hanging off our parent """
		if ((device < 0) or (device > 0x7f)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_I2C,.u={.i2c={ .device = 0x%x }}" % (device))

	def addapicpath(self, apic_id):
		""" Add a relative path to a cpu device hanging off our parent """
		if ((apic_id < 0) or (apic_id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_APIC,.u={.apic={ .apic_id = 0x%x }}" % (apic_id))
    
	def addpci_domainpath(self, pci_domain):
		""" Add a pci_domain number to a chip """
		if ((pci_domain < 0) or (pci_domain > 0xffff)):
			fatal("Invalid pci_domain: 0x%x is out of the range 0 to 0xffff" % pci_domain)
		self.set_path(".type=DEVICE_PATH_PCI_DOMAIN,.u={.pci_domain={ .domain = 0x%x }}" % (pci_domain))
    
	def addapic_clusterpath(self, cluster):
		""" Add an apic cluster to a chip """
		if ((cluster < 0) or (cluster > 15)):
			fatal("Invalid apic cluster: %d is out of the range 0 to ff" % cluster)
		self.set_path(".type=DEVICE_PATH_APIC_CLUSTER,.u={.apic_cluster={ .cluster = 0x%x }}" % (cluster))
    
	def addcpupath(self, cpu_id):
		""" Add a relative path to a cpu device hanging off our parent """
		if ((cpu_id < 0) or (cpu_id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_CPU,.u={.cpu={ .id = 0x%x }}" % (cpu_id))
    

	def addcpu_buspath(self, id):
		""" Add a cpu_bus to a chip """
		if ((id < 0) or (id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_CPU_BUS,.u={.cpu_bus={ .id = 0x%x }}" % (id))
    
	def usesoption(self, name):
		"""Declare option that can be used by this part"""
		global global_options
		o = getdict(global_options, name)
		if (o == 0):
			fatal("can't use undefined option %s" % name)
		o1 = getdict(self.uses_options, name)
		if (o1):
			return
		setdict(self.uses_options, name, o)
		exportoption(o, self.image.exported_options)

# -----------------------------------------------------------------------------
#                    statements 
# -----------------------------------------------------------------------------

def getdict(dict, name):
	if name not in dict.keys(): 
		debug.info(debug.dict, "Undefined: %s" % name)
		return 0
	v = dict.get(name, 0)
	debug.info(debug.dict, "getdict %s returning %s" % (name, v))
	return v

def setdict(dict, name, value):
	debug.info(debug.dict, "setdict sets %s to %s" % (name, value))
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
		fatal("option %s already defined" % name)
	o = option(name)
	setdict(global_options, name, o)
	global_options_by_order.append(name)

def newoptionvalue(name, image):
	g = getdict(global_option_values, name)
	v = option_value(name, g)
	if (image):
		setdict(image.getvalues(), name, v)
	else:
		setdict(global_option_values, name, v)
	return v

def getoptionvalue(name, op, image):
	global global_option_values
	#print "getoptionvalue name %s op %s image %s\n" % (name, op,image)
	if (op == 0):
		# we want to debug config files, not the config tool, so no:
		# print_stack() 
		fatal("Option %s undefined (missing use command?)" % name)
	if (image):
		v = getdict(image.getvalues(), name)
	else:
		v = getdict(global_option_values, name)
	return v

def getoption(name, image):
	"""option must be declared before being used in a part
	if we're not processing a part, then we must
	be at the top level where all options are available"""

	global global_uses_options, alloptions, curimage

	#print "getoption: name %s image %s alloptions %s curimage %s\n\n" % (name, image, alloptions, curimage)
	curpart = partstack.tos()
	if (alloptions):
		o = getdict(global_options, name)
	elif (curpart):
		o = getdict(curpart.uses_options, name)
		if (o == 0):
			print "curpart.uses_options is %s\n" % curpart.uses_options
	else:
		o = getdict(global_uses_options, name)
	v = getoptionvalue(name, o, image)
	if (v == 0):
		v = getoptionvalue(name, o, 0)
	if (v == 0):
		fatal("No value for option %s" % name)
	val = v.contents()
	if (not (type(val) is types.StringType)):
		return v.contents()
	if (val == '' or val[0] != '{'):
		return v.contents()
	s = curimage
	curimage = image
	val = parse('delexpr', val)
	curimage = s
	exitiferrors()
	return val

def setoption(name, value, imp):
	"""Set an option from within a configuration file. Normally this
	is only permitted in the target (top level) configuration file.
	If 'imp' is true, then set an option implicitly (e.g. 'arch' 
	and 'mainboard' statements). Implicit options can be set anywhere 
	the statements are legal, but also performs an implicit 'uses' 
	for the option"""

	global loc, global_options, global_option_values, curimage

	curpart = partstack.tos()
	if (not imp and curpart):
		fatal("Options may only be set in target configuration file")
	if (imp):
		usesoption(name)
	if (curpart):
		o = getdict(curpart.uses_options, name)
	else:
		o = getdict(global_uses_options, name)
	if (not o):
		fatal("Attempt to set nonexistent option %s (missing USES?)" % name)
	v = getoptionvalue(name, o, curimage)
	if (v == 0):
		v = newoptionvalue(name, curimage)
	v.setvalue(value)

def exportoption(op, exported_options):
	if (not op.isexportable()):
		return
	if (not op in exported_options):
		exported_options.append(op)

def setdefault(name, value, isdef):
	"""Set the default value of an option from within a configuration 
	file. This is permitted from any configuration file, but will
	result in a warning if the default is set more than once.
	If 'isdef' is set, we're defining the option in Options.lb so
	there is no need for 'uses'."""

	global loc, global_options, curimage

	if (isdef):
		o = getdict(global_options, name)
		if (not o):
			return
		image = 0
	else:
		curpart = partstack.tos()
		if (curpart):
			o = getdict(curpart.uses_options, name)
		else:
			o = getdict(global_uses_options, name)
		if (not o):
			fatal("Attempt to set default for nonexistent option %s (missing USES?)" % name)
		image = curimage

	v = getoptionvalue(name, o, image)
	if (v == 0):
		v = newoptionvalue(name, image)
	v.setdefault(value)

def setnodefault(name):
	global loc, global_options
	o = getdict(global_options, name)
	if (not o):
		return
	v = getdict(global_option_values, name)
	if (v != 0):
		warning("removing default for %s" % name)
		del global_option_values[name]

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
	o.setexportable()
	global_exported_options.append(o)

def setnoexport(name):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setnoexport: %s not here" % name)
	o.setnoexport()
	if (o in global_exported_options):
		global_exported_options.remove(o)

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

def getformated(name, image):
	global global_options, global_option_values
	o = getdict(global_options, name)
	v = getoption(name, image)
	f = o.getformat()
	return (f % v)

def setwrite(name, part):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("setwrite: %s not here" % name)
	o.setwrite(part)

def hasvalue(name, image):
	global global_options
	o = getdict(global_options, name)
	if (o == 0):
		return 0
	v = 0
	if (image):
		v = getdict(image.getvalues(), name)
	if (v == 0):
		v = getdict(global_option_values, name)
	return (v != 0)

def isset(name, part):
	global global_uses_options, global_option_values, curimage
	if (part):
		o = getdict(part.uses_options, name)
	else:
		o = getdict(global_uses_options, name)
	if (o == 0):
		return 0
	v = 0
	if (curimage):
		v = getdict(curimage.getvalues(), name)
	if (v == 0):
		v = getdict(global_option_values, name)
	return (v != 0 and v.isset())

def usesoption(name):
	global global_options, global_uses_options
	curpart = partstack.tos()
	if (curpart):
		curpart.usesoption(name)
		return
	o = getdict(global_options, name)
	if (o == 0):
		fatal("Can't use undefined option %s" % name)
	o1 = getdict(global_uses_options, name)
	if (o1):
		return
	setdict(global_uses_options, name, o)
	exportoption(o, global_exported_options)

def validdef(name, defval):
	global global_options
	o = getdict(global_options, name)
	if (not o):
		fatal("validdef: %s not here" % name)
	if ((defval & 1) != 1):
	    fatal("Must specify default value for option %s" % name)
	if ((defval & 2) != 2):
	    fatal("Must specify export for option %s" % name)
	if ((defval & 4) != 4):
	    fatal("Must specify comment for option %s" % name)

def loadoptions(path, file, rule):
	file = os.path.join('src', path, file)
	optionsfile = os.path.join(treetop, file)
	fp = safe_open(optionsfile, 'r')
	loc.push(file)
	if (not parse(rule, fp.read())):
		fatal("Could not parse file")
	loc.pop()

def addinit(path):
	global curimage, dirstack
	if (path[0] == '/'):
		curimage.setinitfile(treetop + '/src/' + path)
	else:
		curimage.setinitfile(dirstack.tos() + '/' + path)
	print "Adding init file: %s" % path

def addconfig(path):
	global partstack
	curpart = partstack.tos()
	curpart.addconfig(path)

def addregister(field, value):
	global partstack
	curpart = partstack.tos()
	curpart.addregister(field, value)

def addcrt0include(path):
	"""we do the crt0include as a dictionary, so that if needed we
	can trace who added what when. Also it makes the keys
	nice and unique."""
	global curimage
	curimage.addinitinclude(0, path)

def addinitinclude(str, path):
	global curimage
	curimage.addinitinclude(dequote(str), path)

def addldscript(path):
	global curimage, dirstack
	curdir = dirstack.tos()
	if (path[0] == '/'):
		fullpath =  treetop + '/src/' + path
	else:
		fullpath = curdir + '/' + path
	debug.info(debug.statement, "fullpath :%s: curdir :%s: path :%s:" % (fullpath, curdir, path))
	curimage.addldscript(fullpath)

def payload(path):
	global curimage
	curimage.setpayload(path)
	adduserdefine("PAYLOAD:=%s"%path)

def startromimage(name):
	global romimages, curimage, target_dir, target_name
	curpart = partstack.tos()
	print "Configuring ROMIMAGE %s Curimage %s" % (name, curimage)
	print "Curpart is %s\n" % curpart
	o = getdict(romimages, name)
	if (o):
		fatal("romimage %s previously defined" % name)
	curimage = romimage(name)
	curimage.settargetdir(os.path.join(target_dir, name))
	#o = partobj(curimage, target_dir, 0, 'board', target_name)
	#curimage.setroot(o)
	setdict(romimages, name, curimage)
	dodir('/config', 'Config.lb')

def endromimage():
	global curimage
	mainboard()
	print "End ROMIMAGE"
	curimage = 0
	#curpart = 0

def mainboardsetup(path):
	global full_mainboard_path, mainboard_path
	mainboard_path = os.path.join('mainboard', path)
	loadoptions(mainboard_path, 'Options.lb', 'mainboardvariables')
	full_mainboard_path = os.path.join(treetop, 'src', 'mainboard', path)
	vendor = re.sub("/.*", "", path)
        part_number = re.sub("[^/]*/", "", path)
	setdefault('MAINBOARD', full_mainboard_path, 0)
	setdefault('MAINBOARD_VENDOR', vendor, 0)
	setdefault('MAINBOARD_PART_NUMBER', part_number, 0)

def mainboard():
	global curimage, dirstack, partstack
	file = 'Config.lb'
	partdir = mainboard_path
	srcdir = os.path.join(treetop, 'src')
	fulldir = os.path.join(srcdir, partdir)
	type_name = flatten_name(partdir)
	newpart = partobj(curimage, fulldir, partstack.tos(), 'mainboard', \
		type_name, 0, 'chip')
	#print "Configuring PART %s" % (type)
	partstack.push(newpart)
	#print "  new PART tos is now %s\n" %partstack.tos().info()
	dirstack.push(fulldir)
	loadoptions(mainboard_path, 'Options.lb', 'mainboardvariables')
	# special case for 'cpu' parts.
	# we could add a new function too, but this is rather trivial.
	# if the part is a cpu, and we haven't seen it before,
	# arrange to source the directory /cpu/'type'
	doconfigfile(srcdir, partdir, file, 'cfgfile')
	curimage.setroot(partstack.tos())
	partpop()

def addbuildrom(filename, size, roms):
	global buildroms
	print "Build ROM size %d" % size
	b = buildrom(filename, size, roms)
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


def cpudir(path):
	global cpu_type
	if (cpu_type and (cpu_type != path)):
		fatal("Two different CPU types: %s and %s" % (cpu_type, path))
	srcdir = "/cpu/%s" % path
	dodir(srcdir, "Config.lb")
	cpu_type = path
	
def devicepart(type):
	global curimage, dirstack, partstack
	newpart = partobj(curimage, 0, partstack.tos(), type, \
			'', 0, 'device')
	#print "Configuring PART %s" % (type)
	partstack.push(newpart)
	#print "  new PART tos is now %s\n" %partstack.tos().info()
	# just push TOS, so that we can pop later. 
	dirstack.push(dirstack.tos())
	
def part(type, path, file, name):
	global curimage, dirstack, partstack
        partdir = os.path.join(type, path)
	srcdir = os.path.join(treetop, 'src')
	fulldir = os.path.join(srcdir, partdir)
	type_name = flatten_name(partdir)
	newpart = partobj(curimage, fulldir, partstack.tos(), type, \
			type_name, name, 'chip')
	#print "Configuring PART %s, path %s" % (type, path)
	partstack.push(newpart)
	#print "  new PART tos is now %s\n" %partstack.tos().info()
	dirstack.push(fulldir)
	# special case for 'cpu' parts. 
	# we could add a new function too, but this is rather trivial.
	# if the part is a cpu, and we haven't seen it before, 
	# arrange to source the directory /cpu/'type'
	if (type == 'cpu'):
		cpudir(path)
	else:
		doconfigfile(srcdir, partdir, file, 'cfgfile')

def partpop():
	global dirstack, partstack
	curpart = partstack.tos()
	if (curpart == 0):
		fatal("Trying to pop non-existent part")
	#print "End PART %s" % curpart.part
	# Warn if options are used without being set in this part
	for op in curpart.uses_options.keys():
		if (not isset(op, curpart)):
			notice("Option %s using default value %s" % (op, getformated(op, curpart.image)))
	oldpart = partstack.pop()
	dirstack.pop()
	#print "partstack.pop, TOS is now %s\n" % oldpart.info()

def dodir(path, file):
	"""dodir is like part but there is no new part"""
	global dirstack
	# if the first char is '/', it is relative to treetop, 
	# else relative to curdir
	# os.path.join screws up if the name starts with '/', sigh.
	print "Configuring DIR %s" % os.path.join(path, file)
	if (path[0] == '/'):
		fullpath = os.path.join(treetop, 'src')
		path = re.sub('^/*', '', path)
	else:
		fullpath = dirstack.tos()
	debug.info(debug.statement, "DODIR: path %s, fullpath %s" % (path, fullpath))
	dirstack.push(os.path.join(fullpath, path))
	doconfigfile(fullpath, path, file, 'cfgfile')
	dirstack.pop()

def lookup(name):
	global curimage
	return getoption(name, curimage)

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

def setarch(my_arch):
	"""arch is 'different' ... darn it."""
	global curimage
	print "SETTING ARCH %s\n" % my_arch
	curimage.setarch(my_arch)
	setdefault('ARCH', my_arch, 1)
	part('arch', my_arch, 'Config.lb', 0)

def doconfigfile(path, confdir, file, rule):
	rname = os.path.join(confdir, file)
	loc.push(rname)
	fullpath = os.path.join(path, rname)
	fp = safe_open(fullpath, 'r')
	if (not parse(rule, fp.read())):
		fatal("Could not parse file")
	exitiferrors()
	loc.pop()

#=============================================================================
#		MISC FUNCTIONS
#=============================================================================
def ternary(val, yes, no):
	debug.info(debug.statement, "ternary %s" % expr)
	debug.info(debug.statement, "expr %s a %d yes %d no %d"% (expr, a, yes, no))
        if (val == 0):
		debug.info(debug.statement, "Ternary returns %d" % yes)
		return yes
        else:
		debug.info(debug.statement, "Ternary returns %d" % no)
		return no

def tohex(name):
	"""atoi is in the python library, but not strtol? Weird!"""
	return eval('int(%s)' % name)

def IsInt(str):
	""" Is the given string an integer?"""
	try:
		num = long(str)
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
	a = re.sub("[/-]", "_", str)
	return a

def topify(path):
	"""If the first part of <path> matches treetop, replace 
	that part with $(TOP)"""
	if path[0:len(treetop)] == treetop:
		path = path[len(treetop):len(path)]
		if (path[0:1] == "/"):
			path = path[1:len(path)]
		path = "$(TOP)/" + path
	return path

%%
# to make if work without 2 passses, we use an old hack from SIMD, the 
# context bit. If the bit is 1, then ops get done, otherwise
# ops don't get done. From the top level, context is always
# 1. In an if, context depends on eval of the if condition

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
    token CONFIG:		'config'
    token CPU:			'cpu'
    token CPU_BUS:		'cpu_bus'
    token CHIP:			'chip'
    token DEFAULT:		'default'
    token DEFINE:		'define'
    token DEPENDS:		'depends'
    token DEVICE:		'device'
    token DIR:			'dir'
    token DRIVER:		'driver'
    token DRQ:			'drq'
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
    token IO:			'io'
    token IRQ:			'irq'
    token LDSCRIPT:		'ldscript'
    token LOADOPTIONS:		'loadoptions'
    token MAINBOARD:		'mainboard'
    token MAINBOARDINIT:	'mainboardinit'
    token MAKEDEFINE:		'makedefine'
    token MAKERULE:		'makerule'
    token MEM:			'mem'
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
    token WRITE:		'write'
    token NUM:			'[0-9]+'
    token HEX_NUM:		'[0-9a-fA-F]+'
    token HEX_PREFIX:		'0x'
    # Why is path separate? Because paths to resources have to at least
    # have a slash, we thinks
    token PATH:			r'[-a-zA-Z0-9_.][-a-zA-Z0-9/_.]+[-a-zA-Z0-9_.]+'
    # Dir's on the other hand are abitrary
    # this may all be stupid.
    token DIRPATH:		r'[-a-zA-Z0-9_$()./]+'
    token ID:			r'[a-zA-Z_.]+[a-zA-Z0-9_.]*'
    token DELEXPR:		r'{([^}]+|\\.)*}'
    token STR:			r'"([^\\"]+|\\.)*"'
    token RAWTEXT:		r'.*'
    token ON:			'on'
    token OFF:			'off'
    token PCI:			'pci'
    token PNP:			'pnp'
    token I2C:			'i2c'
    token APIC:			'apic'
    token APIC_CLUSTER:		'apic_cluster'
    token CPU:			'cpu'
    token CPU_BUS:		'cpu_bus'
    token PCI_DOMAIN:		'pci_domain'


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
    rule term:		NUM			{{ return long(NUM, 10) }}
		|	HEX_PREFIX HEX_NUM	{{ return long(HEX_NUM, 16) }}
		|	ID			{{ return lookup(ID) }}
		|	unop			{{ return unop }}
		|	"\\(" expr "\\)"	{{ return expr }}

    rule unop:		"!" expr		{{ return not(expr) }}

    rule partend<<C>>:	(stmt<<C>>)* END 	{{ if (C): partpop()}}

    # This is needed because the legacy cpu command could not distinguish
    # between cpu vendors. It should just be PATH, but getting this change
    # into the source tree will be tricky... 
    # DO NOT USE ID AS IT MAY GO AWAY IN THE FUTURE
    rule partid:	ID 			{{ return ID }}
		|	PATH			{{ return PATH }}

#    rule parttype:	NORTHBRIDGE		{{ return 'northbridge' }} 
#    		|	SUPERIO 		{{ return 'superio' }}
#		|	PMC			{{ return 'pmc' }}
#		|	SOUTHBRIDGE		{{ return 'southbridge' }}
#		|	CPU			{{ return 'cpu' }}
#		|	CHIP                    {{ return '' }}
#
    rule parttype:	CHIP                    {{ return '' }}

    rule partdef<<C>>:				{{ name = 0 }} 
			parttype partid		
			[ STR			{{ name = dequote(STR) }}
			]                       {{ if (C): part(parttype, partid, 'Config.lb', name) }}
			partend<<C>> 		

    rule arch<<C>>:	ARCH ID			{{ if (C): setarch(ID) }}
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

    rule default<<C>>:	DEFAULT ID EQ value	{{ if (C): setdefault(ID, value, 0) }}

    rule ldscript<<C>>:	LDSCRIPT DIRPATH	{{ if (C): addldscript(DIRPATH) }}

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

    rule field:		STR			{{ return STR }}

    rule register<<C>>:	REGISTER field '=' STR	{{ if (C): addregister(field, STR) }}

    rule enable<<C>>:				{{ val = 1 }}
	    		( ON 			{{ val = 1 }}
			| OFF			{{ val = 0 }}
			) 			{{ if(C): partstack.tos().set_enabled(val) }}

    rule resource<<C>>:				{{ type = "" }}
	    		(  IO			{{ type = "IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_IO" }}
			|   MEM			{{ type = "IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_MEM" }}
			|   IRQ			{{ type = "IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_IRQ" }}
			|   DRQ			{{ type = "IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_DRQ" }}
			)
			term '='		{{ index = term }}
			term			{{ value = term }}
						{{ if (C): partstack.tos().add_resource(type, index, value) }}
    
			     
    rule resources<<C>>:			{{ if (C): partstack.tos().start_resources() }}
	    		( resource<<C>> )*
						{{ if (C): partstack.tos().end_resources() }}
	    
    
    rule pci<<C>>:	PCI 			{{ if (C): devicepart('pci') }}

    			HEX_NUM			{{ slot = int(HEX_NUM,16) }}
			'.' HEX_NUM		{{ function = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addpcipath(slot, function) }}
    rule pci_domain<<C>>:	
			PCI_DOMAIN 		{{ if (C): devicepart('pci_domain') }}
			HEX_NUM			{{ pci_domain = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addpci_domainpath(pci_domain) }}

    rule pnp<<C>>:	PNP  			{{ if (C): devicepart('pnp') }}
			HEX_NUM			{{ port = int(HEX_NUM,16) }}
			'.' HEX_NUM		{{ device = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addpnppath(port, device) }}
						
    rule i2c<<C>>:	I2C   			{{ if (C): devicepart('i2c') }}
			HEX_NUM			{{ device = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addi2cpath(device) }}

    rule apic<<C>>:	APIC   			{{ if (C): devicepart('apic') }}
			HEX_NUM			{{ apic_id = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addapicpath(apic_id) }}

    rule apic_cluster<<C>>: APIC_CLUSTER 	{{ if (C): devicepart('apic_cluster') }}
			HEX_NUM			{{ cluster = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addapic_clusterpath(cluster) }}

    rule cpu<<C>>:	CPU			{{ if (C): devicepart('cpu') }}
    			HEX_NUM			{{ id = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addcpupath(id) }}

    rule cpu_bus<<C>>:	CPU_BUS			{{ if (C): devicepart('cpu_bus') }}
    			HEX_NUM			{{ id = int(HEX_NUM, 16) }}
						{{ if (C): partstack.tos().addcpu_buspath(id) }}

    rule dev_path<<C>>:
	    		pci<<C>>		{{ return pci }}
		|	pci_domain<<C>>		{{ return pci_domain }}
	    	|	pnp<<C>>		{{ return pnp }}
		|	i2c<<C>>		{{ return i2c }}
		|	apic<<C>>		{{ return apic }}
		|	apic_cluster<<C>>	{{ return apic_cluster }}
		|	cpu<<C>>		{{ return cpu }}
		|	cpu_bus<<C>>		{{ return cpu_bus }}
		
    rule prtval:	expr			{{ return str(expr) }}
		|	STR			{{ return STR }}

    rule prtlist:	prtval			{{ el = "%(" + prtval }}
			( "," prtval 		{{ el = el + "," + prtval }}
			)*			{{ return el + ")" }}	

    rule prtstmt<<C>>:	PRINT STR 		{{ val = STR }}
			[ "," prtlist 		{{ val = val + prtlist }}
			]			{{ if (C): print eval(val) }}

    rule config<<C>>:	CONFIG PATH		{{ if (C): addconfig(PATH) }}

    rule device<<C>>:   DEVICE dev_path<<C>>
    			enable<<C>>			
			resources<<C>>
			partend<<C>> 

    rule stmt<<C>>:	arch<<C>>		{{ return arch}}
		|	addaction<<C>>		{{ return addaction }}
    		|	config<<C>>		{{ return config}}
		|	default<<C>>		{{ return default}}
		|	dir<<C>>		{{ return dir}}
		|	driver<<C>>		{{ return driver }}
		|	iif<<C>>	 	{{ return iif }}
		|	init<<C>>	 	{{ return init }}
		|	initinclude<<C>>	{{ return initinclude }}
		|	initobject<<C>>		{{ return initobject }}
		|	ldscript<<C>>		{{ return ldscript}}
		|	mainboardinit<<C>>	{{ return mainboardinit }}
		|	makedefine<<C>> 	{{ return makedefine }}
		|	makerule<<C>>		{{ return makerule }}
		|	object<<C>>		{{ return object }}
		|	option<<C>>		{{ return option }}
		|	partdef<<C>>		{{ return partdef }}
		| 	prtstmt<<C>>		{{ return prtstmt }}
		|	register<<C>> 		{{ return register }}
		|	device<<C>>		{{ return device }}

    # ENTRY for parsing Config.lb file
    rule cfgfile:	(uses<<1>>)* 
    			(stmt<<1>>)*
			EOF			{{ return 1 }}

    rule cfgfile:	(uses<<1>>)* 
    			(stmt<<1>>)*
			EOF			{{ return 1 }}

    rule usesid<<C>>:	ID			{{ if (C): usesoption(ID) }}

    rule uses<<C>>:	USES (usesid<<C>>)+

    rule mainboardvariables:	(uses<<1>>)*
				(default<<1>>)*
				(option<<1>>)*
				END		{{ return 1}}

    rule value:		STR			{{ return dequote(STR) }} 
		| 	expr			{{ return expr }}
		|	DELEXPR			{{ return DELEXPR }}

    rule option<<C>>:	OPTION ID EQ value	{{ if (C): setoption(ID, value, 0) }}

    rule opif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(opstmt<<C and c>>)* 
			[ ELSE (opstmt<<C and not c>>)* ] 
			END

    rule opstmt<<C>>:	option<<C>>
		|	opif<<C>>
		|	prtstmt<<C>>

    rule payload<<C>>:	PAYLOAD DIRPATH		{{ if (C): payload(DIRPATH) }}

    rule mainboard:
			MAINBOARD PATH		{{ mainboardsetup(PATH) }}

    rule romif<<C>>:	IF ID			{{ c = lookup(ID) }}
			(romstmt<<C and c>>)* 
			[ ELSE (romstmt<<C and not c>>)* ]
			END

    rule romstmt<<C>>:	romif<<C>>
		|	option<<C>>
		|	payload<<C>>

    rule romimage:	ROMIMAGE STR		{{ startromimage(dequote(STR)) }}
			(romstmt<<1>>)*
			END			{{ endromimage() }}

    rule roms:		STR			{{ s = '[' + STR }}
			( STR			{{ s = s + "," + STR }}
			)*			{{ return eval(s + ']') }}

    rule buildrom:	BUILDROM DIRPATH expr roms	{{ addbuildrom(DIRPATH, expr, roms) }}

    rule romstmts:	romimage 
		|	buildrom
		|	opstmt<<1>>

    # ENTRY for parsing root part
    rule board:		{{ loadoptions("config", "Options.lb", "options") }}
	    		TARGET DIRPATH		{{ target(DIRPATH) }}
			mainboard
			(romstmts)*		
			EOF			{{ return 1 }}

    # ENTRY for parsing a delayed value
    rule delexpr:	"{" expr "}" EOF	{{ return expr }}

    rule wrstr<<ID>>:	STR			{{ setwrite(ID, dequote(STR)) }}

    rule defstmts<<ID>>:			{{ d = 0 }}
			( DEFAULT
			  ( value		{{ setdefault(ID, value, 1) }}
			  | NONE		{{ setnodefault(ID) }}
			  )			{{ d = d | 1 }}
			| FORMAT STR		{{ setformat(ID, dequote(STR)) }}
			| EXPORT 
			  ( ALWAYS		{{ setexported(ID) }}
			  | USED		{{ setexportable(ID) }}
			  | NEVER		{{ setnoexport(ID) }}
			  )			{{ d = d | 2 }}
			| COMMENT STR 		{{ setcomment(ID, dequote(STR)); d = d | 4 }}
			| WRITE (wrstr<<ID>>)+
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
	file.write("# File: %s is autogenerated\n" % fname)

def writemakefilefooter(file, fname):
	file.write("\n\n%s: %s %s\n"
		% (os.path.basename(fname), os.path.abspath(sys.argv[0]), top_config_file))
	file.write("\t(cd %s ; export PYTHONPATH=%s/util/newconfig ; python %s %s %s)\n\n"
		% (os.getcwd(), treetop, sys.argv[0], sys.argv[1], sys.argv[2]))

def writemakefilesettings(path):
	""" Write Makefile.settings to seperate the settings
	from the actual makefile creation."""

	global treetop, target_dir

	filename = os.path.join(path, "Makefile.settings")
	print "Creating", filename
	file = safe_open(filename, 'w+')
	writemakefileheader(file, filename)
	file.write("TOP:=%s\n" % (treetop))
	file.write("TARGET_DIR:=%s\n" % target_dir)
	writemakefilefooter(file, filename)
	file.close()

def writeimagesettings(image):
	"""Write Makefile.settings to seperate the settings
	from the actual makefile creation."""

	global treetop
	global global_options_by_order

	filename = os.path.join(image.gettargetdir(), "Makefile.settings")
	print "Creating", filename
	file = safe_open(filename, 'w+')
	writemakefileheader(file, filename)
	file.write("TOP:=%s\n" % (treetop))
	file.write("TARGET_DIR:=%s\n" % (image.gettargetdir()))
	file.write("\n")
	exported = []
	for o in global_exported_options:
		exported.append(o)
	for o in image.exported_options:
		if (not o in exported):
			exported.append(o)
	for o in exported:
		file.write("export %s:=" % o.name)
		if (hasvalue(o.name, image)):
			file.write("%s" % getformated(o.name, image))
		file.write("\n")
	file.write("\n")
	file.write("export VARIABLES :=\n")
	for o in exported:
		file.write("export VARIABLES += %s\n" % o.name)
	file.write("\n")
	writemakefilefooter(file,filename)
	file.close()

# write the romimage makefile
# let's try the Makefile
# first, dump all the -D stuff

def writeimagemakefile(image):
	makefilepath = os.path.join(image.gettargetdir(), "Makefile")
	print "Creating", makefilepath
	file = safe_open(makefilepath, 'w+')
	writemakefileheader(file, makefilepath)

	# main rule
	file.write("\nall: coreboot.rom\n\n")
	file.write(".PHONY: all\n\n")
	#file.write("include cpuflags\n")
	# Putting "include cpuflags" in the Makefile has the problem that the
	# cpuflags file would be generated _after_ we want to include it.
	# Instead, let make do the work of computing CPUFLAGS:
	file.write("# Get the value of TOP, VARIABLES, and several other variables.\n")
	file.write("include Makefile.settings\n\n")
	file.write("# Function to create an item like -Di586 or -DCONFIG_MAX_CPUS='1' or -Ui686\n")
	file.write("D_item = $(if $(subst undefined,,$(origin $1)),-D$1$(if $($1),='$($1)',),-U$1)\n\n")
	file.write("# Compute the value of CPUFLAGS here during make's first pass.\n")
	file.write("CPUFLAGS := $(foreach _var_,$(VARIABLES),$(call D_item,$(_var_)))\n\n")

	for i in image.getuserdefines():
		file.write("%s\n" %i)
	file.write("\n")

	# print out all the object dependencies
	file.write("\n# object dependencies (objectrules:)\n")
	file.write("INIT-OBJECTS :=\n")
	file.write("OBJECTS :=\n")
	file.write("DRIVER :=\n")
	file.write("\nSOURCES :=\n")
	for irule, init in image.getinitobjectrules().items():
		i_name = init[0]
		i_source = init[1]
		file.write("INIT-OBJECTS += %s\n" % (i_name))
		file.write("SOURCES += %s\n" % (i_source))

	for objrule, obj in image.getobjectrules().items():
		obj_name = obj[0]
		obj_source = obj[1]
		file.write("OBJECTS += %s\n" % (obj_name))
		file.write("SOURCES += %s\n" % (obj_source))

	# for chip_target.c
	file.write("OBJECTS += static.o\n")
	file.write("SOURCES += static.c\n")

	for driverrule, driver in image.getdriverrules().items():
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
	for inc in image.getinitincludes():
		if (local_path.match(inc)):
			file.write("CRT0_INCLUDES += %s\n" % inc)
		else:
			file.write("CRT0_INCLUDES += $(TOP)/src/%s\n" % inc)

	# Print out the user defines.
	file.write("\n# userdefines:\n")
		
	# Print out the base rules.
	# Need to have a rule that counts on 'all'.
	file.write("\n# mainrulelist:")

	# Print out any user rules.
	file.write("\n# From makerule or docipl commands:\n")

	file.write("\n# initobjectrules:\n")
	for irule, init in image.getinitobjectrules().items():
		source = topify(init[1])
		type = init[2]
		if (type  == 'S'):
			# for .S, .o depends on .s
			file.write("%s: %s.s\n" % (init[0], init[3]))
        		file.write("\t$(CC) -c $(CPU_OPT) -o $@ $<\n")
			# and .s depends on .S
			file.write("%s.s: %s\n" % (init[3], source))
			# Note: next 2 lines are ONE output line!
        		file.write("\t$(CPP) $(CPPFLAGS) $< ")
			file.write(">$@.new && mv $@.new $@\n")
		else:
			file.write("%s: %s\n" % (init[0], source))
			file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")

	file.write("\n# objectrules:\n")
	for objrule, obj in image.getobjectrules().items():
		source = topify(obj[1])
		type = obj[2]
		if (type  == 'S'):
			# for .S, .o depends on .s
			file.write("%s: %s.s\n" % (obj[0], obj[3]))
        		file.write("\t$(CC) -c $(CPU_OPT) -o $@ $<\n")
			# and .s depends on .S
			file.write("%s.s: %s\n" % (obj[3], source))
			# Note: next 2 lines are ONE output line!
        		file.write("\t$(CPP) $(CPPFLAGS) $< ")
			file.write(">$@.new && mv $@.new $@\n")
		else:
			file.write("%s: %s\n" % (obj[0], source))
			file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	for driverrule, driver in image.getdriverrules().items():
		source = topify(driver[1])
		file.write("%s: %s\n" % (driver[0], source))
		file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")
		#file.write("%s\n" % objrule[2])

	# special rule for chip_target.c
	file.write("static.o: static.c\n")
	file.write("\t$(CC) -c $(CFLAGS) -o $@ $<\n")

	# Print out the rules that will make cause the files
	# generated by NLBConfig.py to be remade if any dependencies change.

	file.write("\n# Remember the automatically generated files\n")
	file.write("GENERATED:=\n")
	for genfile in ['Makefile',
			'nsuperio.c',
			'static.c',
			'corebootDoc.config' ]:
		file.write("GENERATED += %s\n" % genfile)
	file.write("GENERATED += %s\n" % image.getincludefilename())

	keys = global_options_by_order
	keys.sort()
	file.write("\necho:\n")
	for key in keys:
		 file.write("\t@echo %s='$(%s)'\n"% (key,key))

	for i, m in image.getmakerules().items():
		file.write("%s: " %i)
		for i in m.dependency:
			file.write("%s " % i)
		file.write("\n")
		for i in m.actions:
			file.write("\t%s\n" % i)
	writemakefilefooter(file, makefilepath)
	file.close()

#
def writemakefile(path):
	makefilepath = os.path.join(path, "Makefile")
	print "Creating", makefilepath
	file = safe_open(makefilepath, 'w+')
	writemakefileheader(file, makefilepath)

	# main rule
	file.write("\nall:")
	for i in buildroms:
		file.write(" %s" % i.name)
	file.write("\n\n")	
	file.write("include Makefile.settings\n\n")
	for i, o in romimages.items():
		file.write("%s/coreboot.rom:\n" % o.getname())
		file.write("\tif (cd %s; \\\n" % o.getname())
		file.write("\t\t$(MAKE) coreboot.rom)\\\n")
		file.write("\tthen true; else exit 1; fi;\n\n")
	file.write("clean: ")
	for i in romimages.keys():
		file.write(" %s-clean" % i)
	file.write(" base-clean")
	file.write("\n\n")
	for i, o in romimages.items():
		file.write("%s-clean:\n" % o.getname())
		file.write("\t(cd %s; $(MAKE) clean)\n\n" % o.getname())
	file.write("base-clean:\n")
	file.write("\trm -f romcc*\n\n")

	for i in buildroms:
		file.write("%s:" % i.name)
		for j in i.roms:
			file.write(" %s/coreboot.rom " % j)
		file.write("\n")
		file.write("\t cat ")
		for j in i.roms:
			file.write(" %s/coreboot.rom " % j)
		file.write("> %s\n\n" %i.name)


	file.write(".PHONY: all clean")
	for i in romimages.keys():
		file.write(" %s-clean" % i)
	for i, o in romimages.items():
		file.write(" %s/coreboot.rom" % o.getname())
	file.write("\n\n")

	writemakefilefooter(file, makefilepath)
	file.close()

def writeinitincludes(image):
	global include_pattern
	filepath = os.path.join(image.gettargetdir(), image.getincludefilename())
	print "Creating", filepath
	outfile = safe_open(filepath, 'w+')
	if (image.newformat()):
		infile = safe_open(image.getinitfile(), 'r')

		line = infile.readline()
		while (line):
			p = include_pattern.match(line)
			if (p):
				for i in image.getinitincludes():
					inc = image.getinitinclude(i)
					if (inc.getstring() == p.group(1)):
						outfile.write("#include \"%s\"\n" % inc.getpath())
			else:
				outfile.write(line)
			line = infile.readline()

		infile.close()
	else:
		for i in image.getinitincludes():
			outfile.write("#include <%s>\n" % i)
	outfile.close()

def writeldoptions(image):
	"""Write ldoptions file."""
	filename = os.path.join(image.gettargetdir(), "ldoptions")
	print "Creating", filename
	file = safe_open(filename, 'w+')
	for o in global_exported_options:
		if (hasvalue(o.name, image) and IsInt(getoption(o.name, image))):
			file.write("%s = %s;\n" % (o.name, getformated(o.name, image)))
	for o in image.exported_options:
		if (not o in global_exported_options and hasvalue(o.name, image) and IsInt(getoption(o.name, image))):
			file.write("%s = %s;\n" % (o.name, getformated(o.name, image)))
	file.close()

def dumptree(part, lvl):
	debug.info(debug.dumptree, "DUMPTREE ME is")
	part.dumpme(lvl)
	# dump the siblings -- actually are there any? not sure
	# siblings are:
	debug.info(debug.dumptree, "DUMPTREE SIBLINGS are")
	kid = part.next_sibling
	while (kid):
		kid.dumpme(lvl)
		kid = kid.next_sibling
	# dump the kids
	debug.info(debug.dumptree, "DUMPTREE KIDS are")
	#for kid in part.children:
	if (part.children):
		dumptree(part.children, lvl+1)
	kid = part.next_sibling
	while (kid):
		if (kid.children):
			dumptree(kid.children, lvl + 1)
		kid = kid.next_sibling
	debug.info(debug.dumptree, "DONE DUMPTREE")

def writecode(image):
	filename = os.path.join(img_dir, "static.c")
	print "Creating", filename
	file = safe_open(filename, 'w+')
	file.write("#include <device/device.h>\n")
	file.write("#include <device/pci.h>\n")
	for path in image.getconfigincludes().values():
		file.write("#include \"%s\"\n" % path)
	gencode(image.getroot(), file, 0)
	gencode(image.getroot(), file, 1)
	file.close()

def gencode(part, file, pass_num):
	debug.info(debug.gencode, "GENCODE ME is")
	part.gencode(file, pass_num)
	# dump the siblings -- actually are there any? not sure
	debug.info(debug.gencode, "GENCODE SIBLINGS are")
	kid = part.next_sibling
	while (kid):
		kid.gencode(file, pass_num)
		kid = kid.next_sibling
	# now dump the children 
	debug.info(debug.gencode, "GENCODE KIDS are")
	if (part.children):
		gencode(part.children, file, pass_num)
	kid = part.next_sibling
	while (kid):
		if (kid.children):
			gencode(kid.children, file, pass_num)
		kid = kid.next_sibling
	debug.info(debug.gencode, "DONE GENCODE")

def verifyparse():
	"""Add any run-time checks to verify that parsing the configuration
	was successful"""

	for image in romimages.values():
		print("Verifying ROMIMAGE %s" % image.name)
		if (image.newformat() and image.getinitfile() == ''):
			fatal("An init file must be specified")
		for op in image.exported_options:
			if (getoptionvalue(op.name, op, image) == 0 and getoptionvalue(op.name, op, 0) == 0):
				warning("Exported option %s has no value (check Options.lb)" % op.name);
	print("Verifing global options")
	for op in global_exported_options:
		if (getoptionvalue(op.name, op, 0) == 0):
			notice("Exported option %s has no value (check Options.lb)" % op.name);
			
#=============================================================================
#		MAIN PROGRAM
#=============================================================================
if __name__=='__main__':
	from sys import argv
	if (len(argv) < 3):
		fatal("Args: <file> <path to coreboot>")

	top_config_file = os.path.abspath(sys.argv[1])

	treetop = os.path.abspath(sys.argv[2])

	# Now read in the customizing script...
	loc.push(argv[1])
	fp = safe_open(argv[1], 'r')
	if (not parse('board', fp.read())):
		fatal("Could not parse file")
	loc.pop()

	verifyparse()

	# no longer need to check if an options has been used
	alloptions = 1

	for image_name, image in romimages.items():
		if (debug.level(debug.dumptree)):
			debug.info(debug.dumptree, "DEVICE TREE:")
			dumptree(image.getroot(), 0)

		img_dir = image.gettargetdir()
		if not os.path.isdir(img_dir):
			print "Creating directory %s" % img_dir
			os.makedirs(img_dir)

		if (debug.level(debug.dump)):
			for i in image.getinitincludes():
				debug.info(debug.dump, "crt0include file %s" % i)
			for i in image.getdriverrules().keys():
				debug.info(debug.dump, "driver file %s" % i)
			for i in image.getldscripts():
				debug.info(debug.dump, "ldscript file %s" % i)
			for i, m in image.getmakerules().items():
				debug.info(debug.dump, " makerule %s dep %s act %s" % (i, m.dependency, m.actions))

		writecode(image)
		writeimagesettings(image)
		writeinitincludes(image)
		writeimagemakefile(image)
		writeldoptions(image)

	writemakefilesettings(target_dir)
	writemakefile(target_dir)

	sys.exit(0)
