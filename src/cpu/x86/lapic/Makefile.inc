ramstage-y += lapic.c
ramstage-y += lapic_cpu_init.c
ramstage-y += secondary.S
romstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
ramstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
romstage-y += boot_cpu.c
ramstage-y += boot_cpu.c
