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

driver-y += i82801gx.c
driver-y += i82801gx_ac97.c
driver-y += i82801gx_azalia.c
driver-y += i82801gx_ide.c
driver-y += i82801gx_lpc.c
driver-y += i82801gx_nic.c
driver-y += i82801gx_pci.c
driver-y += i82801gx_pcie.c
driver-y += i82801gx_sata.c
driver-y += i82801gx_smbus.c
driver-y += i82801gx_usb.c
driver-y += i82801gx_usb_ehci.c

ramstage-y += i82801gx_reset.c
ramstage-y += i82801gx_watchdog.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += i82801gx_smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += i82801gx_smihandler.c

romstage-y += i82801gx_early_smbus.c
romstage-$(CONFIG_USBDEBUG) += i82801gx_usb_debug.c

