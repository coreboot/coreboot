bootblock-y += gpio.c

romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += lcd_panel.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
