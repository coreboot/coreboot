## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82801GX),y)

bootblock-y += early_init.c
bootblock-y += bootblock.c

ramstage-y += i82801gx.c
ramstage-y += fadt.c
ramstage-y += ac97.c
ramstage-y += azalia.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += smbus.c
ramstage-y += usb.c
ramstage-y += usb_ehci.c

smm-y += smihandler.c

romstage-y += early_init.c
romstage-y += early_cir.c

CPPFLAGS_common += -I$(src)/southbridge/intel/i82801gx/include

endif
