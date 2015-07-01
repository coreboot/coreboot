ramstage-y += lapic.c
ramstage-y += lapic_cpu_init.c
ramstage-y += secondary.S
romstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
ramstage-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
ifeq ($(CONFIG_LAPIC_MONOTONIC_TIMER),y)
smm-$(CONFIG_UDELAY_LAPIC) += apic_timer.c
endif
romstage-y += boot_cpu.c
ramstage-y += boot_cpu.c
