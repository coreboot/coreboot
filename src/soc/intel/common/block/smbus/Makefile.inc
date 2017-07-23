bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbuslib.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus_early.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbuslib.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus_early.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbuslib.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_SMBUS) += smbus.c
