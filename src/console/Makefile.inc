ramstage-y += vtxprintf.c printk.c vsprintf.c
ramstage-y += init.c console.c
ramstage-y += post.c
ramstage-y += die.c
ifeq ($(CONFIG_HWBASE_DEBUG_CB),y)
ramstage-$(CONFIG_RAMSTAGE_LIBHWBASE) += hw-debug_sink.ads
ramstage-$(CONFIG_RAMSTAGE_LIBHWBASE) += hw-debug_sink.adb
romstage-$(CONFIG_ROMSTAGE_LIBHWBASE) += hw-debug_sink.ads
romstage-$(CONFIG_ROMSTAGE_LIBHWBASE) += hw-debug_sink.adb
endif

smm-$(CONFIG_DEBUG_SMI) += init.c console.c vtxprintf.c printk.c
smm-y += die.c
smm-y += post.c

ifneq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
verstage-y += printk.c
verstage-y += console.c
endif
verstage-y += post.c
verstage-y += die.c
verstage-y += init.c
verstage-y += vtxprintf.c vsprintf.c

romstage-y += vtxprintf.c printk.c vsprintf.c
romstage-y += init.c console.c
romstage-y += post.c
romstage-y += die.c

postcar-y += vtxprintf.c vsprintf.c
postcar-$(CONFIG_POSTCAR_CONSOLE) += printk.c
postcar-$(CONFIG_POSTCAR_CONSOLE) += init.c console.c
postcar-y += post.c
postcar-y += die.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += printk.c
bootblock-y += vtxprintf.c vsprintf.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += init.c console.c
bootblock-y += post.c
bootblock-y += die.c

decompressor-y += die.c
