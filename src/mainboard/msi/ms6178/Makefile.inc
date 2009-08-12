##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

driver-y += mainboard.o

obj-$(CONFIG_HAVE_MP_TABLE) += mptable.o
obj-$(CONFIG_HAVE_PIRQ_TABLE) += irq_tables.o

initobj-y += ../../../arch/i386/init/entry.o
initobj-y += ../../../cpu/intel/model_6ex/car.o # FIXME. romcc.
# initobj-y += ../../../arch/i386/init/rombootstrap.o
# initobj-y += ../../../cpu/intel/model_6ex/disable_car.o
initobj-y += ../../../pc80/mc146818rtc_early.o
initobj-y += ../../../arch/i386/lib/console.o
initobj-y += ../../../arch/i386/lib/console_printk.o
# initobj-y += ../../../ram/ramtest.o # FIXME
initobj-y += ../../../southbridge/intel/i82801xx/i82801xx_early_smbus.o
initobj-y += ../../../southbridge/intel/i82801xx/i82801xx_reset.o
initobj-y += ../../../superio/winbond/w83627hf/w83627hf_early_serial.o
initobj-y += ../../../northbridge/intel/i82810/raminit.o

ifdef POST_EVALUATION

# FIXME: Drop DCACHE_RAM_BASE/DCACHE_RAM_SIZE, only here to make it build.
MAINBOARD_OPTIONS=\
	-DCONFIG_USE_PRINTK_IN_CAR=1 \
	-DCONFIG_HAVE_HIGH_TABLES=1 \
	-DCONFIG_MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID=0 \
	-DCONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID=0 \
	-DCONFIG_MAINBOARD_VENDOR=\"MSI\" \
	-DCONFIG_MAINBOARD_PART_NUMBER=\"MS-6178\" \
	-DCONFIG_DCACHE_RAM_BASE=0xffdf8000 \
	-DCONFIG_DCACHE_RAM_SIZE=0x8000

endif

