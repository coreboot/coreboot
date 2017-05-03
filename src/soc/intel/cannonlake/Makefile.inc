ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE),y)

romstage-y += cbmem.c

ramstage-y += cbmem.c

endif
