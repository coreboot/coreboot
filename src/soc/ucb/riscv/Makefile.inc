ifeq ($(CONFIG_SOC_UCB_RISCV),y)

romstage-y += cbmem.c

ramstage-y += cbmem.c

endif
