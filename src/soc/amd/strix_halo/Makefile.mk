# SPDX-License-Identifier: BSD-3-Clause

# TODO: Move as much as possible to common
# TODO: Update for Strix Halo

ifeq ($(CONFIG_SOC_AMD_STRIX_HALO_BASE),y)

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y		+= aoac.c
all-y		+= config.c
all-y		+= i2c.c

# all_x86-y adds the compilation unit to all stages that run on the x86 cores
all_x86-y	+= gpio.c
all_x86-y	+= i3c.c
all_x86-y	+= uart.c

bootblock-y	+= early_fch.c
bootblock-y	+= espi_util.c

verstage-y	+= espi_util.c

romstage-y	+= fsp_m_params.c

ramstage-y	+= acpi.c
ramstage-y	+= chip.c
ramstage-y	+= cpu.c
ramstage-y	+= fch.c
ramstage-y	+= fsp_s_params.c
ramstage-y	+= mca.c
ramstage-y	+= memmap.c
ramstage-y	+= root_complex.c
ramstage-y	+= xhci.c

smm-y		+= gpio.c
smm-y		+= root_complex.c
smm-y		+= smihandler.c
smm-$(CONFIG_DEBUG_SMI) += uart.c

CPPFLAGS_common += -I$(src)/soc/amd/strix_halo/include
CPPFLAGS_common += -I$(src)/soc/amd/strix_halo/acpi
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/strix_halo

# Target an offset into the CBFS. AMDFWTOOL will align it again and
# pad the space between the CBFS file header and the directory table.
# 0x80 accounts for the cbfs_file struct + filename + metadata structs
AMD_FW_AB_POSITION := 0x80

ifeq ($(CONFIG_PSP_AB_RECOVERY), y)
STRIX_HALO_FW_A_RECOVERY=$(call int-add, \
	$(call get_fmap_value,FMAP_SECTION_RECOVERY_A_START) $(AMD_FW_AB_POSITION))
STRIX_HALO_FW_B_RECOVERY=$(call int-add, \
	$(call get_fmap_value,FMAP_SECTION_RECOVERY_B_START) $(AMD_FW_AB_POSITION))
endif

#
# PSP Directory Table items
#
# Certain ordering requirements apply, however these are ensured by amdfwtool.
# For more information see "AMD Platform Security Processor BIOS Architecture
# Design Guide for AMD Family 17h Processors" (PID #55758, NDA only).
#

ifeq ($(CONFIG_PSP_DISABLE_POSTCODES),y)
PSP_SOFTFUSE_BITS += 7
endif

ifeq ($(CONFIG_PSP_UNLOCK_SECURE_DEBUG),y)
# Enable secure debug unlock
PSP_SOFTFUSE_BITS += 0
OPT_TOKEN_UNLOCK="--token-unlock"
endif

ifeq ($(CONFIG_PSP_LOAD_MP2_FW),y)
OPT_PSP_LOAD_MP2_FW="--load-mp2-fw"
else
# Disable MP2 firmware loading
PSP_SOFTFUSE_BITS += 29
endif

# Use additional Soft Fuse bits specified in Kconfig
PSP_SOFTFUSE_BITS += $(call strip_quotes, $(CONFIG_PSP_SOFTFUSE_BITS))

# type = 0x04
# The flashmap section used for this is expected to be named PSP_NVRAM
PSP_NVRAM_BASE=$(call get_fmap_value,FMAP_SECTION_PSP_NVRAM_START)
PSP_NVRAM_SIZE=$(call get_fmap_value,FMAP_SECTION_PSP_NVRAM_SIZE)

# type = 0x3a
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSP_WHITELIST_FILE=$(CONFIG_PSP_WHITELIST_FILE)
endif

# type = 0x54
# The flashmap section used for this is expected to be named PSP_RPMC_NVRAM
PSP_RPMC_NVRAM_BASE=$(call get_fmap_value,FMAP_SECTION_PSP_RPMC_NVRAM_START)
PSP_RPMC_NVRAM_SIZE=$(call get_fmap_value,FMAP_SECTION_PSP_RPMC_NVRAM_SIZE)

