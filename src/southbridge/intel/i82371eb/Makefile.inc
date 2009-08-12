##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb_isa.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb_ide.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb_usb.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb_smbus.o
driver-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) +=  i82371eb_reset.o

#initobj-$(CONFIG_SOUTHBRIDGE_INTEL_I82371EB) += i82371eb_early_rom.o
