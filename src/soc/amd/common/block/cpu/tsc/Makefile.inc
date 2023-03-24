## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM15H_16H) += cpufreq_15_16.c
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM17H_19H) += cpufreq_17_19.c
bootblock-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM1AH) += cpufreq_1a.c

verstage_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM15H_16H) += cpufreq_15_16.c
verstage_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM17H_19H) += cpufreq_17_19.c
verstage_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM1AH) += cpufreq_1a.c

romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM15H_16H) += cpufreq_15_16.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM17H_19H) += cpufreq_17_19.c
romstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM1AH) += cpufreq_1a.c

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM15H_16H) += cpufreq_15_16.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM17H_19H) += cpufreq_17_19.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM1AH) += cpufreq_1a.c

smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM15H_16H) += cpufreq_15_16.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM17H_19H) += cpufreq_17_19.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_CPUFREQ_FAM1AH) += cpufreq_1a.c

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_TSC),y)

bootblock-y += tsc_freq.c

verstage_x86-y += tsc_freq.c

romstage-y += tsc_freq.c

ramstage-y += tsc_freq.c

smm-y += tsc_freq.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_TSC
