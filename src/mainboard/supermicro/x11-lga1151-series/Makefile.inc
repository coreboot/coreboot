## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

ramstage-y += mainboard.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

subdirs-y += variants/$(VARIANT_DIR)
