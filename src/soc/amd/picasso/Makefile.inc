# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_PICASSO),y)

subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += psp_verstage
subdirs-$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK) += ../common/psp_verstage

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y += config.c
all-y += aoac.c

bootblock-y += early_fch.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-y += uart.c

romstage-y += fsp_m_params.c
romstage-y += gpio.c
romstage-y += i2c.c
romstage-y += romstage.c
romstage-y += uart.c

verstage-y += i2c.c
verstage_x86-y += gpio.c
verstage_x86-y += uart.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += agesa_acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += fch.c
ramstage-y += fsp_s_params.c
ramstage-y += gpio.c
ramstage-y += graphics.c
ramstage-y += i2c.c
ramstage-y += mca.c
ramstage-y += pcie_gpp.c
ramstage-y += root_complex.c
ramstage-y += sata.c
ramstage-y += soc_util.c
ramstage-y += uart.c
ramstage-y += xhci.c

smm-y += smihandler.c
ifeq ($(CONFIG_DEBUG_SMI),y)
smm-y += uart.c
endif
smm-y += gpio.c

CPPFLAGS_common += -I$(src)/soc/amd/picasso/include
CPPFLAGS_common += -I$(src)/soc/amd/picasso/acpi
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/picasso
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/picasso/include

# ROMSIG Normally At ROMBASE + 0x20000
# Overridden by CONFIG_AMD_FWM_POSITION_INDEX
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |               |                |            |
# +-----------+---------------+----------------+------------+
# |           |  PSPDIR ADDR  |  BIOSDIR ADDR  |
# +-----------+---------------+----------------+

$(if $(CONFIG_AMD_FWM_POSITION_INDEX), ,\
     $(error Invalid AMD firmware position index. Check if the board sets a valid ROM size))

PICASSO_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)

# 0x40 accounts for the cbfs_file struct + filename + metadata structs, aligned to 64 bytes
# Building the cbfs image will fail if the offset isn't large enough
AMD_FW_AB_POSITION := 0x40

PICASSO_FW_A_POSITION=$(call int-add, \
	$(shell awk '$$2 == "FMAP_SECTION_FW_MAIN_A_START" {print $$3}' $(obj)/fmap_config.h) \
	$(AMD_FW_AB_POSITION))

PICASSO_FW_B_POSITION=$(call int-add, \
	$(shell awk '$$2 == "FMAP_SECTION_FW_MAIN_B_START" {print $$3}' $(obj)/fmap_config.h) \
	$(AMD_FW_AB_POSITION))

#
# PSP Directory Table items
#
# Certain ordering requirements apply, however these are ensured by amdfwtool.
# For more information see "AMD Platform Security Processor BIOS Architecture
# Design Guide for AMD Family 17h Processors" (PID #55758, NDA only).
#

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

ifeq ($(CONFIG_PSP_LOAD_S0I3_FW),y)
OPT_PSP_LOAD_S0I3_FW="--load-s0i3"
endif

# type = 0x3a
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSP_WHITELIST_FILE=$(CONFIG_PSP_WHITELIST_FILE)
endif
#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x4
# The flashmap section used for this is expected to be named PSP_NVRAM
PSP_NVRAM_BASE=$(shell awk '$$2 == "FMAP_SECTION_PSP_NVRAM_START" {print $$3}' $(obj)/fmap_config.h)
PSP_NVRAM_SIZE=$(shell awk '$$2 == "FMAP_SECTION_PSP_NVRAM_SIZE" {print $$3}' $(obj)/fmap_config.h)

# type = 0x7
# RSA 2048 signature
#ifeq ($(CONFIG_PSP_PLATFORM_SECURE_BOOT),y)
PSP_BIOS_SIG_SIZE=0x100
#endif

# type = 0x60
PSP_APCB_FILES=$(APCB_SOURCES)

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

OPT_PSP_NVRAM_BASE=$(call add_opt_prefix, $(PSP_NVRAM_BASE), --nvram-base)
OPT_PSP_NVRAM_SIZE=$(call add_opt_prefix, $(PSP_NVRAM_SIZE), --nvram-size)

OPT_VERSTAGE_FILE=$(call add_opt_prefix, $(PSP_VERSTAGE_FILE), --verstage)
OPT_VERSTAGE_SIG_FILE=$(call add_opt_prefix, $(PSP_VERSTAGE_SIG_FILE), --verstage_sig)