# type = 0x55
SPL_TABLE_FILE=$(CONFIG_SPL_TABLE_FILE)

#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x60
PSP_APCB_FILES=$(APCB_SOURCES) $(APCB_SOURCES_RECOVERY)

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DRAM_ADDRESS)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_ELF_FILE=$(objcbfs)/bootblock_fixed_data.elf
PSP_BIOSBIN_SIZE=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$5}')
PSP_BIOSBIN_DEST=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$3}')

ifneq ($(CONFIG_SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE),y)
# type = 0x63 - construct APOB NV base/size from flash map
# The flashmap section used for this is expected to be named RW_MRC_CACHE
APOB_NV_SIZE=$(call get_fmap_value,FMAP_SECTION_RW_MRC_CACHE_SIZE)
APOB_NV_BASE=$(call get_fmap_value,FMAP_SECTION_RW_MRC_CACHE_START)
endif # !CONFIG_SOC_AMD_COMMON_BLOCK_APOB_NV_DISABLE

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3)$(CONFIG_SMMSTORE),yy)
# Rom Armor needs the SMM Store region to be whitelisted
PSP_BIOS_NV_ST_BASE=$(call get_fmap_value,FMAP_SECTION_SMMSTORE_START)
PSP_BIOS_NV_ST_SIZE=$(call get_fmap_value,FMAP_SECTION_SMMSTORE_SIZE)
endif

# Helper function to return a value with given bit set
# Soft Fuse type = 0xb - See #55758 (NDA) for bit definitions.
set-bit=$(call int-shift-left, 1 $(call _toint,$1))
PSP_SOFTFUSE=$(shell A=$(call int-add, \
		$(foreach bit,$(sort $(PSP_SOFTFUSE_BITS)),$(call set-bit,$(bit)))); printf "0x%x" $$A)

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_PSP_NVRAM_BASE=$(call add_opt_prefix, $(PSP_NVRAM_BASE), --nvram-base)
OPT_PSP_NVRAM_SIZE=$(call add_opt_prefix, $(PSP_NVRAM_SIZE), --nvram-size)

OPT_PSP_RPMC_NVRAM_BASE=$(call add_opt_prefix, $(PSP_RPMC_NVRAM_BASE), --rpmc-nvram-base)
OPT_PSP_RPMC_NVRAM_SIZE=$(call add_opt_prefix, $(PSP_RPMC_NVRAM_SIZE), --rpmc-nvram-size)

OPT_PSP_APCB_FILES= $(if $(APCB_SOURCES), --instance 0 --apcb $(APCB_SOURCES)) \
                    $(if $(APCB_SOURCES_RECOVERY), --instance 10 --apcb $(APCB_SOURCES_RECOVERY)) \
                    $(if $(APCB_SOURCES_68), --instance 18 --apcb $(APCB_SOURCES_68))

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)

OPT_APOB_NV_SIZE=$(call add_opt_prefix, $(APOB_NV_SIZE), --apob-nv-size)
OPT_APOB_NV_BASE=$(call add_opt_prefix, $(APOB_NV_BASE),--apob-nv-base)
OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)
OPT_EFS_SPI_MICRON_FLAG=$(call add_opt_prefix, $(CONFIG_EFS_SPI_MICRON_FLAG), --spi-micron-flag)

OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)

OPT_WHITELIST_FILE=$(call add_opt_prefix, $(PSP_WHITELIST_FILE), --whitelist)
OPT_SPL_TABLE_FILE=$(call add_opt_prefix, $(SPL_TABLE_FILE), --spl-table)

OPT_BIOS_AMDCOMPRESS=$(if $(CONFIG_CBFS_VERIFICATION), --elfcopy, --compress)
OPT_BIOS_FWCOMPRESS=$(if $(CONFIG_CBFS_VERIFICATION), --bios-bin-uncomp)

