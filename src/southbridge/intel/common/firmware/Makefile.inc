##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
## Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

ifeq ($(CONFIG_HAVE_INTEL_FIRMWARE),y)

# Run intermediate steps when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS

ifeq ($(CONFIG_HAVE_IFD_BIN),y)
INTERMEDIATE+=add_intel_firmware
endif

ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
INTERMEDIATE+=add_intel_firmware
IFD_BIN_PATH := $(objgenerated)/ifdfake.bin
IFD_SECTIONS := $(addprefix -b ,$(CONFIG_IFD_BIOS_SECTION:"%"=%)) \
		$(addprefix -m ,$(CONFIG_IFD_ME_SECTION:"%"=%)) \
		$(addprefix -g ,$(CONFIG_IFD_GBE_SECTION:"%"=%)) \
		$(addprefix -p ,$(CONFIG_IFD_PLATFORM_SECTION:"%"=%))
else
IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
ifneq ($(call strip_quotes,$(CONFIG_IFD_CHIPSET)),)
IFDTOOL_USE_CHIPSET := -p $(CONFIG_IFD_CHIPSET)
endif
endif

add_intel_firmware: $(obj)/coreboot.pre $(IFDTOOL) $(IFDFAKE)
ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "\n** WARNING **\n"
	printf "coreboot will be built with a fake Intel Firmware Descriptor (IFD).\n"
	printf "Never write a complete coreboot.rom with a fake IFD to your board's\n"
	printf "flash ROM! Make sure that you only write valid flash regions.\n\n"
	printf "    IFDFAKE    Building a fake Intel Firmware Descriptor\n"
	$(IFDFAKE) $(IFD_SECTIONS) $(IFD_BIN_PATH)
endif
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i ME:$(CONFIG_ME_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif
ifeq ($(CONFIG_CHECK_ME),y)
	util/me_cleaner/me_cleaner.py -c $(obj)/coreboot.pre > /dev/null
endif
ifeq ($(CONFIG_USE_ME_CLEANER),y)
	printf "    ME_CLEANER coreboot.pre\n"
	util/me_cleaner/me_cleaner.py $(obj)/coreboot.pre \
		$(patsubst "%,%,$(patsubst %",%,$(CONFIG_ME_CLEANER_ARGS))) > \
		$(obj)/me_cleaner.log
endif
ifeq ($(CONFIG_HAVE_GBE_BIN),y)
	printf "    IFDTOOL    gbe.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i GbE:$(CONFIG_GBE_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif
ifeq ($(CONFIG_HAVE_EC_BIN),y)
	printf "    IFDTOOL    ec.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i EC:$(CONFIG_EC_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else ifneq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool \
	$(IFDTOOL_USE_CHIPSET) -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

ifeq ($(CONFIG_EM100),y)
	printf "    IFDTOOL    Setting EM100 mode\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) --em100 $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

PHONY+=add_intel_firmware

endif
