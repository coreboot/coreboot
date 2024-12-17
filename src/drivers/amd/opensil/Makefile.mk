# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_OPENSIL_DRIVER),y)

subdirs-y += mpio

ramstage-y += acpi.c
ramstage-y += ramstage.c

endif
