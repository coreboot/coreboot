##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
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

me-src-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += me.c
me-src-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += me_8.x.c

driver-y += pch.c
driver-y += azalia.c
driver-y += lpc.c
driver-y += pci.c
driver-y += pcie.c
driver-y += sata.c
driver-y += usb_ehci.c
driver-y += $(me-src-y)
driver-y += smbus.c

ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c $(me-src-y) finalize.c

romstage-y += early_usb.c early_smbus.c early_me.c me_status.c gpio.c
romstage-$(CONFIG_USBDEBUG) += usb_debug.c
romstage-y += reset.c

