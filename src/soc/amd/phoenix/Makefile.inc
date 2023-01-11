# SPDX-License-Identifier: BSD-3-Clause

# TODO: Move as much as possible to common
# TODO: Update for Phoenix

ifeq ($(CONFIG_SOC_AMD_PHOENIX),y)

subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += psp_verstage

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y		+= config.c
all-y		+= aoac.c

bootblock-y	+= early_fch.c
bootblock-y	+= espi_util.c
bootblock-y	+= gpio.c
bootblock-y	+= i2c.c
bootblock-y	+= reset.c
bootblock-y	+= uart.c

verstage-y	+= i2c.c
verstage-y	+= espi_util.c
verstage_x86-y	+= gpio.c
verstage_x86-y	+= reset.c
verstage_x86-y	+= uart.c

romstage-y	+= fsp_m_params.c
romstage-y	+= gpio.c
romstage-y	+= i2c.c
romstage-y	+= reset.c
romstage-y	+= romstage.c
romstage-y	+= uart.c

ramstage-y	+= acpi.c
ramstage-y	+= agesa_acpi.c
ramstage-y	+= chip.c
ramstage-y	+= cpu.c
ramstage-y	+= fch.c
ramstage-y	+= fsp_s_params.c
ramstage-y	+= gpio.c
ramstage-y	+= i2c.c
ramstage-y	+= mca.c
ramstage-y	+= reset.c
ramstage-y	+= root_complex.c
ramstage-y	+= uart.c
ramstage-y	+= xhci.c

smm-y		+= gpio.c
smm-y		+= smihandler.c
smm-y		+= smu.c
smm-$(CONFIG_DEBUG_SMI) += uart.c

CPPFLAGS_common += -I$(src)/soc/amd/phoenix/include
CPPFLAGS_common += -I$(src)/soc/amd/phoenix/acpi
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/phoenix
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/common

MAINBOARD_BLOBS_DIR:=$(top)/3rdparty/blobs/mainboard/$(MAINBOARDDIR)

# ROMSIG Normally At ROMBASE + 0x20000
# Overridden by CONFIG_AMD_FWM_POSITION_INDEX
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |               |                |            |
# +-----------+---------------+----------------+------------+
# |           |  PSPDIR ADDR  |  BIOSDIR ADDR  |
# +-----------+---------------+----------------+

$(if $(CONFIG_AMD_FWM_POSITION_INDEX), ,\
     $(error Invalid AMD firmware position index. Check if the board sets a valid ROM size))

PHOENIX_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)

# 0x40 accounts for the cbfs_file struct + filename + metadata structs, aligned to 64 bytes
# Building the cbfs image will fail if the offset isn't large enough
AMD_FW_AB_POSITION := 0x40

PHOENIX_FW_A_POSITION=$(call int-add, \
	$(shell awk '$$2 == "FMAP_SECTION_FW_MAIN_A_START" {print $$3}' $(obj)/fmap_config.h) \
	$(AMD_FW_AB_POSITION))

PHOENIX_FW_B_POSITION=$(call int-add, \
	$(shell awk '$$2 == "FMAP_SECTION_FW_MAIN_B_START" {print $$3}' $(obj)/fmap_config.h) \
	$(AMD_FW_AB_POSITION))
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

# type = 0x3a
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSP_WHITELIST_FILE=$(CONFIG_PSP_WHITELIST_FILE)
endif

# type = 0x55
ifeq ($(CONFIG_HAVE_SPL_FILE),y)
SPL_TABLE_FILE=$(CONFIG_SPL_TABLE_FILE)
ifeq ($(CONFIG_HAVE_SPL_RW_AB_FILE),y)
SPL_RW_AB_TABLE_FILE=$(CONFIG_SPL_RW_AB_TABLE_FILE)
else
SPL_RW_AB_TABLE_FILE=$(CONFIG_SPL_TABLE_FILE)
endif
endif

#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x60
PSP_APCB_FILES=$(APCB_SOURCES) $(APCB_SOURCES_RECOVERY)

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DRAM_ADDRESS)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_ELF_FILE=$(objcbfs)/bootblock.elf
PSP_BIOSBIN_SIZE=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$5}')
PSP_BIOSBIN_DEST=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$3}')

# type = 0x63 - construct APOB NV base/size from flash map
# The flashmap section used for this is expected to be named RW_MRC_CACHE
APOB_NV_SIZE=$(shell awk '$$2 == "FMAP_SECTION_RW_MRC_CACHE_SIZE" {print $$3}' $(obj)/fmap_config.h)
APOB_NV_BASE=$(shell awk '$$2 == "FMAP_SECTION_RW_MRC_CACHE_START" {print $$3}' $(obj)/fmap_config.h)

