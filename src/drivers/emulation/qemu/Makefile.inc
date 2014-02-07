ifeq ($(CONFIG_CONSOLE_QEMU_DEBUGCON),y)
ifeq ($(CONFIG_BOARD_EMULATION_QEMU_X86),y)
romstage-$(CONFIG_EARLY_CONSOLE) += qemu_debugcon.c
ramstage-y += qemu_debugcon.c
endif
ifeq ($(CONFIG_BOARD_EMULATION_QEMU_ARMV7),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += qemu_pl011_uart.c
romstage-$(CONFIG_EARLY_CONSOLE) += qemu_pl011_uart.c
ramstage-y += qemu_pl011_uart.c
endif
endif

ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_BOCHS) += bochs.c
ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_BOCHS) += cirrus.c
ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_BOCHS) += ../../pc80/vga/vga_io.c
