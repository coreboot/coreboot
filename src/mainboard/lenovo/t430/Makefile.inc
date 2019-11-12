bootblock-y += gpio.c
romstage-y += gpio.c
smm-y += smihandler.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
bootblock-y += early_init.c
romstage-y += early_init.c
