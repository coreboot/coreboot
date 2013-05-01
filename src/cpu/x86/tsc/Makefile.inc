ramstage-$(CONFIG_UDELAY_TSC) += delay_tsc.c
romstage-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
smm-$(CONFIG_TSC_CONSTANT_RATE) += delay_tsc.c
endif

