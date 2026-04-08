# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_GM965),y)

bootblock-y += bootblock.c

romstage-y += early_init.c
romstage-y += raminit.c
romstage-y += raminit_receive_enable_calibration.c
romstage-y += raminit_meminfo.c
romstage-y += dmi.c
romstage-y += thermal.c
romstage-y += pm.c
romstage-y += igd.c
romstage-y += romstage.c
romstage-y += memmap.c

ramstage-y += acpi.c
ramstage-y += gma.c
ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += pcie.c

postcar-y += memmap.c

endif
