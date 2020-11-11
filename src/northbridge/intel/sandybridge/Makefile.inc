## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE),y)

bootblock-y += bootblock.c

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += pcie.c
ramstage-y += gma.c

ramstage-y += acpi.c

romstage-y += memmap.c

ramstage-y += common.c
romstage-y += common.c
smm-y += common.c

romstage-y += raminit_shared.c
ifeq ($(CONFIG_USE_NATIVE_RAMINIT),y)
romstage-y += early_dmi.c
romstage-y += raminit.c
romstage-y += raminit_common.c
romstage-y += raminit_iosav.c
romstage-y += raminit_native.c
romstage-y += raminit_tables.c
romstage-y += ../../../device/dram/ddr3.c
else
romstage-y += raminit_mrc.c
cbfs-files-y += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := 0xfffa0000
mrc.bin-type := mrc
endif
romstage-y += romstage.c
romstage-y += early_init.c

smm-y += finalize.c

postcar-y += memmap.c

endif
