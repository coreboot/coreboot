# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_OPENSIL_DRIVER),y)

subdirs-y += mpio

romstage-y += romstage.c

ramstage-y += acpi.c
ramstage-y += ramstage.c
ramstage-y += memmap.c

endif
