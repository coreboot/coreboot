# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_IRONLAKE),y)

bootblock-y += bootblock.c

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += smi.c
ramstage-y += gma.c

ramstage-y += acpi.c

romstage-y += memmap.c
romstage-y += raminit.c
romstage-y += raminit_tables.c
romstage-y += early_init.c
romstage-y += romstage.c
romstage-y += quickpath.c

smm-y += finalize.c

postcar-y += memmap.c

endif
