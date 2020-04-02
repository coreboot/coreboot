## SPDX-License-Identifier: GPL-2.0-only
## This file is part of the coreboot project.

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_LYNXPOINT),y)

bootblock-y += bootblock.c

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += usb_xhci.c
ramstage-y += me_9.x.c
ramstage-y += smbus.c
ramstage-y += hda_verb.c
ramstage-$(CONFIG_INTEL_LYNXPOINT_LP) += serialio.c

ifneq ($(CONFIG_VARIANT_DIR),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/hda_verb.c
else
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c
endif

ramstage-y += rcba.c
ramstage-y += me_status.c
ramstage-y += acpi.c

ramstage-$(CONFIG_ELOG) += elog.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c pmutil.c
smm-y += smihandler.c me_9.x.c pch.c
smm-y += pmutil.c usb_ehci.c usb_xhci.c

bootblock-y += early_pch.c
romstage-y += early_usb.c early_smbus.c early_me.c me_status.c early_pch.c
romstage-y += rcba.c pmutil.c

ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
romstage-y += lp_gpio.c
ramstage-y += lp_gpio.c
smm-y += lp_gpio.c
endif

verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += pmutil.c
verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += lp_gpio.c

endif
