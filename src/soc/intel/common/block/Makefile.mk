## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK),y)

subdirs-y += ./*

CPPFLAGS_common += -I$(src)/soc/intel/common/block/include/

endif
