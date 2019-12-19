## This file is part of the coreboot project.
##
## Copyright 2019 Google LLC
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

SPD_SOURCES = 4G_2400         # 0b0000
SPD_SOURCES += empty_ddr4     # 0b0001
SPD_SOURCES += 8G_2400        # 0b0010
SPD_SOURCES += 8G_2666        # 0b0011
SPD_SOURCES += 16G_2400       # 0b0100
SPD_SOURCES += 16G_2666       # 0b0101
SPD_SOURCES += 8G_3200        # 0b0110
SPD_SOURCES += 16G_3200       # 0b0111
SPD_SOURCES += 16G_2666_2bg   # 0b1000
SPD_SOURCES += 16G_3200_4bg   # 0b1001

bootblock-y += gpio.c
ramstage-y += gpio.c
ramstage-y += variant.c
ramstage-y += ramstage.c
