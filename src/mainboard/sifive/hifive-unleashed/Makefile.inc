# This file is part of the coreboot project.
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

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
