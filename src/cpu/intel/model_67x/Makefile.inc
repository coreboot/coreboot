##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Keith Hui <buurin@gmail.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ramstage-y += model_67x_init.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_67x/microcode.bin
