romstage-y += romstage.c
romstage-y += gpio.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
