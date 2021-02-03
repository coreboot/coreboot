# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_CEZANNE),y)

subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y		+= config.c
all-y		+= aoac.c

bootblock-y	+= bootblock.c
bootblock-y	+= early_fch.c
bootblock-y	+= gpio.c
bootblock-y	+= reset.c
bootblock-y	+= uart.c

verstage_x86-y	+= gpio.c
verstage_x86-y	+= reset.c
verstage_x86-y	+= uart.c

romstage-y	+= gpio.c
romstage-y	+= reset.c
romstage-y	+= romstage.c
romstage-y	+= uart.c

ramstage-y	+= acpi.c
ramstage-y	+= chip.c
ramstage-y	+= cpu.c
ramstage-y	+= fch.c
ramstage-y	+= fsp_params.c
ramstage-y	+= gpio.c
ramstage-y	+= pcie_gpp.c
ramstage-y	+= reset.c
ramstage-y	+= root_complex.c
ramstage-y	+= uart.c

smm-y		+= smihandler.c

CPPFLAGS_common += -I$(src)/soc/amd/cezanne/include
CPPFLAGS_common += -I$(src)/soc/amd/cezanne/acpi
CPPFLAGS_common += -I$(src)/vendorcode/amd/fsp/cezanne

$(call strip_quotes,$(CONFIG_FSP_M_CBFS))-options := -b $(CONFIG_FSP_M_ADDR)

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

CEZANNE_FWM_POSITION=$(call int-add, \
	$(call int-subtract, 0xffffffff \
	$(call int-shift-left, \
	0x80000 $(CONFIG_AMD_FWM_POSITION_INDEX))) 0x20000 1)
#
# PSP Directory Table items
#
# Certain ordering requirements apply, however these are ensured by amdfwtool.
# For more information see "AMD Platform Security Processor BIOS Architecture
# Design Guide for AMD Family 17h Processors" (PID #55758, NDA only).
#

FIRMWARE_LOCATION=$(shell grep -e FIRMWARE_LOCATION $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}')

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

#
# BIOS Directory Table items - proper ordering is managed by amdfwtool
#

# type = 0x60
PSP_APCB_FILES=$(APCB_SOURCES) $(APCB_SOURCES_68) $(APCB_SOURCES_RECOVERY)

# type = 0x61
PSP_APOB_BASE=$(CONFIG_PSP_APOB_DRAM_ADDRESS)

# type = 0x62
PSP_BIOSBIN_FILE=$(obj)/amd_biospsp.img
PSP_ELF_FILE=$(objcbfs)/bootblock.elf
PSP_BIOSBIN_SIZE=$(shell $(READELF_bootblock) -l $(PSP_ELF_FILE) | grep LOAD | awk '{print $$5}')
PSP_BIOSBIN_DEST=$(shell $(READELF_bootblock) -l $(PSP_ELF_FILE) | grep LOAD | awk '{print $$3}')

# type = 0xb - See #55758 (NDA) for bit definitions.
PSP_SOFTFUSE_BITS += 28

#hardcode post code to eSPI
PSP_SOFTFUSE_BITS += 15 6

# Helper function to return a value with given bit set
set-bit=$(call int-shift-left, 1 $(call _toint,$1))
PSP_SOFTFUSE=$(shell A=$(call int-add, \
		$(foreach bit,$(PSP_SOFTFUSE_BITS),$(call set-bit,$(bit)))); printf "0x%x" $$A)

#
# Build the arguments to amdfwtool (order is unimportant).  Missing file names
# result in empty OPT_ variables, i.e. the argument is not passed to amdfwtool.
#

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_PSP_APCB_FILES= $(if $(APCB_SOURCES), --instance 0 --apcb $(APCB_SOURCES)) \
                    $(if $(APCB_SOURCES_RECOVERY), --instance 10 --apcb $(APCB_SOURCES_RECOVERY)) \
                    $(if $(APCB_SOURCES_68), --instance 18 --apcb $(APCB_SOURCES_68))

OPT_APOB_ADDR=$(call add_opt_prefix, $(PSP_APOB_BASE), --apob-base)
OPT_PSP_BIOSBIN_FILE=$(call add_opt_prefix, $(PSP_BIOSBIN_FILE), --bios-bin)
OPT_PSP_BIOSBIN_DEST=$(call add_opt_prefix, $(PSP_BIOSBIN_DEST), --bios-bin-dest)
OPT_PSP_BIOSBIN_SIZE=$(call add_opt_prefix, $(PSP_BIOSBIN_SIZE), --bios-uncomp-size)

OPT_EFS_SPI_READ_MODE=$(call add_opt_prefix, $(CONFIG_EFS_SPI_READ_MODE), --spi-read-mode)
OPT_EFS_SPI_SPEED=$(call add_opt_prefix, $(CONFIG_EFS_SPI_SPEED), --spi-speed)
OPT_EFS_SPI_MICRON_FLAG=$(call add_opt_prefix, $(CONFIG_EFS_SPI_MICRON_FLAG), --spi-micron-flag)

OPT_PSP_SOFTFUSE=$(call add_opt_prefix, $(PSP_SOFTFUSE), --soft-fuse)

# Add all the files listed in the config file
POUND_SIGN=$(call strip_quotes, "\#")
DEP_FILES= $(patsubst %,$(FIRMWARE_LOCATION)/%, $(shell sed -e /^$(POUND_SIGN)/d -e /*/d -e /^FIRMWARE_LOCATION/d $(CONFIG_AMDFW_CONFIG_FILE) | awk '{print $$2}' ))

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
		$(OPT_EFS_SPI_READ_MODE) \
		$(OPT_EFS_SPI_SPEED) \
		$(OPT_EFS_SPI_MICRON_FLAG) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--soc-name "Cezanne" \
		--flashsize $(CONFIG_ROM_SIZE)

$(obj)/amdfw.rom:	$(call strip_quotes, $(PSP_BIOSBIN_FILE)) \
			$$(PSP_APCB_FILES) \
			$(DEP_FILES) \
			$(AMDFWTOOL) \
			$(obj)/fmap_config.h
	$(if $(PSP_APCB_FILES), ,$(error APCB_SOURCES is not set))
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(AMDFW_COMMON_ARGS) \
		--location $(shell printf "%#x" $(CEZANNE_FWM_POSITION)) \
		--multilevel \
		--output $@

$(PSP_BIOSBIN_FILE): $(PSP_ELF_FILE) $(AMDCOMPRESS)
	rm -f $@
	@printf "    AMDCOMPRS  $(subst $(obj)/,,$(@))\n"
	$(AMDCOMPRESS) --infile $(PSP_ELF_FILE) --outfile $@ --compress \
		--maxsize $(PSP_BIOSBIN_SIZE)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(CEZANNE_FWM_POSITION)
apu/amdfw-type := raw

cpu_microcode_bins += $(wildcard ${FIRMWARE_LOCATION}/UcodePatch_*.bin)

endif # ($(CONFIG_SOC_AMD_CEZANNE),y)
