## SPDX-License-Identifier: GPL-2.0-only

romstage-$(CONFIG_PLATFORM_HAS_DRAM_CLEAR) += memory.c
postcar-$(CONFIG_PLATFORM_HAS_DRAM_CLEAR) += memory.c
ramstage-$(CONFIG_PLATFORM_HAS_DRAM_CLEAR) += memory.c

ramstage-$(CONFIG_PLATFORM_HAS_DRAM_CLEAR) += memory_clear.c
