ifeq ($(CONFIG_SOC_LOWRISC_LOWRISC),y)

romstage-y += cbmem.c
ramstage-y += cbmem.c

endif
