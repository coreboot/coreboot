ramstage-y += c_start.S
ramstage-y += cpu.c
ramstage-y += pci_ops_conf1.c
ramstage-y += pci_ops_conf2.c
ramstage-y += pci_ops_mmconf.c
ramstage-y += pci_ops_auto.c
ramstage-y += exception.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c

romstage-y += printk_init.c
romstage-y += cbfs_and_run.c

$(obj)/arch/x86/lib/console.ramstage.o :: $(obj)/build.h
