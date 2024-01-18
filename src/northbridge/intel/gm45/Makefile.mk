# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_GM45),y)

bootblock-y += bootblock.c

romstage-y += early_init.c
romstage-y += early_reset.c
romstage-y += raminit.c
romstage-y += raminit_rcomp_calibration.c
romstage-y += raminit_receive_enable_calibration.c
romstage-y += raminit_read_write_training.c
romstage-y += pcie.c
romstage-y += thermal.c
romstage-y += igd.c
romstage-y += pm.c
romstage-y += memmap.c
romstage-y += iommu.c
romstage-y += romstage.c

ramstage-y += acpi.c

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += gma.c

postcar-y += memmap.c

endif
