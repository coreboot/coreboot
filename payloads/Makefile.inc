##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google Inc.
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

cbfs-files-$(CONFIG_COREINFO_SECONDARY_PAYLOAD) += img/coreinfo
img/coreinfo-file := payloads/coreinfo/build/coreinfo.elf
img/coreinfo-type := payload
img/coreinfo-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

cbfs-files-$(CONFIG_NVRAMCUI_SECONDARY_PAYLOAD) += img/nvramcui
img/nvramcui-file := payloads/nvramcui/nvramcui.elf
img/nvramcui-type := payload
img/nvramcui-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

PAYLOADS_LIST=\
payloads/coreinfo \
payloads/nvramcui \
payloads/libpayload \
payloads/external/depthcharge \
payloads/external/SeaBIOS \
payloads/external/U-Boot \
payloads/external/Memtest86Plus \
payloads/external/iPXE \
payloads/external/tint \
payloads/external/tianocore \
payloads/external/GRUB2 \
payloads/external/LinuxBoot \
payloads/external/Yabits \

payloads/coreinfo/build/coreinfo.elf coreinfo:
	$(MAKE) -C payloads/coreinfo defaultbuild

payloads/nvramcui/nvramcui.elf nvramcui:
	$(MAKE) -C payloads/nvramcui

clean-payloads:
	$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) clean; )

distclean-payloads:
	$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) distclean; )

print-repo-info-payloads:
	-$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) print-repo-info 2>/dev/null; )

.PHONY: clean-payloads distclean-payloads print-repo-info-payloads nvramcui coreinfo
