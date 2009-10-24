##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2009 coresystems GmbH
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
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

driver-y += i82801gx.o
driver-y += i82801gx_ac97.o
driver-y += i82801gx_azalia.o
driver-y += i82801gx_ide.o
driver-y += i82801gx_lpc.o
driver-y += i82801gx_nic.o
driver-y += i82801gx_pci.o
driver-y += i82801gx_pcie.o
driver-y += i82801gx_sata.o
driver-y += i82801gx_smbus.o
driver-y += i82801gx_usb.o
driver-y += i82801gx_usb_ehci.o

obj-y += i82801gx_reset.o
obj-y += i82801gx_watchdog.o

obj-$(CONFIG_HAVE_SMI_HANDLER) += i82801gx_smi.o
smmobj-$(CONFIG_HAVE_SMI_HANDLER) += i82801gx_smihandler.o
