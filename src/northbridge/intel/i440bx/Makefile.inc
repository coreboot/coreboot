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

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_I440BX),y)

ramstage-y += northbridge.c
ramstage-y += ram_calc.c

romstage-y += raminit.c
romstage-$(CONFIG_DEBUG_RAM_SETUP) += debug.c
romstage-y += ram_calc.c

endif
