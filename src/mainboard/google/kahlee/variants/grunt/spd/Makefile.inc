##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
## Copyright (C) 2015 Intel Corporation.
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

LIB_SPD_DEPS = $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)

SPD_SOURCES =  empty				# 0b000
SPD_SOURCES += empty				# 1b001
SPD_SOURCES += empty				# 2b010
SPD_SOURCES += empty				# 3b011
SPD_SOURCES += empty				# 4b100
SPD_SOURCES += empty				# 5b101
SPD_SOURCES += empty				# 6b110
SPD_SOURCES += empty				# 7b111
