# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_X4X),y)

bootblock-y += bootblock.c

romstage-y += early_init.c
romstage-y += raminit.c
romstage-y += raminit_ddr23.c
romstage-y += memmap.c
romstage-y += rcven.c
romstage-y += raminit_tables.c
romstage-y += dq_dqs.c
romstage-y += romstage.c

ramstage-y += acpi.c
ramstage-y += memmap.c
ramstage-y += gma.c
ramstage-y += northbridge.c

postcar-y += memmap.c

endif
