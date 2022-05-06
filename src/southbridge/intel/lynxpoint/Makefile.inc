## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_LYNXPOINT),y)

bootblock-y += bootblock.c

ramstage-y += pch.c
ramstage-y += iobp.c
ramstage-y += azalia.c
ramstage-y += fadt.c
ramstage-y += lpc.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += usb_xhci.c
ramstage-y += me.c
ramstage-y += smbus.c
ramstage-y += hda_verb.c
ramstage-$(CONFIG_INTEL_LYNXPOINT_LP) += serialio.c

ifneq ($(CONFIG_VARIANT_DIR),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/hda_verb.c
endif

ramstage-y += me_status.c
ramstage-y += acpi.c

ramstage-$(CONFIG_ELOG) += elog.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c pmutil.c
smm-y += smihandler.c pch.c
smm-y += pmutil.c usb_ehci.c usb_xhci.c

bootblock-y += early_pch.c
romstage-y += early_usb.c early_me.c me_status.c early_pch.c
romstage-y += pmutil.c

romstage-$(CONFIG_USE_NATIVE_RAMINIT) += early_pch_native.c

ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
romstage-y += lp_gpio.c
ramstage-y += lp_gpio.c
smm-y += lp_gpio.c
verstage-y += lp_gpio.c
bootblock-$(CONFIG_SERIALIO_UART_CONSOLE) += uart_init.c
bootblock-$(CONFIG_SERIALIO_UART_CONSOLE) += iobp.c
all-$(CONFIG_SERIALIO_UART_CONSOLE) += uart.c
smm-$(CONFIG_SERIALIO_UART_CONSOLE) += uart.c
endif

verstage-y += pmutil.c

CPPFLAGS_common += -I$(src)/southbridge/intel/lynxpoint/include

endif
