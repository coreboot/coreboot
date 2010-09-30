ramstage-y += lapic.c
ramstage-y += lapic_cpu_init.c
ramstage-y += secondary.S
ramstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
