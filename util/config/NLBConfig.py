#! /usr/bin/python

import sys
import os
import re
import string

debug = 0;

makeoptions = {};
# rule format. Key is the rule name. value is a list. The first
# element of the list is the dependencies, the rest are actions. 
makebaserules = {};
treetop = '';
outputdir = '';
baserules = [
# wouldn't it be nice, but oh well. 
#	'TOP= %s'%(treetop), 
	'# GENERATED MAKEFILE. EDIT AT YOUR OWN RISK!',
	'LINK = ld -T ldscript.ld -o $@ $(OBJECTS)',
# You MUST have -O2, or lots of things (like outb) fail
	'CFLAGS= -I$(TOP)/src/include -O2',
	'CC=cc $(CFLAGS)',
	'',
	'all: romimage',
	'floppy: all ',
	'	mcopy -o romimage a:',
	'# heres the problem: we should not assume we come up with more than ',
	'# 64K of FLASH up.',
	'# SO we need a working linuxbios at the tail, and it will',
	'# enable all flash and then gunzip the linuxbios. As a result, ',
	'# we need the vmlinux.bin.gz padded out and then cat the linuxbios.rom',
	'# at then end. We always copy it to /tmp so that a waiting root shell ',
	'# can put it on the floppy (see ROOTDOIT)',
	'romimage:: linuxbios.rom vmlinux.bin.gz.block ',
	'	cat vmlinux.bin.gz.block linuxbios.rom > romimage',
	'	cp romimage /tmp',
	'linuxbios.rom: linuxbios.strip mkrom ',
	'\t./mkrom -s 64 -f -o linuxbios.rom linuxbios.strip',
	'linuxbios.strip: linuxbios',
	'\tobjcopy -O binary -R .note -R .comment -S linuxbios linuxbios.strip',
	'linuxbios: $(OBJECTS) vmlinux.bin.gz',
	'\t@rm -f biosobject',
	'\t$(LINK) ',
	'\tnm -n linuxbios > linuxbios.map',
	'# crt0 actually includes .inc files. ',
	'# For self-documenting purposes, we put the FULL PATH of the ',
	'# .inc files (relative to $TOP/src) in crt0.S. ',
	'# So, for example, earlymtrr.inc is included as cpu/p6/earlymtrr.inc',
	'# To make this work, add the extra -I $(TOP)/src here. ',
	'crt0.s: crt0.S ',
	'\t$(CC) -I $(TOP)/src $(CPUFLAGS) -E $< > crt0.s',
	'',
	'crt0.o : crt0.s',
        '\t$(CC) -c crt0.s',
	'',
	'mkrom: $(TOP)/mkrom/mkrom.c',
        '\tcc -o mkrom $<',
	'',
	'clean:: ',
	'\trm -f linuxbios.* vmlinux.* *.o mkrom xa? *~',
	'\trm -f linuxbios romimage crt0.s',
	'\trm -f a.out *.s *.l',
	'\trm -f TAGS tags',
	'\trm -f docipl',
	''
	]

linuxrules = [
	'vmlinux.bin.gz.block: vmlinux.bin.gz',
	'\tdd conv=sync bs=448k if=vmlinux.bin.gz of=vmlinux.bin.gz.block',
	'vmlinux.bin.gz: vmlinux.bin',
	'\tgzip -f -3 vmlinux.bin '
	'',
	'vmlinux.bin: $(LINUX)/vmlinux',
	'\tobjcopy -O binary -R .note -R .comment -S $< vmlinux.bin'
	]

