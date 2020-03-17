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

romstage-y += spd_util.c

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES += micron_dimm_MT52L512M32D2PF-107	# 0b0000  8GB
SPD_SOURCES += micron_dimm_MT52L1G32D4PG-107	# 0b0001 16GB
