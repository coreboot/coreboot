##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
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

romstage-y += modhwinfo.c

ramstage-y += gpio.c
ramstage-y += irqroute.c
ramstage-y += modhwinfo.c
ramstage-y += lcd_panel.c
ramstage-y += ptn3460.c

cbfs-files-y += hwinfo.hex
hwinfo.hex-file := hwinfo.hex
hwinfo.hex-type := raw
hwinfo.hex-align := 0x1000

cbfs-files-y += hwinfo10.hex
hwinfo10.hex-file := hwinfo10.hex
hwinfo10.hex-type := raw

cbfs-files-y += hwinfo12.hex
hwinfo12.hex-file := hwinfo12.hex
hwinfo12.hex-type := raw

cbfs-files-y += hwinfo15.hex
hwinfo15.hex-file := hwinfo15.hex
hwinfo15.hex-type := raw

cbfs-files-y += hwinfo19.hex
hwinfo19.hex-file := hwinfo19.hex
hwinfo19.hex-type := raw
