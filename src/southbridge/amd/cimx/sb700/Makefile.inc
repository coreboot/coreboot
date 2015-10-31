#
# This file is part of the coreboot project.
#
# Copyright (C) 2012 Advanced Micro Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#


# SB700 Platform Files

romstage-y += early.c
romstage-y += smbus.c smbus_spd.c
romstage-y += reset.c

ramstage-y += late.c
ramstage-y += reset.c

ramstage-y   += smbus.c
ramstage-y   += lpc.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += ../../sb700/enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += ../../sb700/enable_usbdebug.c
