## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_HAVE_INTEL_FIRMWARE),y)

# Run intermediate steps when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS

ifeq ($(CONFIG_HAVE_IFD_BIN),y)
$(call add_intermediate, add_intel_firmware)
else ifeq ($(CONFIG_INTEL_DESCRIPTOR_MODE_REQUIRED),y)
files_added:: warn_intel_firmware
endif

IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
ifneq ($(call strip_quotes,$(CONFIG_IFD_CHIPSET)),)
IFDTOOL_USE_CHIPSET := -p $(CONFIG_IFD_CHIPSET)
endif

ifeq ($(CONFIG_ME_REGION_ALLOW_CPU_READ_ACCESS),y)
IFDTOOL_LOCK_ME_MODE := -lr
else
IFDTOOL_LOCK_ME_MODE := -l
endif

add_intel_firmware: $(call strip_quotes,$(CONFIG_IFD_BIN_PATH))
ifeq ($(CONFIG_HAVE_ME_BIN),y)

OBJ_ME_BIN := $(obj)/me.bin

ifneq ($(CONFIG_STITCH_ME_BIN),y)

$(OBJ_ME_BIN): $(call strip_quotes,$(CONFIG_ME_BIN_PATH))
	cp $< $@

endif

add_intel_firmware: $(OBJ_ME_BIN)

endif
ifeq ($(CONFIG_HAVE_GBE_BIN),y)
add_intel_firmware: $(call strip_quotes,$(CONFIG_GBE_BIN_PATH))
endif
ifeq ($(CONFIG_HAVE_EC_BIN),y)
add_intel_firmware: $(call strip_quotes,$(CONFIG_EC_BIN_PATH))
endif
add_intel_firmware: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
ifeq ($(CONFIG_VALIDATE_INTEL_DESCRIPTOR),y)
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-t $(obj)/coreboot.pre
endif
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i ME:$(OBJ_ME_BIN) \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
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
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif
ifeq ($(CONFIG_HAVE_EC_BIN),y)
	printf "    IFDTOOL    ec.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i EC:$(CONFIG_EC_BIN_PATH) \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif
ifeq ($(CONFIG_HAVE_10GBE_0_BIN),y)
	printf "    IFDTOOL    10gbe0.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i 10GbE_0:$(CONFIG_10GBE_0_BIN_PATH) \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif
ifeq ($(CONFIG_HAVE_10GBE_1_BIN),y)
	printf "    IFDTOOL    10gbe1.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) \
		-i 10GbE_1:$(CONFIG_10GBE_1_BIN_PATH) \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) $(IFDTOOL_LOCK_ME_MODE) \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif
ifeq ($(CONFIG_UNLOCK_FLASH_REGIONS),y)
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool \
	$(IFDTOOL_USE_CHIPSET) -u \
	-O $(obj)/coreboot.pre \
	$(obj)/coreboot.pre
endif

ifeq ($(CONFIG_EM100),y)
	printf "    IFDTOOL    Setting EM100 mode\n"
	$(objutil)/ifdtool/ifdtool \
		$(IFDTOOL_USE_CHIPSET) --em100 \
		-O $(obj)/coreboot.pre \
		$(obj)/coreboot.pre
endif

warn_intel_firmware:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without an Intel Firmware Descriptor.\n"
	printf "Never write a complete coreboot.rom without an IFD to your\n"
	printf "board's flash chip! You can use flashrom's IFD or layout\n"
	printf "parameters to flash only to the BIOS region.\n\n"

PHONY+=warn_intel_firmware

endif
