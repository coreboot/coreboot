# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY15_TN) += family15tn
subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY16_KB) += family16kb

romstage-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
postcar-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
ramstage-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
