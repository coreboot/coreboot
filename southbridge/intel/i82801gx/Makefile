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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82801GX),y)

STAGE2_CHIPSET_SRC += $(src)/southbridge/intel/i82801gx/i82801gx.c

STAGE2_CHIPSET_SRC += \
			$(src)/southbridge/intel/i82801gx/ac97.c \
  			$(src)/southbridge/intel/i82801gx/ide.c \
 			$(src)/southbridge/intel/i82801gx/lpc.c \
			$(src)/southbridge/intel/i82801gx/nic.c \
			$(src)/southbridge/intel/i82801gx/pci.c \
			$(src)/southbridge/intel/i82801gx/pcie.c \
			$(src)/southbridge/intel/i82801gx/sata.c \
			$(src)/southbridge/intel/i82801gx/smi.c \
			$(src)/southbridge/intel/i82801gx/smbus.c \
			$(src)/southbridge/intel/i82801gx/usb_ehci.c \
  			$(src)/southbridge/intel/i82801gx/usb.c	\
  			$(src)/southbridge/intel/i82801gx/watchdog.c \
			$(src)/southbridge/intel/i82801gx/libsmbus.c \
			$(obj)/southbridge/intel/i82801gx/smmrelocate.o \

ifeq ($(CONFIG_PIRQ_TABLE),y)
STAGE2_CHIPSET_SRC += $(src)/southbridge/intel/i82801gx/irq_tables.c
endif

$(obj)/southbridge/intel/i82801gx/smmrelocate.o:	$(src)/southbridge/intel/i82801gx/smmrelocate.S
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(COREBOOTINCLUDE) -I$(src)/northbridge/intel/i945 $< \
		-o $(obj)/southbridge/intel/i82801gx/smmrelocate.s 
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/southbridge/intel/i82801gx/smmrelocate.s -o $@

STAGE0_CHIPSET_SRC += \
					$(src)/southbridge/intel/i82801gx/stage1_smbus.c \
					$(src)/southbridge/intel/i82801gx/libsmbus.c \

endif
