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

# Add FSP blobs into cbfs. SoC code may supply  additional options with
# -options, e.g --xip or -b
cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(CONFIG_FSP_M_CBFS)
$(CONFIG_FSP_M_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_M_FILE))
$(CONFIG_FSP_M_CBFS)-type := fsp

cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(CONFIG_FSP_S_CBFS)
$(CONFIG_FSP_S_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_S_FILE))
$(CONFIG_FSP_S_CBFS)-type := fsp

endif
