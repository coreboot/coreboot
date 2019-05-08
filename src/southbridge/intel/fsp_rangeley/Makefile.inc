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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_FSP_RANGELEY),y)

ramstage-y += soc.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += spi.c
ramstage-y += smbus.c
ramstage-y += acpi.c

romstage-y += early_usb.c early_smbus.c gpio.c early_init.c
romstage-y += romstage.c

bootblock-$(CONFIG_USBDEBUG) += usb_debug.c
romstage-$(CONFIG_USBDEBUG) += usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += usb_debug.c

endif
