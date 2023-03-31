## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI),y)

all_x86-y += acpi.c
smm-y += acpi.c

ramstage-y += pm_state.c
ramstage-y += tables.c
ramstage-$(CONFIG_ACPI_BERT) += bert.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_ALIB) += alib.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_CPPC) += cppc.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_CPU_POWER_STATE) += cpu_power_state.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_GPIO) += gpio.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_ACPI_IVRS) += ivrs.c

romstage-y += elog.c
ramstage-y += elog.c
smm-y += elog.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_ACPI
