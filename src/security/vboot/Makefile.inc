## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_VBOOT_LIB),y)

bootblock-y += vboot_lib.c
verstage-y += vboot_lib.c
romstage-y += vboot_lib.c
ramstage-y += vboot_lib.c
postcar-y += vboot_lib.c

vboot-fixup-includes = $(patsubst -I%,-I$(top)/%,\
		       $(patsubst $(src)/%.h,$(top)/$(src)/%.h,\
		       $(filter-out -I$(obj),$(1))))

# call with $1 = stage name to create rules for building the library
# for the stage and adding it to the stage's set of object files.
define vboot-for-stage
VBOOT_LIB_$(1) = $(obj)/external/vboot_reference-$(1)/vboot_fw.a
VBOOT_CFLAGS_$(1) += $$(call vboot-fixup-includes,$$(CPPFLAGS_$(1)))
VBOOT_CFLAGS_$(1) += $$(CFLAGS_$(1))
VBOOT_CFLAGS_$(1) += $$(call vboot-fixup-includes,$$($(1)-c-ccopts))
VBOOT_CFLAGS_$(1) += -I$(abspath $(obj)) -Wno-missing-prototypes
VBOOT_CFLAGS_$(1) += -DVBOOT_DEBUG

$$(VBOOT_LIB_$(1)): $(obj)/config.h
	printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	+FIRMWARE_ARCH=$$(ARCHDIR-$$(ARCH-$(1)-y)) \
	CC="$$(CC_$(1))" \
	CFLAGS="$$(VBOOT_CFLAGS_$(1))" VBOOT2="y" \
	EC_EFS="$(CONFIG_VBOOT_EC_EFS)" \
	X86_SHA_EXT="$(if $(CONFIG_ARCH_$(call toupper,$(1))_X86_32)$(CONFIG_ARCH_$(call toupper,$(1))_X86_64),$\
			$(CONFIG_VBOOT_X86_SHA256_ACCELERATION))" \
	$(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$$(abspath $$(dir $$(VBOOT_LIB_$(1)))) \
		V=$(V) \
		fwlib \
		$(if $(CONFIG_SBOM_VBOOT),$$(abspath $$(dir $$(VBOOT_LIB_$(1))))/vboot_host.pc)

$(1)-srcs += $$(VBOOT_LIB_$(1))

endef # vboot-for-stage

$(eval $(call vboot-for-stage,bootblock))
$(eval $(call vboot-for-stage,romstage))
$(eval $(call vboot-for-stage,ramstage))
$(eval $(call vboot-for-stage,postcar))

endif # CONFIG_VBOOT_LIB

ifeq ($(CONFIG_VBOOT),y)

bootblock-y += bootmode.c
romstage-y += bootmode.c
ramstage-y += bootmode.c
verstage-y += bootmode.c
postcar-y += bootmode.c

verstage-generic-ccopts += -D__VERSTAGE__

bootblock-y += vbnv.c
verstage-y += vbnv.c
romstage-y += vbnv.c
ramstage-y += vbnv.c
postcar-y += vbnv.c

romstage-$(CONFIG_VBOOT_EARLY_EC_SYNC) += ec_sync.c

bootblock-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
verstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
romstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
ramstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
postcar-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c

$(call src-to-obj,bootblock,$(dir)/vbnv_flash.c) : $(obj)/fmap_config.h
$(call src-to-obj,verstage,$(dir)/vbnv_flash.c) : $(obj)/fmap_config.h
$(call src-to-obj,romstage,$(dir)/vbnv_flash.c) : $(obj)/fmap_config.h
$(call src-to-obj,ramstage,$(dir)/vbnv_flash.c) : $(obj)/fmap_config.h
$(call src-to-obj,postcar,$(dir)/vbnv_flash.c) : $(obj)/fmap_config.h
bootblock-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
verstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
romstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
ramstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
postcar-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c

bootblock-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
verstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
romstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
ramstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
postcar-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c

bootblock-y += vboot_loader.c
romstage-y += vboot_loader.c
ramstage-y += vboot_loader.c
verstage-y += vboot_loader.c
postcar-y += vboot_loader.c

bootblock-y += vboot_common.c
verstage-y += vboot_common.c
romstage-y += vboot_common.c
ramstage-y += vboot_common.c
postcar-y += vboot_common.c

bootblock-y += common.c
verstage-y += vboot_logic.c
verstage-y += common.c
ifeq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),)
verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += verstage.c
endif
ifeq (${CONFIG_VBOOT_MOCK_SECDATA},y)
verstage-y += secdata_mock.c
romstage-y += secdata_mock.c
ramstage-y += secdata_mock.c
else
verstage-y += secdata_tpm.c
romstage-y += secdata_tpm.c
ramstage-y += secdata_tpm.c
endif

