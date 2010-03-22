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

driver-y += i82801bx.o
driver-y += i82801bx_ac97.o
driver-y += i82801bx_ide.o
driver-y += i82801bx_lpc.o
driver-y += i82801bx_nic.o
driver-y += i82801bx_pci.o
driver-y += i82801bx_sata.o
# driver-y += i82801bx_smbus.o
driver-y += i82801bx_usb.o
driver-y += i82801bx_usb_ehci.o

obj-y += i82801bx_reset.o
obj-y += i82801bx_watchdog.o

# TODO: Fix and enable i82801bx_smbus.o later.