ifeq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
# type = 0x6B - PSP Shared memory location
ifneq ($(CONFIG_PSP_SHAREDMEM_SIZE),0x0)
PSP_SHAREDMEM_SIZE=$(CONFIG_PSP_SHAREDMEM_SIZE)
PSP_SHAREDMEM_BASE=$(shell awk '$$3 == "_psp_sharedmem_dram" {printf "0x" $$1}' $(objcbfs)/bootblock.map)
endif

# type = 0x52 - PSP Bootloader Userspace Application (verstage)
PSP_VERSTAGE_FILE=$(call strip_quotes,$(CONFIG_PSP_VERSTAGE_FILE))
PSP_VERSTAGE_SIG_FILE=$(call strip_quotes,$(CONFIG_PSP_VERSTAGE_SIGNING_TOKEN))
endif # CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK

ifeq ($(CONFIG_SEPARATE_SIGNED_PSPFW),y)
SIGNED_AMDFW_A_POSITION=$(call int-subtract, \
	$(shell awk '$$2 == "FMAP_SECTION_SIGNED_AMDFW_A_START" {print $$3}' $(obj)/fmap_config.h) \
	$(shell awk '$$2 == "FMAP_SECTION_FLASH_START" {print $$3}' $(obj)/fmap_config.h))
SIGNED_AMDFW_B_POSITION=$(call int-subtract, \
	$(shell awk '$$2 == "FMAP_SECTION_SIGNED_AMDFW_B_START" {print $$3}' $(obj)/fmap_config.h) \
	$(shell awk '$$2 == "FMAP_SECTION_FLASH_START" {print $$3}' $(obj)/fmap_config.h))
SIGNED_AMDFW_A_FILE=$(obj)/amdfw_a.rom.signed
SIGNED_AMDFW_B_FILE=$(obj)/amdfw_b.rom.signed
endif # CONFIG_SEPARATE_SIGNED_PSPFW

# Helper function to return a value with given bit set
# Soft Fuse type = 0xb - See #55758 (NDA) for bit definitions.
set-bit=$(call int-shift-left, 1 $(call _toint,$1))
PSP_SOFTFUSE=$(shell A=$(call int-add, \
		$(foreach bit,$(PSP_SOFTFUSE_BITS),$(call set-bit,$(bit)))); printf "0x%x" $$A)

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_VERSTAGE_FILE=$(call add_opt_prefix, $(PSP_VERSTAGE_FILE), --verstage)
OPT_VERSTAGE_SIG_FILE=$(call add_opt_prefix, $(PSP_VERSTAGE_SIG_FILE), --verstage_sig)

OPT_PSP_APCB_FILES= $(if $(APCB_SOURCES), --instance 0 --apcb $(APCB_SOURCES)) \
                    $(if $(APCB_SOURCES_RECOVERY), --instance 10 --apcb $(APCB_SOURCES_RECOVERY)) \
                    $(if $(APCB_SOURCES_68), --instance 18 --apcb $(APCB_SOURCES_68))

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)

OPT_PSP_SHAREDMEM_BASE=$(call add_opt_prefix, $(PSP_SHAREDMEM_BASE), --sharedmem)
OPT_PSP_SHAREDMEM_SIZE=$(call add_opt_prefix, $(PSP_SHAREDMEM_SIZE), --sharedmem-size)
OPT_APOB_NV_SIZE=$(call add_opt_prefix, $(APOB_NV_SIZE), --apob-nv-size)
OPT_APOB_NV_BASE=$(call add_opt_prefix, $(APOB_NV_BASE),--apob-nv-base)
OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)
OPT_EFS_SPI_MICRON_FLAG=$(call add_opt_prefix, $(CONFIG_EFS_SPI_MICRON_FLAG), --spi-micron-flag)

OPT_SIGNED_AMDFW_A_POSITION=$(call add_opt_prefix, $(SIGNED_AMDFW_A_POSITION), --signed-addr)
OPT_SIGNED_AMDFW_A_FILE=$(call add_opt_prefix, $(SIGNED_AMDFW_A_FILE), --signed-output)
OPT_SIGNED_AMDFW_B_POSITION=$(call add_opt_prefix, $(SIGNED_AMDFW_B_POSITION), --signed-addr)
OPT_SIGNED_AMDFW_B_FILE=$(call add_opt_prefix, $(SIGNED_AMDFW_B_FILE), --signed-output)

OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)

OPT_WHITELIST_FILE=$(call add_opt_prefix, $(PSP_WHITELIST_FILE), --whitelist)
OPT_SPL_TABLE_FILE=$(call add_opt_prefix, $(SPL_TABLE_FILE), --spl-table)
OPT_SPL_RW_AB_TABLE_FILE=$(call add_opt_prefix, $(SPL_RW_AB_TABLE_FILE), --spl-table)

# If vboot uses 2 RW slots, then 2 copies of PSP binaries are redundant
OPT_RECOVERY_AB_SINGLE_COPY=$(if $(CONFIG_VBOOT_SLOTS_RW_AB), --recovery-ab-single-copy)

