ifeq ($(CONFIG_ARCH_X86),y)

romstage-$(CONFIG_ROMSTAGE_VGA) += vga_io.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga_palette.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga_font_8x16.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga.c

ramstage-$(CONFIG_VGA) += vga_io.c
ramstage-$(CONFIG_VGA) += vga_palette.c
ramstage-$(CONFIG_VGA) += vga_font_8x16.c
ramstage-$(CONFIG_VGA) += vga.c

endif
