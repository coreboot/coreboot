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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

ramstage-y += south.c
ramstage-y += audio.c
ramstage-y += lpc.c
ramstage-y += ide.c
ramstage-y += pcie.c
ramstage-y += usb.c
ramstage-y += usb_ehci.c
ramstage-y += usb_client.c
ramstage-y += mmc.c
ramstage-y += smbus.c

ramstage-y += reset.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c


# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_CMC) += cmc.bin
cmc.bin-file := $(call strip_quotes,$(CONFIG_CMC_FILE))
cmc.bin-type := 0xaa
cmc.bin-position := 0xfffd0000
