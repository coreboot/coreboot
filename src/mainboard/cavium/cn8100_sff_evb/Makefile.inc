##
## This file is part of the coreboot project.
##
## Copyright 2017-present Facebook, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

bootblock-y += bootblock.c
bootblock-y += memlayout.ld

romstage-y += memlayout.ld
romstage-y += romstage.c
romstage-y += bdk_devicetree.c

ramstage-y += mainboard.c
ramstage-y += memlayout.ld
ramstage-y += bdk_devicetree.c

verstage-y += memlayout.ld

MB_DIR = src/mainboard/$(MAINBOARDDIR)

LINUX_DTB = sff8104-linux.dtb
build/$(LINUX_DTB):
	# FIXME: why isn't this producing the correct size DTB?
	dtc -p 4096 -I dts -O dtb -o build/$(LINUX_DTB) -i $(MB_DIR) $(MB_DIR)/$(patsubst %.dtb,%.dts,$(LINUX_DTB))

cbfs-files-y += $(LINUX_DTB)
$(LINUX_DTB)-file := build/$(LINUX_DTB)
$(LINUX_DTB)-type := raw
