romstage-y += early_southbridge.c
romstage-y += gpio.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
