ifeq ($(CONFIG_NORTHBRIDGE_INTEL_I82830),y)

ramstage-y += northbridge.c
ramstage-y += vga.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

endif
