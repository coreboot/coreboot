##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 Advanced Micro Devices, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 2 as
## published by the Free Software Foundation.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_SOUTHBRIDGE_AMD_CS5536),y)

ramstage-y += cs5536.c
ramstage-y += ide.c
ramstage-y += pirq.c
ramstage-y += smbus.c
romstage-y += smbus.c

endif
