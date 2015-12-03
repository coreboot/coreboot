##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
## Copyright (C) 2013-2015 Sage Electronic Engineering, LLC.
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
## Foundation, Inc.
##

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_FSP_I89XX),y)

ramstage-y += pch.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += me.c
ramstage-y += me_8.x.c
ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += ../common/spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += ../common/spi.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me.c me_8.x.c finalize.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += ../../../console/post.c

romstage-y += early_usb.c early_smbus.c early_me.c me_status.c gpio.c early_init.c
romstage-$(CONFIG_USBDEBUG) += usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += usb_debug.c
smm-$(CONFIG_USBDEBUG) += usb_debug.c
romstage-y += reset.c
romstage-y += early_spi.c
romstage-y += romstage.c

CPPFLAGS_common += -I$(src)/southbridge/intel/fsp_i89xx

endif
