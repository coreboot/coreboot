## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON),y)

subdirs-y += ./*

CPPFLAGS_common += -I$(src)/soc/amd/common/block/include/

endif
