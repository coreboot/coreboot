## SPDX-License-Identifier: GPL-2.0-only
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbuslib.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus_early.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbuslib.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus_early.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c

postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c
verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_TCO) += tco.c
