##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

ifeq ($(CONFIG_SOUTHBRIDGE_AMD_AMD8111),y)

STAGE2_CHIPSET_SRC += $(src)/southbridge/amd/amd8111/amd8111.c

STAGE2_CHIPSET_SRC += \
					$(src)/southbridge/amd/amd8111/ac97.c \
 					$(src)/southbridge/amd/amd8111/acpi.c \
 					$(src)/southbridge/amd/amd8111/ide.c \
 					$(src)/southbridge/amd/amd8111/lpc.c \
 					$(src)/southbridge/amd/amd8111/nic.c \
					$(src)/southbridge/amd/amd8111/pci.c \
					$(src)/southbridge/amd/amd8111/smbus.c \
 					$(src)/southbridge/amd/amd8111/usb.c \
 					$(src)/southbridge/amd/amd8111/usb2.c 
					
STAGE0_CHIPSET_OBJ += $(obj)/southbridge/amd/amd8111/stage1.o 


endif
