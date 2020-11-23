ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SMBUS),y)

romstage-y += smbus.c
ramstage-y += smbus.c
ramstage-y += sm.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SMBUS
