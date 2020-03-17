##
## This file is part of the coreboot project.
##
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

CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/quark

bootblock-y += gpio.c
bootblock-y += reg_access.c

verstage-y += gpio.c
verstage-y += reg_access.c
verstage-$(CONFIG_VBOOT) += vboot.c

romstage-y += gpio.c
romstage-y += reg_access.c
romstage-$(CONFIG_COMMONLIB_STORAGE_SD) += sd.c
romstage-$(CONFIG_VBOOT) += vboot.c

postcar-y += gpio.c
postcar-y += reg_access.c

ramstage-y += gpio.c
ramstage-y += reg_access.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_SD) += sd.c
