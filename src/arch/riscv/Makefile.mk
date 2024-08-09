## SPDX-License-Identifier: GPL-2.0-only

################################################################################
## RISC-V specific options
################################################################################
ifeq ($(CONFIG_ARCH_RISCV),y)

ifeq ($(CONFIG_ARCH_RAMSTAGE_RISCV),y)
check-ramstage-overlap-regions += stack
endif

riscv_flags = -I$(src)/arch/riscv/

ifeq ($(CONFIG_ARCH_RISCV_RV64),y)
_rv_flags += -D__riscv -D__riscv_xlen=64 -D__riscv_flen=64
else
ifeq ($(CONFIG_ARCH_RISCV_RV32),y)
_rv_flags += -D__riscv -D__riscv_xlen=32 -D__riscv_flen=32
else
$(error "You need to select ARCH_RISCV_RV64 or ARCH_RISCV_RV32")
endif
endif

# Needed for -print-libgcc-file-name which gets confused about all those arch
# suffixes in ARCH_SUFFIX_riscv.
simple_riscv_flags = $(riscv_flags)

ifeq ($(CONFIG_COMPILER_GCC),y)
MARCH_SUFFIX=$(ARCH_SUFFIX_riscv)
else
MARCH_SUFFIX=
endif

ifeq ($(CCC_ANALYZER_OUTPUT_FORMAT),)
riscv_flags += -march=$(CONFIG_RISCV_ARCH)$(MARCH_SUFFIX) -mabi=$(CONFIG_RISCV_ABI) -mcmodel=$(CONFIG_RISCV_CODEMODEL)
simple_riscv_flags += -march=$(CONFIG_RISCV_ARCH) -mabi=$(CONFIG_RISCV_ABI) -mcmodel=$(CONFIG_RISCV_CODEMODEL)
else
riscv_flags += $(_rv_flags)
simple_riscv_flags += $(_rv_flags)
endif

riscv_asm_flags = -march=$(CONFIG_RISCV_ARCH)$(MARCH_SUFFIX) -mabi=$(CONFIG_RISCV_ABI)

COMPILER_RT_bootblock = $(shell $(GCC_bootblock) $(simple_riscv_flags) -print-libgcc-file-name)

COMPILER_RT_romstage  = $(shell  $(GCC_romstage) $(simple_riscv_flags) -print-libgcc-file-name)

COMPILER_RT_ramstage  = $(shell  $(GCC_ramstage) $(simple_riscv_flags) -print-libgcc-file-name)

## All stages

all-y += trap_util.S
all-y += trap_handler.c
all-y += fp_asm.S
all-y += sbi.c
all-y += mcall.c
all-y += virtual_memory.c
all-y += boot.c
all-y += smp.c
all-y += misc.c
all-$(CONFIG_ARCH_RISCV_PMP) += pmp.c
all-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c
all-$(CONFIG_RISCV_USE_ARCH_TIMER) += arch_timer.c

## FDT (Flattened Devicetree) inclusion

ifeq ($(CONFIG_RISCV_DTS),y)

# at some point dtc may be compiled by our toolchain
DTC ?= dtc
CPPFLAGS_dts += -nostdinc -P -x assembler-with-cpp -I src/arch/riscv/include

$(obj)/preprocessed.dts: $(call strip_quotes, $(CONFIG_RISCV_DTS_FILE))
	$(CPP_riscv) $(CPPFLAGS_dts) -o $@ $<

$(obj)/dtb: $(obj)/preprocessed.dts
	$(DTC) -I dts -O dtb -o $@ $<

# This may be optimized in the future by letting cbfstool parse our FDT into a unflattened
# devicetree blob in build time, so that we only need to flatten it in runtime instead of
# unflatten and flatten it in runtime.
cbfs-files-y += DTB
DTB-file := $(obj)/dtb
DTB-type := raw
DTB-align := 8 # according to spec device trees needs to be 8 byte aligned

