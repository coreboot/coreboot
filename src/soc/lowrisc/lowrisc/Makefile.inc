ifeq ($(CONFIG_SOC_LOWRISC_LOWRISC),y)

bootblock-y += mtime.c
romstage-y += cbmem.c
ramstage-y += cbmem.c
ramstage-y += mtime.c

endif
