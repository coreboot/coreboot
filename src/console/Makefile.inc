ramstage-y += vtxprintf.c printk.c vsprintf.c
ramstage-y += init.c console.c
ramstage-y += post.c
ramstage-y += die.c

smm-$(CONFIG_DEBUG_SMI) += init.c console.c vtxprintf.c printk.c
smm-$(CONFIG_SMM_TSEG) += die.c

romstage-$(CONFIG_EARLY_CONSOLE) += vtxprintf.c printk.c
romstage-$(CONFIG_EARLY_CONSOLE) += init.c console.c
romstage-y += post.c
romstage-y += die.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += vtxprintf.c printk.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += init.c console.c
bootblock-y += die.c

$(obj)/console/init.smm.o : $(obj)/build.h
$(obj)/console/init.ramstage.o : $(obj)/build.h
$(obj)/console/init.romstage.o : $(obj)/build.h
$(obj)/console/init.bootblock.o : $(obj)/build.h
