## SPDX-License-Identifier: GPL-2.0-only
subdirs-$(CONFIG_SOC_INTEL_COMMON_PCH_BASE) += ./*
subdirs-$(CONFIG_SOC_INTEL_COMMON_IBL_BASE) += ./*

ifeq ($(CONFIG_SOC_INTEL_COMMON_PCH_BASE),y)
CPPFLAGS_common += -I$(src)/soc/intel/common/pch/include/
endif

ifeq ($(CONFIG_SOC_INTEL_COMMON_IBL_BASE),y)
CPPFLAGS_common += -I$(src)/soc/intel/common/pch/include/
endif
