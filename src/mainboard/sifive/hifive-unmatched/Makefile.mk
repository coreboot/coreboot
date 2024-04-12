# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += romstage.c
bootblock-y += cbfs_spi.c

ramstage-y += cbfs_spi.c
ramstage-y += fixup_fdt.c

DTB=$(obj)/hifive-unmatched.dtb
DTS=src/mainboard/sifive/hifive-unmatched/hifive-unmatched-a00.dts
$(DTB): $(DTS)
	dtc -I dts -O dtb -o $(DTB) $(DTS)

cbfs-files-y += fallback/DTB
fallback/DTB-file := $(DTB)
fallback/DTB-type := raw