OPT_PSP_APCB_FILES=$(foreach i, $(shell seq $(words $(PSP_APCB_FILES))), \
	$(call add_opt_prefix, $(word $(i), $(PSP_APCB_FILES)), \
	--instance $(shell printf "%x" $$(($(i)-1))) --apcb  )   )
OPT_PSP_APCB_FILES_BK=$(foreach i, $(shell seq $(words $(PSP_APCB_FILES))), \
	$(call add_opt_prefix, $(word $(i), $(PSP_APCB_FILES)), \
	--instance $(shell printf "1%x" $$(($(i)-1))) --apcb  )   )

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)

OPT_PSP_BIOS_SIG_SIZE=$(call add_opt_prefix, $(PSP_BIOS_SIG_SIZE), --bios-sig-size)
OPT_PSP_SHAREDMEM_BASE=$(call add_opt_prefix, $(PSP_SHAREDMEM_BASE), --sharedmem)
OPT_PSP_SHAREDMEM_SIZE=$(call add_opt_prefix, $(PSP_SHAREDMEM_SIZE), --sharedmem-size)
OPT_APOB_NV_SIZE=$(call add_opt_prefix, $(APOB_NV_SIZE), --apob-nv-size)
OPT_APOB_NV_BASE=$(call add_opt_prefix, $(APOB_NV_BASE),--apob-nv-base)
OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)
OPT_EFS_SPI_MICRON_FLAG=$(call add_opt_prefix, $(CONFIG_EFS_SPI_MICRON_FLAG), --spi-micron-flag)

OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)

ifeq ($(CONFIG_VBOOT),)
OPT_APOB0_NV_SIZE=$(OPT_APOB_NV_SIZE)
OPT_APOB0_NV_BASE=$(OPT_APOB_NV_BASE)
endif

OPT_WHITELIST_FILE=$(call add_opt_prefix, $(PSP_WHITELIST_FILE), --whitelist)

AMDFW_COMMON_ARGS=$(OPT_PSP_APCB_FILES) \
		$(OPT_PSP_NVRAM_BASE) \
		$(OPT_PSP_NVRAM_SIZE) \
		$(OPT_PSP_APCB_FILES_BK) \
		$(OPT_APOB_ADDR) \
		$(OPT_PSP_BIOSBIN_FILE) \
		$(OPT_PSP_BIOSBIN_DEST) \
		$(OPT_PSP_BIOSBIN_SIZE) \
		$(OPT_PSP_BIOS_SIG_SIZE) \
		$(OPT_PSP_SOFTFUSE) \
		--use-pspsecureos \
		$(OPT_PSP_LOAD_MP2_FW) \
		$(OPT_PSP_LOAD_S0I3_FW) \
		$(OPT_WHITELIST_FILE) \
		$(OPT_PSP_SHAREDMEM_BASE) \
		$(OPT_PSP_SHAREDMEM_SIZE) \
		--combo-capable \
		$(OPT_TOKEN_UNLOCK) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_EFS_SPI_MICRON_FLAG) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--soc-name "Picasso" \
		--flashsize $(CONFIG_ROM_SIZE)

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
		$(OPT_APOB0_NV_SIZE) \
		$(OPT_APOB0_NV_BASE) \
		$(OPT_VERSTAGE_FILE) \
		$(OPT_VERSTAGE_SIG_FILE) \
		--location $(shell printf "%#x" $(PICASSO_FWM_POSITION)) \
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
		--location $(shell printf "%#x" $(PICASSO_FW_A_POSITION)) \
		--anywhere \
		--output $@

$(obj)/amdfw_b.rom: $(obj)/amdfw.rom
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		$(OPT_APOB_NV_SIZE) \
		$(OPT_APOB_NV_BASE) \
		--location $(shell printf "%#x" $(PICASSO_FW_B_POSITION)) \
		--anywhere \
		--output $@

ifeq ($(CONFIG_VBOOT_SLOTS_RW_A)$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),yy)
cbfs-files-y += apu/amdfw_a
apu/amdfw_a-file := $(obj)/amdfw_a.rom
apu/amdfw_a-position := $(AMD_FW_AB_POSITION)
apu/amdfw_a-type := raw
endif

ifeq ($(CONFIG_VBOOT_SLOTS_RW_AB)$(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),yy)
cbfs-files-y += apu/amdfw_b
apu/amdfw_b-file := $(obj)/amdfw_b.rom
apu/amdfw_b-position := $(AMD_FW_AB_POSITION)
apu/amdfw_b-type := raw
endif

endif # ($(CONFIG_SOC_AMD_PICASSO),y)
