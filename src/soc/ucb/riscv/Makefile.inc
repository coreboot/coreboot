ifeq ($(CONFIG_SOC_UCB_RISCV),y)

bootblock-y += mtime.c
romstage-y += cbmem.c
ramstage-y += cbmem.c
ramstage-y += mtime.c

endif
