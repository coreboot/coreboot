## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_ACPI),y)

bootblock-y += ec.c
verstage-y += ec.c
romstage-y += ec.c
ramstage-y += ec.c
smm-y += ec.c

endif
