bootblock-y += gpio.c

ramstage-y += gpio.c
ramstage-y += nhlt.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
