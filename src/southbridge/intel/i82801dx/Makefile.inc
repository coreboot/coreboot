## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82801DX),y)

ramstage-y += i82801dx.c
ramstage-y += ac97.c
ramstage-y += fadt.c
ramstage-y += ide.c
ramstage-y += lpc.c
#ramstage-y += pci.c
ramstage-y += usb.c
ramstage-y += usb2.c

ifeq ($(CONFIG_SMM_ASEG),y)
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm/smmrelocate.S
endif

smm-y += smihandler.c

romstage-y += early_smbus.c

bootblock-y += bootblock.c

endif
