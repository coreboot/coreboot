# -*- python -*-
import sys
import os
import re
import string
import types

import traceback

warnings = 0
errors = 0

treetop = ''
full_mainboard_path = ''
mainboard_path = ''
romimages = {}
curimage = 0

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

class debug_info:
	none = 0
	gencode = 1
	dumptree = 2
	object = 3
	dict = 4
	statement = 5
	dump = 6
	gengraph = 7

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
#debug = debug_info(debug_info.dumptree)
#debug = debug_info(debug_info.object)
#debug = debug_info(debug_info.gencode)

# -----------------------------------------------------------------------------
#                    Error Handling
# -----------------------------------------------------------------------------

def error(string):      
	"""Print error message"""
        global errors, loc
	errors = errors + 1
        print "===> ERROR: %s" % string

def fatal(string):      
	"""Print error message and exit"""
	error(string)
        exitiferrors()

def warning(string):
	"""Print warning message"""
        global warnings, loc
	warnings = warnings + 1
        print "===> WARNING: %s" % string

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

		# instance counter for parts
		self.partinstance = 0

		# chip config files included by the 'config' directive
		self.configincludes = {}

		# root of part tree
		self.root = 0

		# Last device built
		self.last_device = 0

	def getname(self):
		return self.name

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

class partobj:
	"""A configuration part"""
	def __init__ (self, image, dir, parent, part, type_name, instance_name, chip_or_device):
		if (parent):
			debug.info(debug.object, "partobj dir %s parent %s part %s" \
				% (dir, parent.instance_name, part))
		else:
			debug.info(debug.object, "partobj dir %s part %s" \
				% (dir, part))

		# romimage that is configuring this part
		self.image = image

		# links for static device tree
		self.children = 0
		self.prev_sibling = 0
		self.next_sibling = 0
		self.prev_device = 0
		self.next_device = 0
		self.chip_or_device = chip_or_device

		# initializers for static device tree
		self.registercode = {}

		# part name
		self.part = part

		# type name of this part
		self.type_name = type_name

		# directory containing part files
		self.dir = dir

		# instance number, used to distinguish anonymous
		# instances of this part
		self.instance = image.newpartinstance()
		debug.info(debug.object, "INSTANCE %d" % self.instance)

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

		# Flag if I am a duplicate device
		self.dup = 0

		# If there is a chip.h file, we will create an 
		# include for it. 
		if (dir):
			chiph = os.path.join(dir, "chip.h")
			if (os.path.exists(chiph)):
				debug.info(debug.object,  "%s has chip at %s" % (self, dir))
				self.addconfig(chiph)

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

	def graph_name(self):
		name = "{ {_dev%d|" % self.instance
		if (self.part):
			name = "%s%s" % (name, self.part)
		else:
			name = "%s%s" % (name, self.chip_or_device)
		if (self.type_name):
			name = "%s}|%s}" % (name, self.type_name)
		else:
			name = "%s}|%s}" % (name, self.parent.type_name)
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
		print "%d: registercode " % lvl
		for f, v in self.registercode.items():
			print "\t%s = %s" % (f, v)
		print "%d: chipconfig %s" % (lvl, self.chipconfig)
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
		self.set_path(".type=DEVICE_PATH_PCI,{.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}" % (slot, function))

	def addpnppath(self, port, device):
		""" Add a relative path to a pnp device hanging off our parent """
		if ((port < 0) or (port > 65536)):
			fatal("Invalid port")
		if ((device < 0) or (device > 0xffff)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_PNP,{.pnp={ .port = 0x%x, .device = 0x%x }}" % (port, device))
		
	def addi2cpath(self, device):
		""" Add a relative path to a i2c device hanging off our parent """
		if ((device < 0) or (device > 0x7f)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_I2C,{.i2c={ .device = 0x%x }}" % (device))

	def addapicpath(self, apic_id):
		""" Add a relative path to a cpu device hanging off our parent """
		if ((apic_id < 0) or (apic_id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_APIC,{.apic={ .apic_id = 0x%x }}" % (apic_id))
    
	def addpci_domainpath(self, pci_domain):
		""" Add a pci_domain number to a chip """
		if ((pci_domain < 0) or (pci_domain > 0xffff)):
			fatal("Invalid pci_domain: 0x%x is out of the range 0 to 0xffff" % pci_domain)
		self.set_path(".type=DEVICE_PATH_PCI_DOMAIN,{.pci_domain={ .domain = 0x%x }}" % (pci_domain))
    
	def addapic_clusterpath(self, cluster):
		""" Add an apic cluster to a chip """
		if ((cluster < 0) or (cluster > 15)):
			fatal("Invalid apic cluster: %d is out of the range 0 to ff" % cluster)
		self.set_path(".type=DEVICE_PATH_APIC_CLUSTER,{.apic_cluster={ .cluster = 0x%x }}" % (cluster))
    
	def addcpupath(self, cpu_id):
		""" Add a relative path to a cpu device hanging off our parent """
		if ((cpu_id < 0) or (cpu_id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_CPU,{.cpu={ .id = 0x%x }}" % (cpu_id))
    

	def addcpu_buspath(self, id):
		""" Add a cpu_bus to a chip """
		if ((id < 0) or (id > 255)):
			fatal("Invalid device")
		self.set_path(".type=DEVICE_PATH_CPU_BUS,{.cpu_bus={ .id = 0x%x }}" % (id))
    

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
	if name in dict.keys():
		print "Duplicate in dict: %s" % name
	dict[name] = value


def addconfig(path):
	global partstack
	curpart = partstack.tos()
	curpart.addconfig(path)

def addregister(field, value):
	global partstack
	curpart = partstack.tos()
	curpart.addregister(field, value)

def devicepart(type):
	global curimage, partstack
	newpart = partobj(curimage, 0, partstack.tos(), type, \
			'', 0, 'device')
	#print "Configuring PART %s" % (type)
	partstack.push(newpart)
	#print "  new PART tos is now %s\n" %partstack.tos().info()
	# just push TOS, so that we can pop later. 
	
def part(type, path, file, name):
	global curimage, partstack
	partdir = os.path.join(type, path)
	srcdir = os.path.join(treetop, 'src')
	fulldir = os.path.join(srcdir, partdir)
	type_name = flatten_name(partdir)
	#print "PART(%s, %s, %s, %s)\n" % (type, path, file, name)
	newpart = partobj(curimage, fulldir, partstack.tos(), type, \
			type_name, name, 'chip')
	#print "Configuring PART %s, path %s" % (type, path)
	partstack.push(newpart)

def partpop():
	global partstack
	curpart = partstack.tos()
	if (curpart == 0):
		fatal("Trying to pop non-existent part")
	#print "End PART %s" % curpart.part
	oldpart = partstack.pop()
	#print "partstack.pop, TOS is now %s\n" % oldpart.info()

#=============================================================================
#		MISC FUNCTIONS
#=============================================================================
def dequote(str):
	a = re.sub("^\"", "", str)
	a = re.sub("\"$", "", a)
	# highly un-intuitive, need four \!
	a = re.sub("\\\\\"", "\"", a)
	return a

def flatten_name(str):
	a = re.sub("[/-]", "_", str)
	return a
%%
parser Config:
    ignore:			r'\s+'
    ignore:			"#.*?\r?\n"

    # less general tokens should come first, otherwise they get matched
    # by the re's
    token COMMENT:		'comment'
    token CPU:			'cpu'
    token CPU_BUS:		'cpu_bus'
    token CHIP:			'chip'
    token DEVICE:		'device'
    token DEVICE_ID:		'device_id'
    token DRQ:			'drq'
    token END:			'end'
    token EOF:			'$'
    token EQ:			'='
    token FORMAT:		'format'
    token IO:			'io'
    token IRQ:			'irq'
    token MEM:			'mem'
    token NEVER:		'never'
    token NONE:			'none'
    token PMC:			'pmc'
    token PRINT:		'print'
    token REGISTER:		'register'
    token VENDOR_ID:		'vendor_id'
    token WRITE:		'write'
    token NUM:			'[0-9]+'
    token HEX_NUM:		'[0-9a-fA-F]+'
    token HEX_PREFIX:		'0x'
    # Why is path separate? Because paths to resources have to at least
    # have a slash, we thinks
    token PATH:			r'[-a-zA-Z0-9_.][-a-zA-Z0-9/_.]+[-a-zA-Z0-9_.]+'
    # Dir's on the other hand are abitrary
    # this may all be stupid.
    token RULE:			r'[-a-zA-Z0-9_$()./]+[-a-zA-Z0-9_ $()./]+[-a-zA-Z0-9_$()./]+'
    token ID:			r'[a-zA-Z_.]+[a-zA-Z0-9_.]*'
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
			| "[|][|]" logical	{{ l = l or logical }}
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

    rule parttype:	CHIP                    {{ return '' }}

    rule partdef<<C>>:				{{ name = 0 }} 
			parttype partid		
			[ STR			{{ name = dequote(STR) }}
			]                       {{ if (C): part(parttype, partid, 'Config.lb', name) }}
			partend<<C>>

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

    rule device<<C>>:   DEVICE dev_path<<C>>
    			enable<<C>>			
			resources<<C>>
			partend<<C>> 

    rule stmt<<C>>:
			partdef<<C>>		{{ return partdef }}
		| 	prtstmt<<C>>		{{ return prtstmt }}
		|	register<<C>> 		{{ return register }}
		|	device<<C>>		{{ return device }}

    rule value:		STR			{{ return dequote(STR) }} 
		| 	expr			{{ return expr }}

    rule devicetree: 	partdef<<1>>
			EOF 			{{ return 1 }}

    rule wrstr<<ID>>:	STR			{{ setwrite(ID, dequote(STR)) }}

%%

#=============================================================================
#		FILE OUTPUT 
#=============================================================================

def dumptree(part, lvl):
	debug.info(debug.dumptree, "DUMPTREE ME is")
	print "%s " % part
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
	print "    SCONFIG   ", join(filename.split('/')[-4:], '/')
	file = safe_open(filename, 'w+')
	file.write("#include <device/device.h>\n")
	file.write("#include <device/pci.h>\n")
	for path in image.getconfigincludes().values():
		file.write("#include \"%s\"\n" % path)
	file.write("\n/* pass 0 */\n")
	gencode(image.getroot(), file, 0)
	file.write("\n/* pass 1 */\n")
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

def writegraph(image):
	filename = os.path.join(img_dir, "static.dot")
	print "    SCONFIG   ", join(filename.split('/')[-4:], '/')
	file = safe_open(filename, 'w+')
	file.write("digraph devicetree {\n")
	file.write("	rankdir=LR\n")
	genranks(image.getroot(), file, 0)
	gennodes(image.getroot(), file)
	gengraph(image.getroot(), file)
	file.write("}\n")
	file.close()

def genranks(part, file, level):
	#file.write("	# Level %d\n" % level )
	file.write("	{ rank = same; \"dev_%s_%d\"" % (part.type_name,part.instance ))
	sib = part.next_sibling
	while (sib):
		file.write("; \"dev_%s_%d\"" % (sib.type_name, sib.instance))
		sib = sib.next_sibling
	file.write("}\n" )
	# now dump the children 
	if (part.children):
		genranks(part.children, file, level + 1)

	kid = part.next_sibling
	while (kid):
		if (kid.children):
			genranks(kid.children, file, level + 1)
		kid = kid.next_sibling


def gennodes(part, file):
	file.write("	dev_%s_%d[shape=record, label=\"%s\"];\n" % (part.type_name,part.instance,part.graph_name() ))
	sib = part.next_sibling
	while (sib):
		file.write("	dev_%s_%d[shape=record, label=\"%s\"];\n" % (sib.type_name,sib.instance,sib.graph_name() ))
		sib = sib.next_sibling
	# now dump the children
	if (part.children):
		gennodes(part.children, file)

	kid = part.next_sibling
	while (kid):
		if (kid.children):
			gennodes(kid.children, file)
		kid = kid.next_sibling


def gengraph(part, file):
	if (part.parent != part):
		file.write("	dev_%s_%d -> dev_%s_%d;\n" % \
				(part.parent.type_name, part.parent.instance, \
				 part.type_name, part.instance ))
	sib = part.next_sibling
	while (sib):
		file.write("	dev_%s_%d -> dev_%s_%d;\n" % \
				(sib.parent.type_name, sib.parent.instance, \
				 sib.type_name, sib.instance ))
		sib = sib.next_sibling

	kid = part.next_sibling
	while (kid):
		if (kid.children):
			gengraph(kid.children, file)
		kid = kid.next_sibling

	if (part.children):
		gengraph(part.children, file)

#=============================================================================
#		MAIN PROGRAM
#=============================================================================
if __name__=='__main__':
	from sys import argv
	if (len(argv) < 4):
		fatal("Args: <file> <path to coreboot> <output-dir>")

	file = "devicetree.cb"
	partdir = os.path.join("mainboard", sys.argv[1])
	treetop = argv[2]
	srcdir = os.path.join(treetop, 'src')
	fulldir = os.path.join(srcdir, partdir)
	type_name = flatten_name(partdir)
	config_file = os.path.join(fulldir, file)

	curimage = romimage("new")
	image = curimage

	newpart = partobj(curimage, fulldir, partstack.tos(), 'mainboard', \
		'mainboard', 0, 'chip')
       #print "Configuring PART %s, path %s" % (type, path)
	image.setroot(newpart);
	partstack.push(newpart)

	fp = safe_open(config_file, 'r')
	if (not parse('devicetree', fp.read())):
		fatal("Could not parse file")
	partstack.pop()
	
	img_dir = argv[3]

	#debug.info(debug.dumptree, "DEVICE TREE:")
	#dumptree(curimage.getroot(), 0)

	writecode(image)
	writegraph(image)

	sys.exit(0)
