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

ifeq ($(CONFIG_SOUTHBRIDGE_AMD_SB600),y)

STAGE2_CHIPSET_SRC += $(src)/southbridge/amd/sb600/sb600.c

STAGE2_CHIPSET_SRC += \
					$(src)/southbridge/amd/sb600/ac97.c \
 					$(src)/southbridge/amd/sb600/hda.c \
 					$(src)/southbridge/amd/sb600/ide.c \
 					$(src)/southbridge/amd/sb600/lpc.c \
					$(src)/southbridge/amd/sb600/pci.c \
					$(src)/southbridge/amd/sb600/sata.c \
					$(src)/southbridge/amd/sb600/sm.c \
  					$(src)/southbridge/amd/sb600/usb.c
					
STAGE0_CHIPSET_SRC += $(src)/southbridge/amd/sb600/stage1.c \
					$(src)/southbridge/amd/sb600/stage1_reset.c \
					$(src)/southbridge/amd/sb600/stage1_smbus.c

endif
