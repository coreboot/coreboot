##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

bootblock-y += chromeos.c
verstage-y += chromeos.c
romstage-y += chromeos.c
ramstage-y += chromeos.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += gnvs.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
romstage-y += vpd_decode.c
ramstage-y += vpd_decode.c cros_vpd.c vpd_mac.c vpd_serialno.c vpd_calibration.c
ramstage-$(CONFIG_HAVE_REGULATORY_DOMAIN) += wrdd.c
ifeq ($(CONFIG_ARCH_MIPS),)
bootblock-y += watchdog.c
ramstage-y += watchdog.c
endif

ifeq ($(MOCK_TPM),1)
CFLAGS_common += -DMOCK_TPM=1
else
CFLAGS_common += -DMOCK_TPM=0
endif

CONFIG_GBB_HWID := $(call strip_quotes,$(CONFIG_GBB_HWID))
CONFIG_GBB_BMPFV_FILE := $(call strip_quotes,$(CONFIG_GBB_BMPFV_FILE))
CONFIG_VBOOT_KEYBLOCK := $(call strip_quotes,$(CONFIG_VBOOT_KEYBLOCK))
CONFIG_VBOOT_FIRMWARE_PRIVKEY := $(call strip_quotes,$(CONFIG_VBOOT_FIRMWARE_PRIVKEY))
CONFIG_VBOOT_KERNEL_KEY := $(call strip_quotes,$(CONFIG_VBOOT_KERNEL_KEY))
CONFIG_CHROMEOS_FWID_MODEL := $(call strip_quotes,$(CONFIG_CHROMEOS_FWID_MODEL))
CONFIG_CHROMEOS_FWID_VERSION := $(call strip_quotes,$(CONFIG_CHROMEOS_FWID_VERSION))

# bool-to-mask(var, value)
# return "value" if var is "y", 0 otherwise
bool-to-mask = $(if $(filter y,$(1)),$(2),0)

GBB_FLAGS := $(call int-add, \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DEV_SCREEN_SHORT_DELAY),0x1) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_LOAD_OPTION_ROMS),0x2) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_ENABLE_ALTERNATE_OS),0x4) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_DEV_SWITCH_ON),0x8) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_DEV_BOOT_USB),0x10) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_FW_ROLLBACK_CHECK),0x20) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_ENTER_TRIGGERS_TONORM),0x40) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_DEV_BOOT_LEGACY),0x80) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FAFT_KEY_OVERIDE),0x100) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_EC_SOFTWARE_SYNC),0x200) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DEFAULT_DEV_BOOT_LEGACY),0x400) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_PD_SOFTWARE_SYNC),0x800) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_LID_SHUTDOWN),0x1000) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_DEV_BOOT_FASTBOOT_FULL_CAP),0x2000) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_ENABLE_SERIAL),0x4000) \
	)

ifneq ($(CONFIG_GBB_BMPFV_FILE),)
$(obj)/gbb.sizetmp: $(obj)/coreboot.rom
	$(CBFSTOOL) $< read -r GBB -f $@

$(obj)/gbb.stub: $(obj)/coreboot.rom $(FUTILITY) $(obj)/gbb.sizetmp
	@printf "    CREATE GBB (with BMPFV)\n"
	$(FUTILITY) gbb_utility -c 0x100,0x1000,$(call int-subtract,$(call file-size,$(obj)/gbb.sizetmp) 0x2180),0x1000 $@.tmp
	mv $@.tmp $@
else
$(obj)/gbb.stub: $(obj)/coreboot.rom $(FUTILITY)
	@printf "    CREATE GBB (without BMPFV)\n"
	$(FUTILITY) gbb_utility -c 0x100,0x1000,0,0x1000 $@.tmp
	mv $@.tmp $@
endif

$(obj)/gbb.region: $(obj)/gbb.stub
	@printf "    SETUP GBB\n"
	cp $< $@.tmp
	$(FUTILITY) gbb_utility -s \
		--hwid="$(CONFIG_GBB_HWID)" \
		--rootkey="$(CONFIG_VBOOT_ROOT_KEY)" \
		--recoverykey="$(CONFIG_VBOOT_RECOVERY_KEY)" \
		--flags=$(GBB_FLAGS) \
		$@.tmp
ifneq ($(CONFIG_GBB_BMPFV_FILE),)
	$(FUTILITY) gbb_utility -s \
		--bmpfv="$(CONFIG_GBB_BMPFV_FILE)" \
		$@.tmp
endif
	mv $@.tmp $@

$(obj)/fwid.region:
	printf "$(CONFIG_CHROMEOS_FWID_MODEL)$(CONFIG_CHROMEOS_FWID_VERSION)\0" > $@

build_complete:: $(obj)/gbb.region $(obj)/fwid.region
	@printf "    WRITE GBB\n"
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r GBB -i 0 -f $(obj)/gbb.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RO_FRID -i 0 -f $(obj)/fwid.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RW_FWID_A -i 0 -f $(obj)/fwid.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RW_FWID_B -i 0 -f $(obj)/fwid.region

ifneq ($(shell grep "SHARED_DATA" "$(CONFIG_FMDFILE)"),)
build_complete::
	printf "\0" > $(obj)/shared_data.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r SHARED_DATA -i 0 -f $(obj)/shared_data.region
endif

# Extract FW_MAIN_? region and minimize it if the last file is empty, so it
# doesn't contain this empty file (that can have a significant size),
# improving a lot on hash times due to a smaller amount of data loaded from
# firmware storage.
# When passing the minimized image to vbutil_firmware, its length is recorded
# in the keyblock, and coreboot's vboot code clips the region_device to match,
# which prevents any potential extension attacks.
$(obj)/FW_MAIN_%.bin: $(obj)/coreboot.rom
	$(CBFSTOOL) $< read -r $(basename $(notdir $@)) -f $@.tmp
	$(CBFSTOOL) $(obj)/coreboot.rom print -k -r $(basename $(notdir $@)) | \
		tail -1 | \
		sed "s,^(empty)[[:space:]]\(0x[0-9a-f]*\)\tnull\t.*$$,\1," \
		> $@.tmp.size
	if [ -n "$$(cat $@.tmp.size)" ] && [ $$( printf "%d" $$(cat $@.tmp.size)) -gt 0 ]; then \
		head -c $$( printf "%d" $$(cat $@.tmp.size)) $@.tmp > $@.tmp2 && \
		mv $@.tmp2 $@; \
	else \
		mv $@.tmp $@; \
	fi

$(obj)/VBLOCK_%.bin: $(obj)/FW_MAIN_%.bin $(FUTILITY)
	$(FUTILITY) vbutil_firmware \
		--vblock $@ \
		--keyblock "$(CONFIG_VBOOT_KEYBLOCK)" \
		--signprivate "$(CONFIG_VBOOT_FIRMWARE_PRIVKEY)" \
		--version $(CONFIG_VBOOT_KEYBLOCK_VERSION) \
		--fv $< \
		--kernelkey "$(CONFIG_VBOOT_KERNEL_KEY)" \
		--flags $(CONFIG_VBOOT_KEYBLOCK_PREAMBLE_FLAGS)

files_added:: $(obj)/VBLOCK_A.bin $(obj)/VBLOCK_B.bin
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r VBLOCK_A -f $(obj)/VBLOCK_A.bin
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r VBLOCK_B -f $(obj)/VBLOCK_B.bin
