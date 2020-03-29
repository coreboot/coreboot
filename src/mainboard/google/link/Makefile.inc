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

ramstage-y += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

smm-y += mainboard_smi.c

# Order of names in SPD_SOURCES is important!
SPD_SOURCES = elpida_4Gb_1600_x16
SPD_SOURCES += samsung_4Gb_1600_1.35v_x16
SPD_SOURCES += micron_4Gb_1600_1.35v_x16

bootblock-y += gpio.c
romstage-y += gpio.c
bootblock-y += early_init.c
romstage-y += early_init.c
