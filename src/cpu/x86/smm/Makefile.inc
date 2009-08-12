##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 coresystems GmbH
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

##if CONFIG_HAVE_SMI_HANDLER
##	object smmrelocate.S
##
##	smmobject smmhandler.S
##	smmobject smihandler.o
##
##	makerule smm.o
##		depends	"$(SMM-OBJECTS) $(TOP)/src/console/printk.o $(TOP)/src/console/vtxprintf.o $(LIBGCC_FILE_NAME)" 
##		action	"$(CC) $(DISTRO_LFLAGS) -nostdlib -r -o $@ $^"
##	end
##
##	makerule smm
##		depends	"smm.o $(TOP)/src/cpu/x86/smm/smm.ld ldoptions" 
##		action	"$(CC) $(DISTRO_LFLAGS) -nostdlib -nostartfiles -static -o smm.elf -T $(TOP)/src/cpu/x86/smm/smm.ld smm.o"
##		action 	"$(CONFIG_CROSS_COMPILE)nm -n smm.elf | sort > smm.map"
##		action  "$(OBJCOPY) -O binary smm.elf smm"
##	end
##
##	makerule smm_bin.c
##		depends "smm"
##		action "(echo 'unsigned char smm[] = {'; od -vtx1 smm | sed -e 's,^[0-9]* *,,' -e 's:[0-9a-f][0-9a-f] :0x&,:g' -e 's:[0-9a-f][0-9a-f]$$:0x&,:'; echo '}; unsigned int smm_len = '; wc -c smm |awk '{print $$1;}' ; echo ';')  > smm_bin.c"
##	end
##
##	object ./smm_bin.o
##end
