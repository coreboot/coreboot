## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_TSC_FAM17H_19H),y)

bootblock-y += tsc_freq.c
bootblock-y += monotonic_timer.c

verstage_x86-y += tsc_freq.c
verstage_x86-y += monotonic_timer.c

romstage-y += tsc_freq.c
romstage-y += monotonic_timer.c

ramstage-y += tsc_freq.c
ramstage-y += monotonic_timer.c

smm-y += tsc_freq.c
smm-y += monotonic_timer.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_TSC_FAM17H_19H
