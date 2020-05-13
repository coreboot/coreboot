# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += memlayout.ld
bootblock-y += media.c

romstage-y += memlayout.ld
romstage-y += romstage.c
romstage-y += media.c

ramstage-y += memlayout.ld
ramstage-y += fixup_fdt.c
ramstage-y += media.c


DTB=$(obj)/hifive-unleashed.dtb
DTS=src/mainboard/sifive/hifive-unleashed/hifive-unleashed-a00.dts
$(DTB): $(DTS)
	dtc -I dts -O dtb -o $(DTB) $(DTS)

cbfs-files-y += fallback/DTB
fallback/DTB-file := $(DTB)
fallback/DTB-type := raw