# we're trying to generate the ldscript and see how it works out. 
# I think this is a good idea; we'll see. 
ldscript = [
	'/*',
	' * Bootstrap code for the STPC Consumer',
	' * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.',
	' *',
	' * $Id$',
	' *',
	' */',
	'/* oh, barf. This wont work if all you use is .o\'s.  -- RGM */',
	'',
	'/*',
	' *	Written by Johan Rydberg, based on work by Daniel Kahlin.',
	' */',
	'/*',
	' *	We use ELF as output format. So that we can',
	' *	debug the code in some form. ',
	' */',
	'OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")',
	'OUTPUT_ARCH(i386)',
	'',
	'/*',
	' *	Memory map:',
	' *',
	' *	0x00000	(4*4096 bytes)	: stack',
	' *	0x04000	(4096 bytes)	: private data ',
	' *	0x05000			: data space',
	' *	0x90000			: kernel stack',
	' *	0xf0000	(64 Kbyte)	: EPROM',
	' */',
	'MEMORY',
	'{',
	'	ram (rwx) : ORIGIN = 0x00000000, LENGTH = 128M	/* 128 MB memory is max for STPC */',
	'	rom (rx)  : ORIGIN = 0x000f0000, LENGTH = 128K	/* 128 K EPROM */',
	'}',
	'',
	'_PDATABASE	= 0x04000;',
	'_RAMBASE	= 0x05000;',
	'_KERNSTK	= 0x90000;',
	'',
	'/* should be parameterized but is not, yuck! */',
	'_ROMBASE	= 0x80000;',
	'',
	'/*',
	' *	Entry point is not really nececary, since the mkrom(8)',
	' *	tool creates a entry point that jumps to $0xc000:0x0000.',
	' */',
	'/* baloney, but ... RGM*/',
	'ENTRY(_start)',
	'',
	'SECTIONS',
	'{',
	'	/*',
	'	 * First we place the code and read only data (typically const declared).',
	'	 * This get placed in rom.',
	'	 */',
	'	.text _ROMBASE : {',
	'		_text = .;',
	'		*(.text);',
	'		*(.rodata);',
	'		_etext = .;',
	'	}',
	'',
	'	_pdata = .;',
	'',
	'/*',
	'	.pdata _PDATABASE : AT ( LOADADDR(.text) + SIZEOF(.text) +  ',
	'					SIZEOF(.rodata)) {',
	' */',
	'	.pdata _PDATABASE : AT ( _etext ) {',
	'		*(.pdata);',
	'	}',
	'',
	'	_epdata = LOADADDR(.pdata) + SIZEOF(.pdata);',
	'',
	'	/*',
	'	 * After the code we place initialized data (typically initialized',
	'	 * global variables). This gets copied into ram by startup code.',
	'	 * __data_start and __data_end shows where in ram this should be placed,',
	'	 * whereas __data_loadstart and __data_loadend shows where in rom to',
	'	 * copy from.',
	'	 */',
	'	.data _RAMBASE : AT ( LOADADDR(.pdata) + SIZEOF(.pdata) ) {',
	'		_data = .;',
	'		*(.data)',
	'		*(.sdata)',
	'		*(.sdata2)',
	'		*(.got)',
	'		_edata = .;',
	'	}',
	'    ',
	'	_ldata	= LOADADDR(.data);',
	'	_eldata	= LOADADDR(.data) + SIZEOF(.data);',
	' 	',
	'	/*',
	'	 * bss does not contain data, it is just a space that should be zero',
	'	 * initialized on startup. (typically uninitialized global variables)',
	'	 * crt0.S fills between __bss_start and __bss_end with zeroes.',
	'	 */',
	'	.bss ( ADDR(.data) + SIZEOF(.data) ) : {',
	'		_bss = .;',
	'		*(.bss)',
	'		*(.sbss)',
	'		*(COMMON)',
	'		_ebss = .;',
	'		_heap = .;',
	'	}',
	'}',
	'',
	'/*',
	' *	This provides the start and end address for the whole image ',
	' */',
	'_image	= LOADADDR(.text);',
	'_eimage	= LOADADDR(.data) + SIZEOF(.data);',
	'',
	'/* EOF */',
	]

p5crt0preram = [
	'/*',
	' * $ $',
	' *',
	' */',
	'',
	'#include <asm.h>',
	'#include <intel.h>',
	'',
	'#include <pciconf.h>',
	'/*',
	' * This is the entry code (the mkrom(8) utility makes a jumpvector',
	' * to this adddess. ',
	' *',
	' * When we get here we are in x86 real mode.',
	' *',
	' *	%cs	= 0xf000	%ip	= 0x0000',
	' *	%ds	= 0x0000	%es	= 0x0000',
	' *	%dx	= 0x0yxx  (y = 3 for i386, 5 for pentium, 6 for P6,',
	' *					where x is undefined)',
	' *	%fl	= 0x0002',
	' */',
	' 	.text',
	'	.code16',
	'',
	'#include <cpu/p5/start32.inc>',
	'',
	'#include <pc80/i8259.inc>',
	'	/* initialize the RAM */',
	'	/* different for each motherboard */',
]