AMDFW_COMMON_ARGS=$(OPT_PSP_APCB_FILES) \
		$(OPT_APOB_ADDR) \
		$(OPT_PSP_BIOSBIN_FILE) \
		$(OPT_PSP_BIOSBIN_DEST) \
		$(OPT_PSP_BIOSBIN_SIZE) \
		$(OPT_PSP_SOFTFUSE) \
		$(OPT_PSP_LOAD_MP2_FW) \
		--use-pspsecureos \
		--load-s0i3 \
		--combo-capable \
		$(OPT_TOKEN_UNLOCK) \
		$(OPT_WHITELIST_FILE) \
		$(OPT_PSP_SHAREDMEM_BASE) \
		$(OPT_PSP_SHAREDMEM_SIZE) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_EFS_SPI_MICRON_FLAG) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--soc-name "Phoenix" \
		--flashsize $(CONFIG_ROM_SIZE) \
		$(OPT_RECOVERY_AB_SINGLE_COPY)

$(obj)/amdfw.rom:	$(call strip_quotes, $(PSP_BIOSBIN_FILE)) \
			$(PSP_VERSTAGE_FILE) \
			$(PSP_VERSTAGE_SIG_FILE) \
			$$(PSP_APCB_FILES) \
			$(DEP_FILES) \
			$(AMDFWTOOL) \
			$(obj)/fmap_config.h \
			$(objcbfs)/bootblock.elf # this target also creates the .map file
	$(if $(PSP_APCB_FILES), ,$(error APCB_SOURCES is not set))
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		$(OPT_APOB_NV_SIZE) \
		$(OPT_APOB_NV_BASE) \
		$(OPT_VERSTAGE_FILE) \
		$(OPT_VERSTAGE_SIG_FILE) \
		$(OPT_SPL_TABLE_FILE) \
		--location $(shell printf "%#x" $(PHOENIX_FWM_POSITION)) \
		--output $@

$(PSP_BIOSBIN_FILE): $(PSP_ELF_FILE) $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(PSP_ELF_FILE) --outfile $@ --compress \
		--maxsize $(PSP_BIOSBIN_SIZE)

$(obj)/amdfw_a.rom: $(obj)/amdfw.rom
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		$(OPT_APOB_NV_SIZE) \
		$(OPT_APOB_NV_BASE) \
		$(OPT_SPL_RW_AB_TABLE_FILE) \
		$(OPT_SIGNED_AMDFW_A_POSITION) \
		$(OPT_SIGNED_AMDFW_A_FILE) \
		--location $(shell printf "%#x" $(PHOENIX_FW_A_POSITION)) \
		--anywhere \
		--output $@

$(obj)/amdfw_b.rom: $(obj)/amdfw.rom
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		$(OPT_APOB_NV_SIZE) \
		$(OPT_APOB_NV_BASE) \
		$(OPT_SPL_RW_AB_TABLE_FILE) \
		$(OPT_SIGNED_AMDFW_B_POSITION) \
		$(OPT_SIGNED_AMDFW_B_FILE) \
		--location $(shell printf "%#x" $(PHOENIX_FW_B_POSITION)) \
		--anywhere \
		--output $@


ifeq ($(CONFIG_VBOOT_SLOTS_RW_AB)$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),yy)
cbfs-files-y += apu/amdfw_a
apu/amdfw_a-file := $(obj)/amdfw_a.rom
apu/amdfw_a-position := $(AMD_FW_AB_POSITION)
apu/amdfw_a-type := raw

cbfs-files-y += apu/amdfw_b
apu/amdfw_b-file := $(obj)/amdfw_b.rom
apu/amdfw_b-position := $(AMD_FW_AB_POSITION)
apu/amdfw_b-type := raw

ifeq ($(CONFIG_SEPARATE_SIGNED_PSPFW),y)
build_complete:: $(obj)/amdfw_a.rom $(obj)/amdfw_b.rom
	@printf "    Adding Signed ROM and HASH\n"
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r SIGNED_AMDFW_A -i 0 -f $(obj)/amdfw_a.rom.signed
	$(CBFSTOOL) $(obj)/coreboot.rom write -u -r SIGNED_AMDFW_B -i 0 -f $(obj)/amdfw_b.rom.signed
	$(CBFSTOOL) $(obj)/coreboot.rom add -r FW_MAIN_A -f $(obj)/amdfw_a.rom.signed.hash \
		-n apu/amdfw_a_hash -t raw
	$(CBFSTOOL) $(obj)/coreboot.rom add -r FW_MAIN_B -f $(obj)/amdfw_b.rom.signed.hash \
		-n apu/amdfw_b_hash -t raw
endif # CONFIG_SEPARATE_SIGNED_PSPFW
endif

endif # ($(CONFIG_SOC_AMD_PHOENIX),y)
