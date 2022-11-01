ramstage-$(CONFIG_AP_IN_SIPI_WAIT) += lapic_cpu_stop.c

bootblock-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
romstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
ramstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
postcar-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
bootblock-y += boot_cpu.c
verstage_x86-y += boot_cpu.c
romstage-y += boot_cpu.c
ramstage-y += boot_cpu.c
postcar-y += boot_cpu.c

bootblock-y += lapic.c
verstage_x86-y += lapic.c
romstage-y += lapic.c
ramstage-y += lapic.c
postcar-y += lapic.c
