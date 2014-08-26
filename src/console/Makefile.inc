ramstage-y += vtxprintf.c printk.c vsprintf.c
ramstage-y += init.c console.c
ramstage-y += post.c
ramstage-y += die.c

smm-$(CONFIG_DEBUG_SMI) += init.c console.c vtxprintf.c printk.c
smm-$(CONFIG_SMM_TSEG) += die.c

verstage-y += vtxprintf.c
verstage-y += console.c
verstage-y += die.c

romstage-y += vtxprintf.c printk.c
romstage-y += init.c console.c
romstage-y += post.c
romstage-y += die.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += vtxprintf.c printk.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += init.c console.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += die.c

secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += vtxprintf.c printk.c
secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += init.c console.c
secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += die.c
