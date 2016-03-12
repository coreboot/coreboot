#
# This file is part of the coreboot project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)

romstage-y += hand_off_block.c
romstage-y += util.c
romstage-y += memory_init.c

ramstage-y += graphics.c
ramstage-y += hand_off_block.c
ramstage-y += notify.c
ramstage-y += silicon_init.c
ramstage-y += util.c

CPPFLAGS_common += -I$(src)/drivers/intel/fsp2_0/include

endif