verstage-$(CONFIG_TPM) += tpm_common.c

romstage-y += common.c

ramstage-y += common.c
postcar-y += common.c

romstage-$(CONFIG_MRC_SAVE_HASH_IN_TPM) += mrc_cache_hash_tpm.c
ramstage-$(CONFIG_MRC_SAVE_HASH_IN_TPM) += mrc_cache_hash_tpm.c

ifeq ($(CONFIG_VBOOT_SEPARATE_VERSTAGE),y)

$(eval $(call vboot-for-stage,verstage))

ifeq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),)
cbfs-files-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += $(CONFIG_CBFS_PREFIX)/verstage
$(CONFIG_CBFS_PREFIX)/verstage-file := $(objcbfs)/verstage.elf
$(CONFIG_CBFS_PREFIX)/verstage-type := stage
$(CONFIG_CBFS_PREFIX)/verstage-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
endif	# CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)
$(CONFIG_CBFS_PREFIX)/verstage-options := -a 64 -S ".car.data"

# If CAR does not support execution of code, verstage on x86 is expected to be
# xip.
ifneq ($(CONFIG_NO_XIP_EARLY_STAGES),y)
$(CONFIG_CBFS_PREFIX)/verstage-options += --xip
endif

endif
$(CONFIG_CBFS_PREFIX)/verstage-options += $(TXTIBB)

else # CONFIG_VBOOT_SEPARATE_VERSTAGE
ifeq ($(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK),y)
postinclude-hooks += $$(eval bootblock-srcs += $$(verstage-srcs))
else
postinclude-hooks += $$(eval romstage-srcs += $$(verstage-srcs))
endif
endif # CONFIG_VBOOT_SEPARATE_VERSTAGE

#RO-Partition is always there!
VBOOT_PARTITIONS := COREBOOT
# Check for RW_A partition
ifeq ($(CONFIG_VBOOT_SLOTS_RW_A),y)
VBOOT_PARTITIONS += FW_MAIN_A
RW_PARTITIONS := FW_MAIN_A
endif
# Check for RW_B partition
ifeq ($(CONFIG_VBOOT_SLOTS_RW_AB),y)
VBOOT_PARTITIONS += FW_MAIN_B
RW_PARTITIONS += FW_MAIN_B
endif

# Return the regions a specific file should be placed in. The files listed below and the ones
# that are specified in CONFIG_RO_REGION_ONLY, are only specified in the RO region. The files
# specified in the CONFIG_RW_REGION_ONLY are placed in all RW regions.  Files specified
# in CONFIG_RWA_REGION_ONLY or CONFIG_RWB_REGION_ONLY get placed only in those sections.
# All other files will be installed into RO and RW regions
# Use $(sort) to cut down on extra spaces that would be translated to commas
regions-for-file = $(subst $(spc),$(comma),$(sort \
	$(if $(value regions-for-file-$(1)), \
		$(regions-for-file-$(1)), \
		$(if $(filter $(if $(filter y,$(CONFIG_VBOOT_STARTS_IN_ROMSTAGE)), \
			%/romstage,) \
		header_pointer \
		cbfs_master_header \
		mts \
		%/verstage \
		locales \
		locale_%.bin \
		font.bin \
		vbgfx.bin \
		rmu.bin \
		cmos_layout.bin \
		cmos.default \
		intel_fit \
		intel_fit_ts \
		fspt.bin \
		$(call strip_quotes,$(CONFIG_RO_REGION_ONLY)) \
		,$(1)),COREBOOT,\
		$(if $(filter \
		$(call strip_quotes,$(CONFIG_RWA_REGION_ONLY)) \
		,$(1)), FW_MAIN_A, \
		$(if $(filter \
			$(call strip_quotes,$(CONFIG_RWB_REGION_ONLY)) \
			,$(1)), FW_MAIN_B, \
		$(if $(filter \
			$(call strip_quotes,$(CONFIG_RW_REGION_ONLY)) \
			,$(1)), $(RW_PARTITIONS), $(VBOOT_PARTITIONS) ) \
		))))))

