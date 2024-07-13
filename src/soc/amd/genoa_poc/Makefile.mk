## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_GENOA_POC),y)

all-y		+= mmap_boot.c
all-y		+= reset.c
all-y		+= config.c
all-y		+= gpio.c
all-y		+= i2c.c
all-y		+= uart.c

bootblock-y	+= early_fch.c
bootblock-y	+= aoac.c

ramstage-y	+= acpi.c
ramstage-y	+= aoac.c
ramstage-y	+= chip.c
ramstage-y	+= cpu.c
ramstage-y	+= domain.c
ramstage-y	+= fch.c
ramstage-y	+= root_complex.c
ramstage-y	+= smihandler.c
ramstage-y	+= mca.c

smm-y		+= root_complex.c
smm-y		+= smihandler.c
smm-$(CONFIG_DEBUG_SMI) += uart.c

CPPFLAGS_common += -I$(src)/soc/amd/genoa_poc/acpi
CPPFLAGS_common += -I$(src)/soc/amd/genoa_poc/include

ifeq ($(call int-gt, $(CONFIG_ROM_SIZE) 0x1000000), 1)
CBFSTOOL_ADD_CMD_OPTIONS+= --mmap 0:0xff000000:0x1000000
endif

#
# PSP Directory Table items
#
# Certain ordering requirements apply, however these are ensured by amdfwtool.
# For more information see "AMD Platform Security Processor BIOS Implementation
# Guide for Server EPYC Processors" #57299
#

FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')

ifeq ($(CONFIG_PSP_DISABLE_POSTCODES),y)
PSP_SOFTFUSE_BITS += 7
endif

ifeq ($(CONFIG_PSP_UNLOCK_SECURE_DEBUG),y)
# Enable secure debug unlock
PSP_SOFTFUSE_BITS += 0
OPT_TOKEN_UNLOCK="--token-unlock"
endif

# Use additional Soft Fuse bits specified in Kconfig
PSP_SOFTFUSE_BITS += $(call strip_quotes, $(CONFIG_PSP_SOFTFUSE_BITS))

# type = 0x3a
ifeq ($(CONFIG_HAVE_PSP_WHITELIST_FILE),y)
PSP_WHITELIST_FILE=$(CONFIG_PSP_WHITELIST_FILE)
endif

# type = 0x55
SPL_TABLE_FILE=$(CONFIG_SPL_TABLE_FILE)

#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x60
PSP_APCB_FILES=$(APCB_SOURCES) $(APCB1_SOURCES) $(APCB_SOURCES_RECOVERY) $(APCB_SOURCES_RECOVERY1) $(APCB_SOURCES_RECOVERY2)

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DRAM_ADDRESS)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_ELF_FILE=$(objcbfs)/bootblock.elf
PSP_BIOSBIN_SIZE=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$5}')
PSP_BIOSBIN_DEST=$(shell $(READELF_bootblock) -Wl $(PSP_ELF_FILE) | grep LOAD | awk '{print $$3}')

# Helper function to return a value with given bit set
# Soft Fuse type = 0xb - See #57299 (NDA) for bit definitions.
set-bit=$(call int-shift-left, 1 $(call _toint,$1))
PSP_SOFTFUSE=$(shell A=$(call int-add, \
		$(foreach bit,$(sort $(PSP_SOFTFUSE_BITS)),$(call set-bit,$(bit)))); printf "0x%x" $$A)

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_PSP_APCB_FILES= $(if $(APCB_SOURCES), --instance 0 --apcb $(APCB_SOURCES)) \
                    $(if $(APCB_SOURCES1), --instance 1 --apcb $(APCB_SOURCES1)) \
                    $(if $(APCB_SOURCES_RECOVERY), --instance 10 --apcb $(APCB_SOURCES_RECOVERY)) \
                    $(if $(APCB_SOURCES_RECOVERY1), --instance 18 --apcb $(APCB_SOURCES_RECOVERY1)) \
                    $(if $(APCB_SOURCES_RECOVERY2), --instance 19 --apcb $(APCB_SOURCES_RECOVERY2)) \
                    $(if $(APCB_SOURCES_68), --instance 18 --apcb $(APCB_SOURCES_68))

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)

OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)
OPT_EFS_SPI_MICRON_FLAG=$(call add_opt_prefix, $(CONFIG_EFS_SPI_MICRON_FLAG), --spi-micron-flag)
OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)

OPT_WHITELIST_FILE=$(call add_opt_prefix, $(PSP_WHITELIST_FILE), --whitelist)
OPT_SPL_TABLE_FILE=$(call add_opt_prefix, $(SPL_TABLE_FILE), --spl-table)

AMDFW_COMMON_ARGS=$(OPT_PSP_APCB_FILES) \
		$(OPT_APOB_ADDR) \
		$(OPT_DEBUG_AMDFWTOOL) \
		$(OPT_PSP_BIOSBIN_FILE) \
		$(OPT_PSP_BIOSBIN_DEST) \
		$(OPT_PSP_BIOSBIN_SIZE) \
		$(OPT_PSP_SOFTFUSE) \
		--use-pspsecureos \
		$(OPT_TOKEN_UNLOCK) \
		$(OPT_WHITELIST_FILE) \
		$(OPT_SPL_TABLE_FILE) \
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_EFS_SPI_MICRON_FLAG) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--flashsize 0x1000000

$(obj)/amdfw.rom:	$(call strip_quotes, $(PSP_BIOSBIN_FILE)) \
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
		--location $(CONFIG_AMD_FWM_POSITION) \
		--multilevel \
		--output $@

$(PSP_BIOSBIN_FILE): $(PSP_ELF_FILE) $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(PSP_ELF_FILE) --outfile $@ --compress \
		--maxsize $(PSP_BIOSBIN_SIZE)

endif
