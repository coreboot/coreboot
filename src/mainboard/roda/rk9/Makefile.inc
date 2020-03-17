##
## This file is part of the coreboot project.
##
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

bootblock-y += bootblock.c

romstage-y = gpio.c

ramstage-$(CONFIG_CARDBUS_PLUGIN_SUPPORT) += ti_pci7xx1.c
ramstage-y += cstates.c
ramstage-y += blc.c
