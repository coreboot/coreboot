ramstage-$(CONFIG_UDELAY_TSC) += delay_tsc.c
romstage-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += delay_tsc.c

