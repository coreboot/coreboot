ifeq ($(CONFIG_SOC_UCB_RISCV),y)

bootblock-y += ipi.c

romstage-y += cbmem.c
romstage-y += ipi.c

ramstage-y += cbmem.c
ramstage-y += ipi.c

endif
