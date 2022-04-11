ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_DMI), y)

bootblock-y += gpmr.c
romstage-y += gpmr.c
ramstage-y += gpmr.c

endif
