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

SPD_SOURCES = 4G_2400   # 0b000
SPD_SOURCES += empty_ddr4 # 0b001
SPD_SOURCES += 8G_2400    # 0b010
SPD_SOURCES += 8G_2666    # 0b011
SPD_SOURCES += 16G_2400   # 0b100
SPD_SOURCES += 16G_2666   # 0b101
SPD_SOURCES += 8G_3200    # 0b110
SPD_SOURCES += 16G_3200   # 0b111

bootblock-y += gpio.c
ramstage-y += gpio.c
ramstage-y += variant.c