p5crt0postram = [
	'',
	'/* Turn on mtrr for faster boot */',
	'#include <cpu/p6/earlymtrr.inc>',
	'',
	'/*',
	' *	Copy data into RAM and clear the BSS. Since these segments',
	' *	isn\'t really that big we just copy/clear using bytes, not',
	' *	double words.',
	' */',
	'	intel_chip_post_macro(0x11)		/* post 11 */',
	'',
	'#ifdef SERIAL_CONSOLE',
	'        TTYS0_TX_STRING($str_after_ram)',
	'#endif /* SERIAL_CONSOLE */',
	'',
	'	cld				/* clear direction flag */',
	'',
	'	/* copy data segment from FLASH ROM to RAM */',
	'	leal	EXT(_ldata), %esi',
	'	leal	EXT(_data), %edi',
	'	movl	$EXT(_eldata), %ecx',
	'	subl	%esi, %ecx',
	'	jz	.Lnodata		/* should not happen */',
	'	rep',
	'	movsb',
	'.Lnodata:',
	'	intel_chip_post_macro(0x12)		/* post 12 */',
	'',
	'#ifdef SERIAL_CONSOLE',
	'        TTYS0_TX_STRING($str_after_ram)',
	'#endif /* SERIAL_CONSOLE */',
	'',
	'	/** clear stack */',
	'	xorl	%edi, %edi',
	'	movl	$_PDATABASE, %ecx',
	'	xorl	%eax, %eax',
	'	rep',
	'	stosb',
	'',
	'	/** clear bss */',
	'	leal	EXT(_bss), %edi',
	'	movl	$EXT(_ebss), %ecx',
	'	subl	%edi, %ecx',
	'	jz	.Lnobss',
	'	xorl	%eax, %eax',
	'	rep',
	'	stosb',
	'.Lnobss:',
	'',
	'/*',
	' *	Now we are finished. Memory is up, data is copied and',
	' *	bss is cleared.   Now we call the main routine and',
	' *	let it do the rest.',
	' */ ',
	'	intel_chip_post_macro(0xfe)	/* post fe */',
	'',
	'#ifdef SERIAL_CONSOLE',
	'        TTYS0_TX_STRING($str_pre_main)',
	'#endif /* SERIAL_CONSOLE */',
	'',
	'',
	'/* memory is up. Let\'s do the rest in C -- much easier. */',
	'',
	'	/* set new stack */',
	'	movl	$_PDATABASE, %esp',
	'',
	'	call	EXT(intel_main)',
	'	/*NOTREACHED*/',
	'.Lhlt:	hlt',
	'	jmp	.Lhlt',
	'',
	'ttyS0_test:          .string "\\r\\n\\r\\nHello world!!\\r\\n"',
	'str_after_ram:       .string "Ram Initialize?\\r\\n"',
	'str_after_copy:      .string "after copy?\\r\\n"',
	'str_pre_main:        .string "before main\\r\\n"',
	'newline:             .string "\\r\\n"',
	]

objectrules = [];
userrules = [];

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
	defaultrule = "\t cc -c $(CFLAGS) $(CPUFLAGS) $<"
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

def handleconfig(dir, type, name):
	# seems weird, eh? but config file are always done from
	# treetop
	dir = os.path.join(treetop, type, name)
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
	handleconfig(dir, 'src', fullpath)
	return fullpath

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
	common_command_action(dir, 'northbridge', northbridge_name)

def southbridge(dir, southbridge_name):
	common_command_action(dir, 'southbridge', southbridge_name)

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
def nmakerule(dir, rule): 
	wspc = string.whitespace
	cmd = "([^" + wspc + "]+)"
	sep = "([" + wspc + "]+)"
	rest = "(.*)"
	headtail = "([" + wspc + "]*)"
	pat = cmd + sep + cmd + sep + rest + headtail
	print "pat :", pat, ":", rule
	command_re = re.compile(pat)
	m = command_re.match(rule)
	print m
	rulename = m.group(1)
	dependencies = m.group(3)
	actions = m.group(5)
	print "rulename :", rulename, ": deps:", dependencies,":"
	print "    actions ", actions
	userrules.append(rule)

def makerule(dir, rule): 
	userrules.append(rule)
def option(dir, option):
	makeoptions[option] = 1;

def nooption(dir, option):
	makeoptions[option] = 0;

def commandline(dir, command):
	rule = "CMD_LINE=\'\"" + command + "\"\'"
	makerule(dir, rule)

# we do all these rules by hand because docipl will always be special
# it's more or less a stand-alone bootstrap
def docipl(dir, ipl_name):
	mainboard = command_vals['mainboard']
	mainboard_dir = os.path.join(treetop, 'src', mainboard)
	# add a rule for the docipl
	userrules.append("romimage::docipl")
	# add the docipl rule
	userrules.append("docipl: ipl.o")
	userrules.append("\tobjcopy -O binary -R .note -R .comment -S ipl.o docipl")
	# now, add the ipl.o rule
	iplpath = os.path.join(treetop, 'src', ipl_name)
	userrules.append("ipl.o: " + iplpath)
	# Now we need a mainboard-specific include path
	userrules.append("\tcc $(CPUFLAGS) -I%s -c $<" % mainboard_dir)

