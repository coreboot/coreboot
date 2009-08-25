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

driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_ac97.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_ide.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_lpc.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_nic.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_pci.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_sata.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_smbus.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_usb.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_usb_ehci.o

object-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_reset.o
object-$(CONFIG_SOUTHBRIDGE_INTEL_I82801XX) += i82801xx_watchdog.o

# TODO: What about cmos_failover.c?

