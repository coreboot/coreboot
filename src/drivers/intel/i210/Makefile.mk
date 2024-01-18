## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVER_INTEL_I210)+= i210.c

ifeq ($(CONFIG_DRIVER_INTEL_I210),y)
CPPFLAGS_common += -I$(src)/drivers/intel/i210/include
endif
