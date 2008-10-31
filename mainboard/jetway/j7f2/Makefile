##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
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

## TODO

STAGE0_MAINBOARD_SRC := $(src)/mainboard/$(MAINBOARDDIR)/stage1.c \
			$(src)/arch/x86/stage1_mtrr.c

INITRAM_SRC =   	$(src)/mainboard/$(MAINBOARDDIR)/initram.c \
			$(src)/northbridge/via/cn700/initram.c \
			$(src)/southbridge/via/vt8237/stage1.c \
			$(src)/lib/ramtest.c \
			$(src)/arch/x86/pci_ops_conf1.c

STAGE2_MAINBOARD_SRC = 

$(obj)/coreboot.vpd:
	$(Q)printf "  BUILD   DUMMY VPD\n"
	$(Q)dd if=/dev/zero of=$(obj)/coreboot.vpd bs=256 count=1 $(SILENT)

