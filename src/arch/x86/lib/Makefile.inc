ramstage-y += c_start.S
ramstage-y += cpu.c
ramstage-y += pci_ops_conf1.c
ramstage-y += pci_ops_conf2.c

ramstage-$(CONFIG_MMCONF_SUPPORT) += pci_ops_mmconf.c

ramstage-y += pci_ops_auto.c
ramstage-y += exception.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c
ramstage-y += memcpy.c

romstage-y += romstage_console.c
romstage-y += cbfs_and_run.c
romstage-y += memcpy.c

smm-y += memcpy.c

$(obj)/arch/x86/lib/console.ramstage.o :: $(obj)/build.h