CONFIG_GBB_HWID := $(call strip_quotes,$(CONFIG_GBB_HWID))
CONFIG_GBB_BMPFV_FILE := $(call strip_quotes,$(CONFIG_GBB_BMPFV_FILE))
CONFIG_VBOOT_KEYBLOCK := $(call strip_quotes,$(CONFIG_VBOOT_KEYBLOCK))
CONFIG_VBOOT_FIRMWARE_PRIVKEY := $(call strip_quotes,$(CONFIG_VBOOT_FIRMWARE_PRIVKEY))
CONFIG_VBOOT_KERNEL_KEY := $(call strip_quotes,$(CONFIG_VBOOT_KERNEL_KEY))
CONFIG_VBOOT_FWID_MODEL := $(call strip_quotes,$(CONFIG_VBOOT_FWID_MODEL))
CONFIG_VBOOT_FWID_VERSION := $(call strip_quotes,$(CONFIG_VBOOT_FWID_VERSION))

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
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_DEV_BOOT_ALTFW),0x80) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_RUNNING_FAFT),0x100) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_EC_SOFTWARE_SYNC),0x200) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DEFAULT_DEV_BOOT_ALTFW),0x400) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_PD_SOFTWARE_SYNC),0x800) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_LID_SHUTDOWN),0x1000) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_FORCE_MANUAL_RECOVERY),0x4000) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_DISABLE_FWMP),0x8000) \
	$(call bool-to-mask,$(CONFIG_GBB_FLAG_ENABLE_UDC),0x10000) \
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

# Generate a test-only HWID
ifeq ($(CONFIG_GBB_HWID),)
CONFIG_GBB_HWID := $$($(top)/util/chromeos/gen_test_hwid.sh "$(CONFIG_MAINBOARD_PART_NUMBER)")
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

$(obj)/fwid.version:
	echo -n "$(CONFIG_VBOOT_FWID_VERSION)" > $@

$(obj)/fwid.region: $(obj)/fwid.version
	printf "%s%s\0" \
		"$(CONFIG_VBOOT_FWID_MODEL)" \
		"$$(cat "$(obj)/fwid.version")" > $@

build_complete:: $(obj)/gbb.region $(obj)/fwid.region
	@printf "    WRITE GBB\n"
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r GBB -i 0 -f $(obj)/gbb.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RO_FRID -i 0 -f $(obj)/fwid.region
ifeq ($(CONFIG_VBOOT_SLOTS_RW_A),y)
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RW_FWID_A -i 0 -f $(obj)/fwid.region
endif
ifeq ($(CONFIG_VBOOT_SLOTS_RW_AB),y)
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r RW_FWID_B -i 0 -f $(obj)/fwid.region
endif

ifneq ($(shell grep "SHARED_DATA" "$(CONFIG_FMDFILE)"),)
build_complete::
	printf "\0" > $(obj)/shared_data.region
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r SHARED_DATA -i 0 -f $(obj)/shared_data.region
endif

fmap-section-offset-cmd = $(FUTILITY) dump_fmap -p $(obj)/coreboot.rom | \
					grep '^$(1) ' | cut '-d ' -f2
fmap-section-size-cmd =   $(FUTILITY) dump_fmap -p $(obj)/coreboot.rom | \
					grep '^$(1) ' | cut '-d ' -f3