endif # CONFIG_RISCV_DTS

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_RISCV),y)

bootblock-y = bootblock.S

$(eval $(call link_stage,bootblock))

bootblock-c-ccopts += $(riscv_flags)
bootblock-S-ccopts += $(riscv_asm_flags)

ifeq ($(CONFIG_ARCH_RISCV_RV32),y)
LDFLAGS_bootblock += -m elf32lriscv
endif #CONFIG_ARCH_RISCV_RV32

endif #CONFIG_ARCH_BOOTBLOCK_RISCV

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_RISCV),y)

romstage-$(CONFIG_SEPARATE_ROMSTAGE) += romstage.S

# Build the romstage

$(eval $(call link_stage,romstage))

romstage-c-ccopts += $(riscv_flags)
romstage-S-ccopts += $(riscv_asm_flags)

ifeq ($(CONFIG_ARCH_RISCV_RV32),y)
LDFLAGS_romstage += -m elf32lriscv
endif #CONFIG_ARCH_RISCV_RV32

endif #CONFIG_ARCH_ROMSTAGE_RISCV

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_RISCV),y)

ramstage-y =
ramstage-y += ramstage.S
ramstage-y += tables.c
ramstage-y += payload.c
ramstage-y += fit_payload.c

$(eval $(call create_class_compiler,rmodules,riscv))

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c

# Build the ramstage

$(eval $(call link_stage,ramstage))

ramstage-c-ccopts += $(riscv_flags)
ramstage-S-ccopts += $(riscv_asm_flags)

ifeq ($(CONFIG_ARCH_RISCV_RV32),y)
LDFLAGS_ramstage += -m elf32lriscv
endif #CONFIG_ARCH_RISCV_RV32

endif #CONFIG_ARCH_RAMSTAGE_RISCV

ifeq ($(CONFIG_RISCV_OPENSBI),y)

OPENSBI_SOURCE := $(top)/3rdparty/opensbi
OPENSBI_BUILD  := $(abspath $(obj)/3rdparty/opensbi)
OPENSBI_TARGET := $(OPENSBI_BUILD)/platform/$(CONFIG_OPENSBI_PLATFORM)/firmware/fw_dynamic.elf
OPENSBI := $(obj)/opensbi.elf

# TODO: Building with clang has troubles finding the proper linker.
# Always use GCC for now.
$(OPENSBI_TARGET): $(obj)/config.h | $(OPENSBI_SOURCE)
	printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	mkdir -p $(OPENSBI_BUILD)
	$(MAKE) \
		-C "$(OPENSBI_SOURCE)" \
		CC="$(GCC_ramstage) -fno-builtin" \
		LD="$(LD_ramstage)" \
		OBJCOPY="$(OBJCOPY_ramstage)" \
		AR="$(AR_ramstage)" \
		PLATFORM=$(CONFIG_OPENSBI_PLATFORM) \
		O="$(OPENSBI_BUILD)" \
		FW_JUMP=n \
		FW_DYNAMIC=y \
		FW_PAYLOAD=n \
		FW_TEXT_START=$(CONFIG_OPENSBI_TEXT_START)

# build upstream OpenSBI source tree
opensbi-source-y = $(OPENSBI_TARGET)
# get OpenSBI from specified binary
opensbi-source-$(CONFIG_OPENSBI_BLOB) = $(call strip_quotes,$(CONFIG_OPENSBI_BLOB_PATH))

$(OPENSBI): $(opensbi-source-y)
	cp $< $@

OPENSBI_CBFS := $(CONFIG_CBFS_PREFIX)/opensbi
$(OPENSBI_CBFS)-file := $(OPENSBI)
$(OPENSBI_CBFS)-type := payload
$(OPENSBI_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(OPENSBI_CBFS)

check-ramstage-overlap-files += $(OPENSBI_CBFS)

ramstage-y += opensbi.c

endif #CONFIG_RISCV_OPENSBI

endif #CONFIG_ARCH_RISCV
