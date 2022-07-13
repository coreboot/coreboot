##
##
## Copyright (C) 2016 Google Inc.
##
## SPDX-License-Identifier: GPL-2.0-only

cbfs-files-$(CONFIG_COREINFO_SECONDARY_PAYLOAD) += img/coreinfo
img/coreinfo-file := payloads/coreinfo/build/coreinfo.elf
img/coreinfo-type := payload
img/coreinfo-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

cbfs-files-$(CONFIG_NVRAMCUI_SECONDARY_PAYLOAD) += img/nvramcui
img/nvramcui-file := payloads/nvramcui/build/nvramcui.elf
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
payloads/external/edk2 \
payloads/external/GRUB2 \
payloads/external/LinuxBoot \
payloads/external/Yabits \
payloads/external/skiboot \
payloads/external/coreDOOM \

force-payload:

payloads/coreinfo/build/coreinfo.elf coreinfo: export CCACHE := $(CCACHE)
payloads/coreinfo/build/coreinfo.elf coreinfo: force-payload
	$(MAKE) -C payloads/coreinfo defaultbuild

payloads/nvramcui/build/nvramcui.elf nvramcui: export CCACHE := $(CCACHE)
payloads/nvramcui/build/nvramcui.elf nvramcui: force-payload
	$(MAKE) -C payloads/nvramcui

clean-payloads:
	$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) clean; )

distclean-payloads:
	$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) distclean; )

print-repo-info-payloads:
	-$(foreach payload, $(PAYLOADS_LIST), $(MAKE) -C $(payload) print-repo-info 2>/dev/null; )

ifeq ($(CONFIG_PAYLOAD_NONE),y)
files_added:: warn_no_payload
endif

warn_no_payload:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without a payload. Writing\n"
	printf "a coreboot image without a payload to your board's\n"
	printf "flash chip will result in a non-booting system. You\n"
	printf "can use cbfstool to add a payload to the image.\n\n"

.PHONY: force-payload coreinfo nvramcui
.PHONY: clean-payloads distclean-payloads print-repo-info-payloads warn_no_payload
