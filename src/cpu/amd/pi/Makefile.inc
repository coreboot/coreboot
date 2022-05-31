# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_CPU_AMD_PI_00730F01) += 00730F01

romstage-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
postcar-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
ramstage-y += ../../../soc/amd/common/block/cpu/smm/smm_helper.c