ifeq ($(CONFIG_VBOOT_GSCVD),y)
#
# vboot-gscvd-ranges
#
# This variable expands to the list of ranges that will be verified by the GSC
# before releasing the SoC from reset. It needs to cover all security-relevant
# ranges of the flash that CBFS verification cannot cover itself. By default
# this is the `GBB` FMAP section (not handled here but through the special `-G`
# parameter to `futility gscvd` below) and the bootblock. Here we are
# initializing the variable to expansions that produce ranges for both the
# `BOOTBLOCK` FMAP section (filled up to the real size of
# `$(objcbfs)/bootblock.bin`) and the `bootblock` file in the primary CBFS --
# only one of those two should normally exist on a given platform.
#
# Platforms where the bootblock isn't the first and only thing loaded by the
# hardware or which otherwise have special security-relevant flash areas that
# cannot be covered normally by CBFS verification will need to manually add
# ranges to this variable in their own Makefiles, in the format produced by
# printf("%x:%x", start_offset, size). The variable is only expanded once in a
# recipe of the `files_added` target, so $(shell) expansions that depend on
# inspecting $(obj)/coreboot.rom (or any of its dependencies) are valid.
#
vboot-gscvd-ranges += $(shell ( \
	offset=$$($(call fmap-section-offset-cmd,BOOTBLOCK)) ;\
	if [ -n "$$offset" ]; then \
		size=$$(wc -c < $(objcbfs)/bootblock.bin) ;\
		printf "%x:%x" $$offset $$size ;\
	fi ;\
))
vboot-gscvd-ranges += $(shell ( \
	line=$$($(CBFSTOOL) $(obj)/coreboot.rom print -k | grep '^bootblock[[:space:]]') ;\
	if [ -n "$$line" ]; then \
		cbfs_start=$$($(call fmap-section-offset-cmd,COREBOOT)) ;\
		offset=$$(printf "$$line" | cut -f2) ;\
		size=$$(printf "$$line" | cut -f6) ;\
		printf "%x:%x" $$((cbfs_start + offset)) $$size ;\
	fi ;\
))
files_added:: $(FUTILITY)
	@printf "    WRITE GSCVD\n"
	gscvd_range_args="$(foreach range,$(vboot-gscvd-ranges),-R $(range))" ;\
	if [ -z "$$gscvd_range_args" ]; then \
		echo "ERROR: No valid GSCVD ranges detected in image!" ;\
		exit 1 ;\
	fi ;\
	$(FUTILITY) gscvd -G $$gscvd_range_args -b $(CONFIG_VBOOT_GSC_BOARD_ID) \
		-r "$(CONFIG_VBOOT_GSCVD_ROOT_PUBKEY)" \
		-p "$(CONFIG_VBOOT_GSCVD_PLATFORM_PRIVKEY)" \
		-k "$(CONFIG_VBOOT_GSCVD_PLATFORM_KEYBLOCK)" \
		$(obj)/coreboot.rom
endif

ifneq (,$(filter y,$(CONFIG_VBOOT_SLOTS_RW_A) $(CONFIG_VBOOT_SLOTS_RW_AB)))
files_added:: $(obj)/coreboot.rom $(FUTILITY) $(CBFSTOOL)
	CBFSTOOL="$(CBFSTOOL)" \
	$(FUTILITY) sign \
		--signprivate "$(CONFIG_VBOOT_FIRMWARE_PRIVKEY)" \
		--keyblock "$(CONFIG_VBOOT_KEYBLOCK)" \
		--kernelkey "$(CONFIG_VBOOT_KERNEL_KEY)" \
		--version $(CONFIG_VBOOT_KEYBLOCK_VERSION) \
		--flags $(CONFIG_VBOOT_KEYBLOCK_PREAMBLE_FLAGS) \
		$(obj)/coreboot.rom
	if [ "$(CONFIG_VBOOT_SLOTS_RW_AB)" = 'y' ]; then \
		printf "    FLASHMAP Layout generated for RO, A and B partition.\n"; \
	elif [ "$(CONFIG_VBOOT_SLOTS_RW_A)" = 'y' ]; then \
		printf "    FLASHMAP Layout generated for RO and A partition.\n"; \
	fi
else
files_added::
	@printf "    FLASHMAP Layout generated for RO partition only.\n"
	@printf "             Beware that there is no failure safety in case of update now!\n"
endif

endif # CONFIG_VBOOT
