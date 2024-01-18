## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82801DX),y)

bootblock-y += bootblock.c
bootblock-y += early_init.c

romstage-y += early_init.c

ramstage-y += i82801dx.c
ramstage-y += ac97.c
ramstage-y += fadt.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += usb.c
ramstage-y += usb2.c

smm-y += smihandler.c

endif