def linux(dir, linux_name):
	linuxrule = 'LINUX=' + linux_name
	makerule(dir, linuxrule)

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
	'raminit'     : []    # set of files to include for ram init
	}

command_actions = {
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
	'keyboard'    : keyboard, 
	'docipl'      : docipl,
	'makerule'    : makerule,
	'nmakerule'    : nmakerule,
	'option'    : option,
	'nooption'    : nooption,
	'commandline' : commandline
	}

makeobjects = [];

def readfile(filename): 
	# open file, extract lines, and close
	if not os.path.isfile(filename):
		print config_file, "is not a file \n"
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
			
			if command_actions.has_key(verb):
				command_actions[verb](dir, args)
			elif list_vals.has_key(verb):
				list_vals[verb] = list_vals[verb] + [args]
			else:
				print verb, "is not a valid command! \n"
				sys.exit()

# output functions
# write crt0
def writep5crt0(path):
	crt0filepath = os.path.join(path, "crt0.S")
	raminitfiles = command_vals["raminit"]
	paramfile = os.path.join(treetop, 'src/include', 
			command_vals['northbridge'][0], "param.h")
	print "Trying to create ", crt0filepath
#	try: 
	file = open(crt0filepath, 'w+')

	for i in range(len(p5crt0preram)):
		file.write("%s\n" % p5crt0preram[i])
	print "Check for crt0.S param file:", paramfile
	if os.path.isfile(paramfile):
		ipfile = os.path.join(command_vals['northbridge'][0],"param.h")
		file.write("#include <%s>\n" %  ipfile)
		print " Adding include to crt0.S for this parameter file"

	# we will do this better at some point. 
	# possible we need a 'console' command.
	file.write("\n");
	file.write("#ifdef SERIAL_CONSOLE\n");
	file.write("#include <%s/setup_serial.inc>\n" % command_vals['superio'])
	file.write("#include <pc80/serial.inc>\n");
	file.write("TTYS0_TX_STRING($ttyS0_test)\n")
	file.write("#endif /* SERIAL_CONSOLE */\n")
	file.write("\n");

	for i in range(len(raminitfiles)):
		file.write("#include <%s>\n" % raminitfiles[i])
	for i in range(len(p5crt0postram)):
		file.write("%s\n" % p5crt0postram[i])

# write ldscript
def writeldscript(path):
	ldfilepath = os.path.join(path, "ldscript.ld")
	print "Trying to create ", ldfilepath
#	try: 
	file = open(ldfilepath, 'w+')
	# print out the ldscript rules
	
	for i in range(len(ldscript)):
		file.write("%s\n" % ldscript[i])
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
		if (makeoptions[z]):
			file.write("CPUFLAGS += -D%s\n" % (z))
		else:
			file.write("CPUFLAGS += -U%s\n" % (z))
				
	# print out all the object dependencies
	# There is ALWAYS a crt0.o
	file.write("OBJECTS=crt0.o\n")
	for i in range(len(objectrules)):
		file.write("OBJECTS += %s\n" % (objectrules[i][0]))
		
	# print out the base rules
	
	for i in range(len(baserules)):
		file.write("%s\n" % baserules[i])

	# print out any user rules
	for i in range(len(userrules)):
		file.write("%s\n" % userrules[i])
		
	for i in range(len(objectrules)):
		base = objectrules[i][0]
		base = base[0:len(base)-2]
		source = os.path.join(objectrules[i][1], base)
		source = source + ".c"
		file.write( "%s: %s\n" % (objectrules[i][0], source))
		file.write( "%s\n" % objectrules[i][2])

	# print out the linux rules
	# these go here because some pieces depend on user rules
	
	for i in range(len(linuxrules)):
		file.write("%s\n" % linuxrules[i])

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
	
doconfigfile(treetop, sys.argv[1])


# do standard config files that the user need not specify
# for now, this is just 'lib', but i tmay be more later. 
libdir = treetop + "/src/lib"
libconfigfile = libdir + "/Config"
print "Process config file: ", libconfigfile
doconfigfile(libdir, libconfigfile)

# print out command values
#print "Command Values:"
#for key,val in command_vals.items():
#	print key, val

writemakefile(outputdir)
writeldscript(outputdir)
writep5crt0(outputdir)
