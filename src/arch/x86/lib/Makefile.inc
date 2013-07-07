ramstage-y += c_start.S
ramstage-y += cpu.c
ramstage-y += pci_ops_conf1.c
ramstage-$(CONFIG_MMCONF_SUPPORT) += pci_ops_mmconf.c
ramstage-y += exception.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c
ramstage-y += memset.c
ramstage-y += memcpy.c
ramstage-y += memmove.c
ramstage-y += ebda.c
ramstage-y += rom_media.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread_switch.S

romstage-$(CONFIG_EARLY_CONSOLE) += romstage_console.c
romstage-y += cbfs_and_run.c
romstage-y += memset.c
romstage-y += memcpy.c
romstage-y += memmove.c
romstage-y += rom_media.c

smm-y += memset.c
smm-y += memcpy.c
smm-y += memmove.c
smm-y += rom_media.c

rmodules-y += memset.c
rmodules-y += memcpy.c
rmodules-y += memmove.c

$(obj)/arch/x86/lib/console.ramstage.o :: $(obj)/build.h
