##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
## Copyright (C) 2018 Intel Corporation.
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

SPD_SOURCES = empty				# 0b000
SPD_SOURCES += samsung_ddr4_4GB			# 0b001 Dual Channel 4GB
SPD_SOURCES += samsung_lpddr4_8GB		# 0b001 Dual Channel 8GB
SPD_SOURCES += empty				# 0b011
SPD_SOURCES += empty				# 0b100
SPD_SOURCES += empty				# 0b101
SPD_SOURCES += empty				# 0b110
SPD_SOURCES += empty				# 0b111
