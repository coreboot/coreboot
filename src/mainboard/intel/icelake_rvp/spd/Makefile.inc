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

SPD_SOURCES = empty	# 0b000
SPD_SOURCES += samsung_K4F6E304HBMGCJ	# 1b001
SPD_SOURCES += empty	# 2b010
SPD_SOURCES += empty	# 3b011
SPD_SOURCES += samsung_K4F6E304HBMGCJ	# 4b100
SPD_SOURCES += empty	# 5b101
SPD_SOURCES += samsung_K4F6E304HBMGCJ	# 6b110
SPD_SOURCES += empty	# 7b111
