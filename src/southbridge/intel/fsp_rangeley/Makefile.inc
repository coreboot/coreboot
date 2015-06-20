##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
## Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_FSP_RANGELEY),y)

subdirs-y += ../common/firmware

ramstage-y += soc.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += reset.c
ramstage-y += watchdog.c
ramstage-y += spi.c
ramstage-y += smbus.c
ramstage-y += acpi.c

romstage-y += early_usb.c early_smbus.c gpio.c reset.c early_spi.c early_init.c
romstage-y += romstage.c

romstage-$(CONFIG_USBDEBUG) += usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += usb_debug.c

endif
