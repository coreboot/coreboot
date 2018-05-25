##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

ramstage-y += slot_1.c
ramstage-y += l2_cache.c
subdirs-y += ../model_6xx
subdirs-y += ../model_65x
subdirs-y += ../model_67x
subdirs-y += ../model_68x
subdirs-y += ../model_6bx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode

cpu_incs-y += $(src)/cpu/intel/car/p3/cache_as_ram.S
romstage-y += ../car/romstage.c
