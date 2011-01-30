##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 coresystems GmbH
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

driver-y += south.c
driver-y += audio.c
driver-y += lpc.c
driver-y += ide.c
driver-y += pcie.c
driver-y += usb.c
driver-y += usb_ehci.c
driver-y += usb_client.c
driver-y += mmc.c
driver-y += smbus.c

ramstage-y += reset.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

romstage-$(CONFIG_USBDEBUG) += usb_debug.c

# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_CMC) += cmc.bin
cmc.bin-file := $(CONFIG_CMC_FILE)
cmc.bin-type := 0xaa
cmc.bin-position := 0xfffd0000
