ramstage-y += common_init.c
romstage-$(CONFIG_UDELAY_LAPIC) += fsb.c
ramstage-$(CONFIG_UDELAY_LAPIC) += fsb.c
postcar-$(CONFIG_UDELAY_LAPIC) += fsb.c
smm-y += fsb.c
