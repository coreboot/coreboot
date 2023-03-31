ramstage-$(CONFIG_AP_IN_SIPI_WAIT) += lapic_cpu_stop.c

bootblock-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
romstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
ramstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
postcar-$(CONFIG_UDELAY_LAPIC) += apic_timer.c

all_x86-y += boot_cpu.c
all_x86-y += lapic.c