OPT_BIOS_NV_ST_BASE=$(call add_opt_prefix, $(PSP_BIOS_NV_ST_BASE), --variable-nvram-base)
OPT_BIOS_NV_ST_SIZE=$(call add_opt_prefix, $(PSP_BIOS_NV_ST_SIZE), --variable-nvram-size)

OPT_RECOVERY_AB=$(if $(CONFIG_PSP_AB_RECOVERY), --recovery-ab)
OPT_RECOVERY_AB+=$(if $(CONFIG_PSP_AB_RECOVERY), --recovery-a-location $(call _tohex, $(STRIX_HALO_FW_A_RECOVERY)))
OPT_RECOVERY_AB+=$(if $(CONFIG_PSP_AB_RECOVERY), --recovery-b-location $(call _tohex, $(STRIX_HALO_FW_B_RECOVERY)))

AMDFW_COMMON_ARGS=$(OPT_PSP_APCB_FILES) \
		$(OPT_PSP_NVRAM_BASE) \
		$(OPT_PSP_NVRAM_SIZE) \
		$(OPT_PSP_RPMC_NVRAM_BASE) \
		$(OPT_PSP_RPMC_NVRAM_SIZE) \
		$(OPT_APOB_ADDR) \
		$(OPT_DEBUG_AMDFWTOOL) \
		$(OPT_PSP_BIOSBIN_FILE) \
		$(OPT_PSP_BIOSBIN_DEST) \
		$(OPT_PSP_BIOSBIN_SIZE) \
		$(OPT_PSP_SOFTFUSE) \
		$(OPT_PSP_LOAD_MP2_FW) \
		--use-pspsecureos \
		--load-s0i3 \
		$(OPT_TOKEN_UNLOCK) \
		$(OPT_WHITELIST_FILE) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_EFS_SPI_MICRON_FLAG) \
		$(OPT_RECOVERY_AB) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		$(OPT_BIOS_NV_ST_BASE) \
		$(OPT_BIOS_NV_ST_SIZE) \
		$(OPT_BIOS_FWCOMPRESS)

$(obj)/amdfw.rom:	$(call strip_quotes, $(PSP_BIOSBIN_FILE)) \
			$$(PSP_APCB_FILES) \
			$(DEP_FILES) \
			$(AMDFWTOOL) \
			$(obj)/fmap_config.h \
			$(objcbfs)/bootblock_fixed_data.elf # this target also creates the .map file
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		$(OPT_APOB_NV_SIZE) \
		$(OPT_APOB_NV_BASE) \
		$(OPT_SPL_TABLE_FILE) \
		--location $(CONFIG_AMD_FWM_POSITION) \
		--output $@

ifeq ($(CONFIG_PSP_AB_RECOVERY),y)

regions-for-file-apu/amdfw_ra = BOOTBLOCK
regions-for-file-apu/amdfw_rb = BOOTBLOCK_B

cbfs-files-y += apu/amdfw_ra
apu/amdfw_ra-file := $(obj)/amdfw.rom.ra
apu/amdfw_ra-position := $(AMD_FW_AB_POSITION)
apu/amdfw_ra-type := amdfw

cbfs-files-y += apu/amdfw_rb
apu/amdfw_rb-file := $(obj)/amdfw.rom.rb
apu/amdfw_rb-position := $(AMD_FW_AB_POSITION)
apu/amdfw_rb-type := amdfw

endif
#
# Extracts everything from the ELF's first PT_LOAD area and compresses it.
# This discards everything before PT_LOAD, every symbol, debug information
# and relocations. The generated binary is expected to run at PSP_BIOSBIN_DEST
# with a maximum size of PSP_BIOSBIN_SIZE. The entrypoint is fixed at
# PSP_BIOSBIN_DEST + PSP_BIOSBIN_SIZE - 0x10.
#
$(PSP_BIOSBIN_FILE): $(PSP_ELF_FILE) $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(PSP_ELF_FILE) --outfile $@ \
		$(OPT_BIOS_AMDCOMPRESS) --maxsize $(PSP_BIOSBIN_SIZE)

endif # ($(CONFIG_SOC_AMD_STRIX_HALO_BASE),y)
