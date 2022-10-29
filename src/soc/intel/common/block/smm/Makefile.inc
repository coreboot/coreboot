## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM) += smm.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM) += smm.c
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM) += smm.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM) += smm.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM) += smihandler.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM_IO_TRAP) += smitraphandler.c
