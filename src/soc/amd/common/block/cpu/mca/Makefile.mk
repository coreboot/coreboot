## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_MCA_COMMON),y)
ramstage-y += mca_common.c
ramstage-$(CONFIG_ACPI_BERT) += mca_common_bert.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_MCA_COMMON

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_MCA),y)
ramstage-$(CONFIG_ACPI_BERT) += mca_bert.c
ramstage-y += mca.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_MCA

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_MCAX),y)
ramstage-$(CONFIG_ACPI_BERT) += mcax_bert.c
ramstage-y += mcax.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_MCAX
