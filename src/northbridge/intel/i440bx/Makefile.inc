##
## This file is part of the coreboot project.
##
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

romstage-y += raminit.c
romstage-y += romstage.c
romstage-$(CONFIG_DEBUG_RAM_SETUP) += debug.c
romstage-y += memmap.c

endif
