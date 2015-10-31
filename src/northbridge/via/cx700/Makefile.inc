##
## This file is part of the coreboot project.
##
## Copyright (C) 2007-2009 coresystems GmbH
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

ifeq ($(CONFIG_NORTHBRIDGE_VIA_CX700),y)

ramstage-y += reset.c
ramstage-y += northbridge.c

ramstage-y += agp.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += vga.c

endif
