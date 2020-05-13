## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVER_INTEL_I210)+= i210.c

ifeq ($(CONFIG_DRIVER_INTEL_I210),y)
CFLAGS_x86_32 += -Isrc/drivers/intel/i210
endif
