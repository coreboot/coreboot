obj-y += lapic.o
obj-y += lapic_cpu_init.o
obj-y += secondary.o
obj-$(CONFIG_UDELAY_LAPIC) += apic_timer.o
