ramstage-y += common_init.c
romstage-$(CONFIG_UDELAY_LAPIC) += fsb.c
ramstage-$(CONFIG_UDELAY_LAPIC) += fsb.c
postcar-$(CONFIG_UDELAY_LAPIC) += fsb.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += fsb.c
