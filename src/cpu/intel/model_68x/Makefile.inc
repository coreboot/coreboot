##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Joseph Smith <joe@settoplinux.org>
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

ramstage-y += model_68x_init.c
subdirs-y += ../../x86/name

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_68x/microcode.bin
