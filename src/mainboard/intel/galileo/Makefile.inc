##
## This file is part of the coreboot project.
##
## Copyright (C) 2015-2016 Intel Corp.
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

ifeq ($(CONFIG_PLATFORM_USES_FSP2_0)$(CONFIG_PLATFORM_USES_FSP1_1),y)
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/$(CONFIG_FSP_VERSION)/quark
endif

bootblock-y += gpio.c
bootblock-y += reg_access.c

romstage-y += gpio.c
romstage-y += reg_access.c

postcar-y += gpio.c
postcar-y += reg_access.c

ramstage-y += gpio.c
ramstage-y += reg_access.c
