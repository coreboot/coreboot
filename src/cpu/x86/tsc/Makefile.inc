bootblock-$(CONFIG_UDELAY_TSC) += delay_tsc.c
ramstage-$(CONFIG_UDELAY_TSC) += delay_tsc.c
romstage-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
verstage-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
postcar-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
smm-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
