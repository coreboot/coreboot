# SPDX-License-Identifier: GPL-2.0-only

ifneq ($(NOCOMPILE),1)
GIT:=$(shell git -C "$(top)" rev-parse --git-dir 1>/dev/null 2>&1 \
	&& command -v git)
else
GIT:=
endif

#######################################################################
# normalize Kconfig variables in a central place
CONFIG_CBFS_PREFIX:=$(call strip_quotes,$(CONFIG_CBFS_PREFIX))
CONFIG_FMDFILE:=$(call strip_quotes,$(CONFIG_FMDFILE))
CONFIG_DEVICETREE:=$(call strip_quotes, $(CONFIG_DEVICETREE))
CONFIG_OVERRIDE_DEVICETREE:=$(call strip_quotes, $(CONFIG_OVERRIDE_DEVICETREE))
CONFIG_CHIPSET_DEVICETREE:=$(call strip_quotes, $(CONFIG_CHIPSET_DEVICETREE))
CONFIG_MEMLAYOUT_LD_FILE:=$(call strip_quotes, $(CONFIG_MEMLAYOUT_LD_FILE))

#######################################################################
# misleadingly named, this is the coreboot version
ifeq ($(KERNELVERSION),)
ifeq ($(BUILD_TIMELESS),1)
KERNELVERSION := -TIMELESS--LESSTIME-
else
KERNELVERSION := $(strip $(if $(GIT),\
	$(shell git describe --dirty --always || git describe),\
	$(if $(wildcard $(top)/.coreboot-version),\
		$(shell cat $(top)/.coreboot-version),\
		coreboot-unknown$(KERNELREVISION))))
endif
endif
COREBOOT_EXPORTS += KERNELVERSION

#######################################################################
# Basic component discovery
MAINBOARDDIR=$(call strip_quotes,$(CONFIG_MAINBOARD_DIR))
VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))
CARRIER_DIR:=$(call strip_quotes,$(CONFIG_CARRIER_DIR))
COREBOOT_EXPORTS += MAINBOARDDIR VARIANT_DIR CARRIER_DIR

## Final build results, which CBFSTOOL uses to create the final
## rom image file, are placed under $(objcbfs).
## These typically have suffixes .debug .elf .bin and .map
objcbfs := $(obj)/cbfs/$(CONFIG_CBFS_PREFIX)
additional-dirs += $(objcbfs)
COREBOOT_EXPORTS += objcbfs

## Based on the active configuration, Makefile conditionally collects
## the required assembly includes and saves them in a file.
## Such files that do not have a clear one-to-one relation to a source
## file under src/ are placed and built under $(objgenerated)
objgenerated := $(obj)/generated
additional-dirs += $(objgenerated)
COREBOOT_EXPORTS += objgenerated

## CCACHE_EXTRAFILES can be set by individual rules to help CCACHE
## discover dependencies it might not notice on its own (e.g. asm (".incbin")).
COREBOOT_EXPORTS += CCACHE_EXTRAFILES

#######################################################################
# root rule to resolve if in build mode (ie. configuration exists)
real-target: $(obj)/config.h coreboot files_added
coreboot: $(obj)/coreboot.rom $(obj)/cbfstool $(obj)/rmodtool $(obj)/ifwitool $(obj)/cse_fpt $(obj)/cse_serger

# This target can be used in site local to run scripts or additional
# targets after the build completes by creating a Makefile.inc in the
# site-local directory with a target named 'build_complete::'
build_complete:: coreboot
	printf "\nBuilt %s (%s)\n" $(MAINBOARDDIR) \
		$(CONFIG_MAINBOARD_PART_NUMBER)

# This target can be used to run rules after all files were added to CBFS,
# for example to process FMAP regions or the entire image.
files_added:: build_complete

#######################################################################
# our phony targets
PHONY+= clean-abuild coreboot check-style build_complete

#######################################################################
# root source directories of coreboot
subdirs-y := src/lib src/commonlib/ src/console src/device src/acpi src/superio/common
subdirs-$(CONFIG_EC_ACPI) += src/ec/intel
subdirs-y += src/ec/acpi $(wildcard src/ec/*/*) $(wildcard src/southbridge/*/*)
subdirs-y += $(wildcard src/soc/*) $(wildcard src/soc/*/common) $(filter-out $(wildcard src/soc/*/common),$(wildcard src/soc/*/*))
subdirs-y += $(wildcard src/northbridge/*/*)
subdirs-y += $(filter-out src/superio/common,$(wildcard src/superio/*)) $(wildcard src/superio/*/*)
subdirs-y += $(wildcard src/drivers/*) $(wildcard src/drivers/*/*) $(wildcard src/drivers/*/*/*)
subdirs-y += src/cpu src/vendorcode
subdirs-y += util/cbfstool util/sconfig util/nvramtool util/pgtblgen util/amdfwtool
subdirs-y += util/futility util/marvell util/bincfg util/supermicro util/qemu
subdirs-y += util/ifdtool
subdirs-y += $(wildcard src/arch/*)
subdirs-y += src/mainboard/$(MAINBOARDDIR)
subdirs-y += src/security
subdirs-y += payloads payloads/external
subdirs-$(CONFIG_SBOM) += src/sbom

subdirs-y += site-local
subdirs-y += util/checklist util/testing

#######################################################################
# Add source classes and their build options
classes-y := ramstage romstage bootblock decompressor postcar smm smmstub cpu_microcode verstage

# Add a special 'all' class to add sources to all stages
$(call add-special-class,all)
all-handler = $(foreach class,bootblock verstage romstage postcar ramstage,$(eval $(class)-y += $(2)))

$(call add-special-class,verstage_x86)
ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)
verstage_x86-handler = $(eval verstage-y += $(2))
else
verstage_x86-handler =
endif

# Add dynamic classes for rmodules
$(foreach supported_arch,$(ARCH_SUPPORTED), \
	    $(eval $(call define_class,rmodules_$(supported_arch),$(supported_arch))))
# Provide a macro to determine environment for free standing rmodules.
$(foreach supported_arch,$(ARCH_SUPPORTED), \
	$(eval rmodules_$(supported_arch)-generic-ccopts += -D__RMODULE__))

#######################################################################
# Helper functions for math, strings, and various file placement matters.
# macros work on all formats understood by printf(1)
# values are space separated if using more than one value
#
# int-add:        adds an arbitrary length list of integers
# int-subtract:   subtracts the second of two integers from the first
# int-multiply:   multiplies an arbitrary length list of integers
# int-divide:     divides the first integer by the second
# int-remainder:  arithmetic remainder of the first number divided by the second
# int-shift-left: Shift $1 left by $2 bits
# int-lt:         1 if the first value is less than the second.  0 otherwise
# int-gt:         1 if the first values is greater than the second.  0 otherwise
# int-eq:         1 if the two values are equal.  0 otherwise
# int-align:      align $1 to $2 units
# file-size:      returns the filesize of the given file
# tolower:        returns the value in all lowercase
# toupper:        returns the value in all uppercase
# ws_to_under:    returns the value with any whitespace changed to underscores
_toint=$(shell printf "%d" $1)
_int-add2=$(shell expr $(call _toint,$1) + $(call _toint,$2))
int-add=$(if $(filter 1,$(words $1)),$(strip $1),$(call int-add,$(call _int-add2,$(word 1,$1),$(word 2,$1)) $(wordlist 3,$(words $1),$1)))
int-subtract=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) - $(call _toint,$(word 2,$1))))
_int-multiply2=$(shell expr $(call _toint,$1) \* $(call _toint,$2))
int-multiply=$(if $(filter 1,$(words $1)),$(strip $1),$(call int-multiply,$(call _int-multiply2,$(word 1,$1),$(word 2,$1)) $(wordlist 3,$(words $1),$1)))
int-divide=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) / $(call _toint,$(word 2,$1))))
int-remainder=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) % $(call _toint,$(word 2,$1))))
int-shift-left=$(shell echo "$(call _toint,$(word 1, $1)) * (2 ^ $(call _toint,$(word 2, $1)))" | bc)
int-lt=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) \< $(call _toint,$(word 2,$1))))
int-gt=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) \> $(call _toint,$(word 2,$1))))
int-eq=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) = $(call _toint,$(word 2,$1))))
int-align=$(shell A=$(call _toint,$1) B=$(call _toint,$2); expr $$A + \( \( $$B - \( $$A % $$B \) \) % $$B \) )
int-align-down=$(shell A=$(call _toint,$1) B=$(call _toint,$2); expr $$A - \( $$A % $$B \) )
file-size=$(strip $(shell wc -c "$1" | cut -f 1 -d ' '))
tolower=$(shell echo '$1' | tr '[:upper:]' '[:lower:]')
toupper=$(shell echo '$1' | tr '[:lower:]' '[:upper:]')
ws_to_under=$(shell echo '$1' | tr ' \t' '_')

#######################################################################
# Helper functions for ramstage postprocess
spc :=
spc := $(spc) $(spc)
comma := ,

# Returns all files and dirs below `dir` (recursively).
# files-below-dir,dir,files
files-below-dir=$(filter $(1)%,$(2))

# Returns all dirs below `dir` (recursively).
# dirs-below-dir,dir,files
dirs-below-dir=$(filter-out $(1),$(sort $(dir $(call files-below-dir,$(1),$(2)))))

# Returns all files directly in `dir` (non-recursively).
# files-in-dir,dir,files
files-in-dir=$(filter-out $(addsuffix %,$(call dirs-below-dir,$(1),$(2))),$(call files-below-dir,$(1),$(2)))

#######################################################################
# reduce command line length by linking the objects of each
# directory into an intermediate file
ramstage-postprocess=$$(eval DEPENDENCIES+=$$(addsuffix .d,$$(basename $(1)))) \
	$(foreach d,$(sort $(dir $(filter-out %.ld,$(1)))), \
	$(eval $(d)ramstage.a: $(call files-in-dir,$(d),$(filter-out %.ld,$(1))); rm -f $$@ && $(AR_ramstage) rcsT $$@ $$^ ) \
	$(eval ramstage-objs:=$(d)ramstage.a $(filter-out $(filter-out %.ld, $(call files-in-dir,$(d),$(1))),$(ramstage-objs))))

decompressor-generic-ccopts += -D__DECOMPRESSOR__
bootblock-generic-ccopts += -D__BOOTBLOCK__
romstage-generic-ccopts += -D__ROMSTAGE__
ramstage-generic-ccopts += -D__RAMSTAGE__
ifeq ($(CONFIG_COVERAGE),y)
ramstage-c-ccopts += -fprofile-arcs -ftest-coverage
endif
ifneq ($(GIT),)
ifneq ($(UPDATED_SUBMODULES),1)
$(info Updating git submodules.)
# try to fetch non-optional submodules if the source is under git
forgetthis:=$(shell git submodule update --init $(quiet_errors))
# Checkout Cmocka repository
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/cmocka $(quiet_errors))
ifeq ($(CONFIG_USE_BLOBS),y)
# These items are necessary because each has update=none in .gitmodules.  They are ignored
# until expressly requested and enabled with --checkout
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/blobs $(quiet_errors))
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/intel-microcode $(quiet_errors))
ifeq ($(CONFIG_FSP_USE_REPO),y)
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/fsp $(quiet_errors))
endif
ifeq ($(CONFIG_USE_AMD_BLOBS),y)
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/amd_blobs $(quiet_errors))
endif
ifeq ($(CONFIG_USE_QC_BLOBS),y)
forgetthis:=$(shell git submodule update --init --checkout 3rdparty/qc_blobs $(quiet_errors))
endif
endif
UPDATED_SUBMODULES:=1
COREBOOT_EXPORTS += UPDATED_SUBMODULES

endif
endif # GIT != ""

postcar-c-deps:=$$(OPTION_TABLE_H)
ramstage-c-deps:=$$(OPTION_TABLE_H)
romstage-c-deps:=$$(OPTION_TABLE_H)
verstage-c-deps:=$$(OPTION_TABLE_H)
bootblock-c-deps:=$$(OPTION_TABLE_H)
$(foreach type,ads adb, \
  $(foreach stage,$(COREBOOT_STANDARD_STAGES), \
    $(eval $(stage)-$(type)-deps := \
	$(obj)/$(stage)/$(notdir $(KCONFIG_AUTOADS)) \
	$(obj)/libgnat-$(ARCH-$(stage)-y)/libgnat.a)))

# Add handler to copy linker scripts
define generic-objs_ld_template_gen
de$(EMPTY)fine $(1)-objs_ld_template
$$(call src-to-obj,$1,$$(1).ld): $$(1).ld $(obj)/config.h $(obj)/fmap_config.h
	@printf "    CP         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$$(CC_$(1)) -MMD $$(CPPFLAGS_$(1)) $$($(1)-ld-ccopts) $(PREPROCESS_ONLY) -include $(obj)/config.h -MT $$$$@ -o $$$$@ $$$$<
en$(EMPTY)def
endef

# Add handler to deal with archives
define generic-objs_a_template_gen
de$(EMPTY)fine $(1)-objs_a_template
$$(call src-to-obj,$1,$$(1).a): $$(1).a
	@printf "    AR         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$$$$(AR_$(1)) rcsT $$$$@.tmp $$$$<
	mv $$$$@.tmp $$$$@
en$(EMPTY)def
endef

# Add handler to add no rules for manual files
define generic-objs_manual_template_gen
# do nothing
endef

#######################################################################
# Add handler to compile ACPI's ASL
# arg1: base file name
# arg2: y or n for including in cbfs. defaults to y

# Empty resource templates were marked as a warning in IASL with the comment
# "This would appear to be worthless in real-world ASL code.", which is
# possibly true in many cases.  In other cases it seems that an empty
# ResourceTemplate is the correct code.
# As it's valid ASL, disable the warning.
EMPTY_RESOURCE_TEMPLATE_WARNING = 3150

IASL_WARNINGS_LIST = $(EMPTY_RESOURCE_TEMPLATE_WARNING)

IGNORED_IASL_WARNINGS = $(addprefix -vw , $(IASL_WARNINGS_LIST))

define asl_template
$(CONFIG_CBFS_PREFIX)/$(1).aml-file = $(obj)/$(1).aml
$(CONFIG_CBFS_PREFIX)/$(1).aml-type = raw
$(CONFIG_CBFS_PREFIX)/$(1).aml-compression = none
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
$(CONFIG_CBFS_PREFIX)/$(1).aml-align = 64
endif
cbfs-files-$(if $(2),$(2),y) += $(CONFIG_CBFS_PREFIX)/$(1).aml
-include $(obj)/$(1).d
$(obj)/$(1).aml: $(src)/mainboard/$(MAINBOARDDIR)/$(1).asl $(obj)/config.h
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CC_ramstage) -x assembler-with-cpp -E -MMD -MT $$(@) $$(CPPFLAGS_ramstage) -D__ACPI__ -P -include $(src)/include/kconfig.h -I$(obj) -I$(src) -I$(src)/include -I$(src)/arch/$(ARCHDIR-$(ARCH-ramstage-y))/include -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $(obj)/$(1).asl
	cd $$(dir $$@); $(IASL) $(IGNORED_IASL_WARNINGS) -we -p $$(notdir $$@) $(1).asl
	echo "    IASL       "$(IASL_WARNINGS_LIST)" warning types were ignored!"
	if ! $(IASL) -d $$@ 2>&1 | grep -Eq 'ACPI (Warning|Error)'; then	\
		echo "    IASL       $$@ disassembled correctly.";		\
		true;								\
	else									\
		echo "Error: Could not correctly disassemble $$@";		\
		$(IASL) -d $$@;							\
		false;								\
	fi
endef

#######################################################################
# Parse plaintext CMOS defaults into binary format
# arg1: source file
# arg2: binary file name
cbfs-files-processor-nvramtool= \
	$(eval $(2): $(1) $(top)/$(call strip_quotes,$(CONFIG_CMOS_LAYOUT_FILE)) | $(objutil)/nvramtool/nvramtool ; \
		printf "    CREATE     $(2) (from $(1))\n"; \
		$(objutil)/nvramtool/nvramtool -y $(top)/$(call strip_quotes,$(CONFIG_CMOS_LAYOUT_FILE)) -D $(2).tmp -p $(1) && \
		mv $(2).tmp $(2))

#######################################################################
# Reduce a .config file by removing lines about default unset booleans
# arg1: input
# arg2: output
define cbfs-files-processor-config
	$(eval $(2): $(1) $(obj)/build.h $(objutil)/kconfig/conf; \
		+printf "    CREATE     $(2) (from $(1))\n"; \
		printf "# This image was built using coreboot " > $(2).tmp && \
		grep "\<COREBOOT_VERSION\>" $(obj)/build.h |cut -d\" -f2 >> $(2).tmp && \
		$(MAKE) DOTCONFIG=$(1) DEFCONFIG=$(2).tmp2 savedefconfig && \
		cat $(2).tmp2 >> $(2).tmp && \
		printf "# End of defconfig. Derivable values start here.\n" >> $(2).tmp && \
		grep "^CONFIG" $(1) | grep -F -v -f $(2).tmp2 >> $(2).tmp && \
		rm -f $(2).tmp2 && \
		mv -f $(2).tmp $(2))
endef

#######################################################################
# Compile a C file with a bare struct definition into binary
# arg1: C source file
# arg2: binary file
cbfs-files-processor-struct= \
	$(eval $(2): $(1) $(obj)/build.h $(obj)/fmap_config.h $(KCONFIG_AUTOHEADER); \
		printf "    CC+STRIP   $(1)\n"; \
		$(CC_ramstage) -MMD $(CPPFLAGS_ramstage) $(CFLAGS_ramstage) --param asan-globals=0 $$(ramstage-c-ccopts) -include $(KCONFIG_AUTOHEADER) -MT $(2) -o $(2).tmp -c $(1) && \
		$(OBJCOPY_ramstage) -O binary --only-section='.data*' --only-section='.bss*' --set-section-flags .bss*=alloc,contents,load $(2).tmp $(2); \
		rm -f $(2).tmp) \
	$(eval DEPENDENCIES += $(2).d)

#######################################################################
# Add handler for arbitrary files in CBFS
$(call add-special-class,cbfs-files)
cbfs-files-handler= \
		$(eval tmp-cbfs-method:=$(word 2, $(subst :, ,$($(2)-file)))) \
		$(eval $(2)-file:=$(call strip_quotes,$(word 1, $(subst :, ,$($(2)-file))))) \
		$(eval tmp-cbfs-file:= ) \
		$(if $($(2)-file), \
			$(if $(wildcard $(1)$($(2)-file)), \
				$(eval tmp-cbfs-file:= $(wildcard $(1)$($(2)-file))), \
				$(eval tmp-cbfs-file:= $($(2)-file)))) \
		$(if $(strip $($(2)-required)), \
			$(if $(wildcard $(tmp-cbfs-file)),, \
				$(info This build configuration requires $($(2)-required)) \
				$(eval FAILBUILD:=1) \
			)) \
		$(if $(strip $($(2)-align)), \
			$(if $(strip $($(2)-position)), \
				$(info ERROR: It is not allowed to specify both alignment and position for $($(2)-file)) \
				$(eval FAILBUILD:=1) \
			)) \
		$(if $(tmp-cbfs-method), \
			$(eval tmp-old-cbfs-file:=$(tmp-cbfs-file)) \
			$(eval tmp-cbfs-file:=$(shell mkdir -p $(obj)/mainboard/$(MAINBOARDDIR); mktemp $(obj)/mainboard/$(MAINBOARDDIR)/cbfs-file.XXXXXX).out) \
			$(call cbfs-files-processor-$(tmp-cbfs-method),$(tmp-old-cbfs-file),$(tmp-cbfs-file))) \
		$(if $(tmp-cbfs-file), \
			$(eval cbfs-files += $(subst $(spc),*,$(tmp-cbfs-file)|$(2)|$($(2)-type)|$($(2)-compression)|$(strip $($(2)-position))|$($(2)-align)|$($(2)-options)))) \
		$(eval $(2)-name:=) \
		$(eval $(2)-type:=) \
		$(eval $(2)-compression:=) \
		$(eval $(2)-position:=) \
		$(eval $(2)-required:=) \
		$(eval $(2)-options:=) \
		$(eval $(2)-align:=)

#######################################################################
# a variety of flags for our build
CBFS_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESS_RAMSTAGE_LZMA),y)
CBFS_COMPRESS_FLAG:=LZMA
endif
ifeq ($(CONFIG_COMPRESS_RAMSTAGE_LZ4),y)
CBFS_COMPRESS_FLAG:=LZ4
endif

CBFS_PAYLOAD_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_LZMA),y)
CBFS_PAYLOAD_COMPRESS_FLAG:=LZMA
endif
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_LZ4),y)
CBFS_PAYLOAD_COMPRESS_FLAG:=LZ4
endif

CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESS_SECONDARY_PAYLOAD),y)
CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG:=LZMA
endif

CBFS_PRERAM_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESS_PRERAM_STAGES),y)
CBFS_PRERAM_COMPRESS_FLAG:=LZ4
endif

ifneq ($(CONFIG_LOCALVERSION),"")
COREBOOT_EXTRA_VERSION := -$(call strip_quotes,$(CONFIG_LOCALVERSION))
COREBOOT_EXPORTS += COREBOOT_EXTRA_VERSION
endif

CPPFLAGS_common := -Isrc -Isrc/include -Isrc/commonlib/include -Isrc/commonlib/bsd/include -I$(obj)
VBOOT_SOURCE ?= 3rdparty/vboot
CPPFLAGS_common += -I$(VBOOT_SOURCE)/firmware/include
CPPFLAGS_common += -include $(src)/include/kconfig.h
CPPFLAGS_common += -include $(src)/include/rules.h
CPPFLAGS_common += -include $(src)/commonlib/bsd/include/commonlib/bsd/compiler.h
CPPFLAGS_common += -I3rdparty
CPPFLAGS_common += -D__BUILD_DIR__=\"$(obj)\"

ifeq ($(BUILD_TIMELESS),1)
CPPFLAGS_common += -D__TIMELESS__
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_YABEL)$(CONFIG_PCI_OPTION_ROM_RUN_REALMODE),y)
CPPFLAGS_ramstage += -Isrc/device/oprom/include
endif

CFLAGS_common += -pipe -g -nostdinc -std=gnu11
CFLAGS_common += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS_common += -Wwrite-strings -Wredundant-decls -Wno-trigraphs -Wimplicit-fallthrough
CFLAGS_common += -Wshadow -Wdate-time -Wtype-limits -Wvla -Wold-style-definition
CFLAGS_common += -Wdangling-else
CFLAGS_common += -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer
CFLAGS_common += -fstrict-aliasing -ffunction-sections -fdata-sections -fno-pie
ifeq ($(CONFIG_COMPILER_GCC),y)
# Don't add these GCC specific flags when running scan-build
ifeq ($(CCC_ANALYZER_OUTPUT_FORMAT),)
CFLAGS_common += -Wno-packed-not-aligned
CFLAGS_common += -fconserve-stack
CFLAGS_common += -Wnull-dereference -Wreturn-type
CFLAGS_common += -Wlogical-op -Wduplicated-cond -Wno-array-compare
# cf. commit f69a99db (coreboot: x86: enable gc-sections)
CFLAGS_common += -Wno-unused-but-set-variable
endif
endif

ADAFLAGS_common += -gnatp
ADAFLAGS_common += -Wuninitialized -Wall -Werror
ADAFLAGS_common += -pipe -g -nostdinc
ADAFLAGS_common += -Wstrict-aliasing -Wshadow
ADAFLAGS_common += -fno-common -fomit-frame-pointer
ADAFLAGS_common += -ffunction-sections -fdata-sections
ADAFLAGS_common += -fno-pie
# Ada warning options:
#
#  a   Activate most optional warnings.
# .e   Activate every optional warnings.
#  e   Treat warnings and style checks as errors.
#
#  D   Suppress warnings on implicit dereferences:
#      As SPARK does not accept access types we have to map the
#      dynamically chosen register locations to a static SPARK
#      variable.
#
# .H   Suppress warnings on holes/gaps in records:
#      We are modelling hardware here!
#
#  H   Suppress warnings on hiding:
#      It's too annoying, you run out of ideas for identifiers fast.
#
#  T   Suppress warnings for tracking of deleted conditional code:
#      We use static options to select code paths at compile time.
#
#  U   Suppress warnings on unused entities:
#      Would have lots of warnings for unused register definitions,
#      `withs` for debugging etc.
#
# .U   Deactivate warnings on unordered enumeration types:
#      As SPARK doesn't support `pragma Ordered` by now, we don't
#      use that, yet.
#
# .W   Suppress warnings on unnecessary Warnings Off pragmas:
#      Things get really messy when you use different compiler
#      versions, otherwise.
# .Y   Disable information messages for why package spec needs body:
#      Those messages are annoying. But don't forget to enable those,
#      if you need the information.
ADAFLAGS_common += -gnatwa.eeD.HHTU.U.W.Y
# Disable style checks for now
ADAFLAGS_common += -gnatyN

LDFLAGS_common := --gc-sections -nostdlib --nmagic -static

# Workaround for RISC-V linker bug, merge back into above line when fixed.
# https://sourceware.org/bugzilla/show_bug.cgi?id=27180
ifneq ($(CONFIG_ARCH_RISCV),y)
LDFLAGS_common += --emit-relocs
endif

ifeq ($(CONFIG_WARNINGS_ARE_ERRORS),y)
CFLAGS_common += -Werror
endif
ifneq ($(GDB_DEBUG),)
CFLAGS_common += -Og
ADAFLAGS_common += -Og
else
CFLAGS_common += -Os
ADAFLAGS_common += -Os
endif

ifeq ($(CONFIG_DEBUG_ADA_CODE),y)
ADAFLAGS_common += -gnata
endif

additional-dirs += $(objutil)/cbfstool $(objutil)/ifdtool \
		   $(objutil)/options $(objutil)/amdfwtool \
		   $(objutil)/cbootimage

export $(COREBOOT_EXPORTS)

#######################################################################
# generate build support files

build_h := $(obj)/build.h

# We have to manually export variables that `genbuild_h.sh` uses
# when we call it through the `$(shell)` function. This is fragile
# but as variables newly added to `genbuild_h.sh` would just not
# work, we'd notice that instantly at least.
build_h_exports := BUILD_TIMELESS KERNELVERSION COREBOOT_EXTRA_VERSION

# Report new `build.ht` as dependency if `build.h` differs.
build_h_check := \
	export $(foreach exp,$(build_h_exports),$(exp)="$($(exp))"); \
	util/genbuild_h/genbuild_h.sh $(xcompile) \
		>$(build_h)t 2>/dev/null; \
	cmp -s $(build_h)t $(build_h) >/dev/null 2>&1 || echo $(build_h)t

$(build_h): $$(shell $$(build_h_check))
	@printf "    GEN        build.h\n"
	mv $< $@

$(obj)/build_info:
	@echo 'COREBOOT_VERSION: $(call strip_quotes,$(KERNELVERSION))' > $@.tmp
	@echo 'MAINBOARD_VENDOR: $(call strip_quotes,$(CONFIG_MAINBOARD_VENDOR))' >> $@.tmp
	@echo 'MAINBOARD_PART_NUMBER: $(call strip_quotes,$(CONFIG_MAINBOARD_PART_NUMBER))' >> $@.tmp
	mv $@.tmp $@

#######################################################################
# Build the tools
CBFSTOOL:=$(objutil)/cbfstool/cbfstool
FMAPTOOL:=$(objutil)/cbfstool/fmaptool
RMODTOOL:=$(objutil)/cbfstool/rmodtool
IFWITOOL:=$(objutil)/cbfstool/ifwitool
IFITTOOL:=$(objutil)/cbfstool/ifittool
AMDCOMPRESS:=$(objutil)/cbfstool/amdcompress
CSE_FPT:=$(objutil)/cbfstool/cse_fpt
CSE_SERGER:=$(objutil)/cbfstool/cse_serger

$(obj)/cbfstool: $(CBFSTOOL)
	cp $< $@

$(obj)/fmaptool: $(FMAPTOOL)
	cp $< $@

$(obj)/rmodtool: $(RMODTOOL)
	cp $< $@

$(obj)/ifwitool: $(IFWITOOL)
	cp $< $@

$(obj)/ifittool: $(IFITTOOL)
	cp $< $@

$(obj)/amdcompress: $(AMDCOMPRESS)
	cp $< $@

$(obj)/cse_fpt: $(CSE_FPT)
	cp $< $@

$(obj)/cse_serger: $(CSE_SERGER)
	cp $< $@

_WINCHECK=$(shell uname -o 2> /dev/null)
STACK=
ifeq ($(_WINCHECK),Msys)
	STACK=-Wl,--stack,16384000
endif
ifeq ($(_WINCHECK),Cygwin)
	STACK=-Wl,--stack,16384000
endif

BINCFG:=$(objutil)/bincfg/bincfg

IFDTOOL:=$(objutil)/ifdtool/ifdtool

AMDFWTOOL:=$(objutil)/amdfwtool/amdfwtool
AMDFWREAD:=$(objutil)/amdfwtool/amdfwread

APCB_EDIT_TOOL:=$(top)/util/apcb/apcb_edit.py

APCB_V3_EDIT_TOOL:=$(top)/util/apcb/apcb_v3_edit.py

CBOOTIMAGE:=$(objutil)/cbootimage/cbootimage

FUTILITY?=$(objutil)/futility/futility

subdirs-y += util/nvidia

$(obj)/config.h: $(objutil)/kconfig/conf

#######################################################################
# needed objects that every mainboard uses
# Creation of these is architecture and mainboard independent
DEVICETREE_FILE := $(src)/mainboard/$(MAINBOARDDIR)/$(CONFIG_DEVICETREE)

SCONFIG_OPTIONS := --mainboard_devtree=$(DEVICETREE_FILE)

ifneq ($(CONFIG_OVERRIDE_DEVICETREE),)
OVERRIDE_DEVICETREE_FILE := $(src)/mainboard/$(MAINBOARDDIR)/$(CONFIG_OVERRIDE_DEVICETREE)
SCONFIG_OPTIONS += --override_devtree=$(OVERRIDE_DEVICETREE_FILE)
endif

ifneq ($(CONFIG_CHIPSET_DEVICETREE),)
CHIPSET_DEVICETREE_FILE := $(src)/$(CONFIG_CHIPSET_DEVICETREE)
SCONFIG_OPTIONS += --chipset_devtree=$(CHIPSET_DEVICETREE_FILE)
endif

DEVICETREE_STATIC_C := $(obj)/mainboard/$(MAINBOARDDIR)/static.c
SCONFIG_OPTIONS += --output_c=$(DEVICETREE_STATIC_C)

DEVICETREE_STATIC_H := $(obj)/static.h
SCONFIG_OPTIONS += --output_h=$(DEVICETREE_STATIC_H)

DEVICETREE_DEVICENAMES_H := $(obj)/static_devices.h
SCONFIG_OPTIONS += --output_d=$(DEVICETREE_DEVICENAMES_H)

DEVICETREE_FWCONFIG_H := $(obj)/static_fw_config.h
SCONFIG_OPTIONS += --output_f=$(DEVICETREE_FWCONFIG_H)

$(DEVICETREE_STATIC_C): $(DEVICETREE_FILE) $(OVERRIDE_DEVICETREE_FILE) $(CHIPSET_DEVICETREE_FILE) $(objutil)/sconfig/sconfig
	@printf "    SCONFIG    $(subst $(src)/,,$(<))\n"
	mkdir -p $(dir $(DEVICETREE_STATIC_C))
	$(objutil)/sconfig/sconfig $(SCONFIG_OPTIONS)

ramstage-y+=$(DEVICETREE_STATIC_C)
romstage-y+=$(DEVICETREE_STATIC_C)
verstage-y+=$(DEVICETREE_STATIC_C)
bootblock-y+=$(DEVICETREE_STATIC_C)
postcar-y+=$(DEVICETREE_STATIC_C)
smm-y+=$(DEVICETREE_STATIC_C)

# Ensure static.c and static.h are created before any objects are compiled
ramstage-c-deps+=$(DEVICETREE_STATIC_C)
romstage-c-deps+=$(DEVICETREE_STATIC_C)
verstage-c-deps+=$(DEVICETREE_STATIC_C)
bootblock-c-deps+=$(DEVICETREE_STATIC_C)
postcar-c-deps+=$(DEVICETREE_STATIC_C)
smm-c-deps+=$(DEVICETREE_STATIC_C)

.PHONY: devicetree
devicetree: $(DEVICETREE_STATIC_C)

ramstage-y += $(CONFIG_MEMLAYOUT_LD_FILE)
romstage-y += $(CONFIG_MEMLAYOUT_LD_FILE)
bootblock-y += $(CONFIG_MEMLAYOUT_LD_FILE)
verstage-y += $(CONFIG_MEMLAYOUT_LD_FILE)
postcar-y += $(CONFIG_MEMLAYOUT_LD_FILE)
decompressor-y += $(CONFIG_MEMLAYOUT_LD_FILE)

#######################################################################
# Clean up rules
clean-abuild:
	rm -rf coreboot-builds

#######################################################################
# Development utilities
printcrt0s:
	@echo crt0s=$(crt0s)
	@echo ldscripts=$(ldscripts)

update:
	dongle.py -c /dev/term/1 $(obj)/coreboot.rom EOF

check-style:
	grep "^# DESCR:" util/lint/check-style | sed "s,.*DESCR: *,,"
	echo "========"
	util/lint/check-style
	echo "========"

gitconfig:
	util/gitconfig/gitconfig.sh "$(MAKE)"

install-git-commit-clangfmt:
	cp util/scripts/prepare-commit-msg.clang-format .git/hooks/prepare-commit-msg

include util/crossgcc/Makefile.inc

.PHONY: tools
tools: $(objutil)/kconfig/conf $(objutil)/kconfig/toada $(CBFSTOOL) $(objutil)/cbfstool/cbfs-compression-tool $(FMAPTOOL) $(RMODTOOL) $(IFWITOOL) $(objutil)/nvramtool/nvramtool $(objutil)/sconfig/sconfig $(IFDTOOL) $(CBOOTIMAGE) $(AMDFWTOOL) $(AMDCOMPRESS) $(FUTILITY) $(BINCFG) $(IFITTOOL) $(objutil)/supermicro/smcbiosinfo $(CSE_FPT) $(CSE_SERGER) $(AMDFWREAD)

###########################################################################
# Common recipes for all stages
###########################################################################

# loadaddr can determine the load address of a stage, which may be needed for
# platform-specific image headers (only works *after* the stage has been built)
loadaddr = $(shell $(OBJDUMP_$(1)) -p $(objcbfs)/$(1).debug | \
	     sed -ne '/LOAD/s/^.*vaddr 0x\([0-9a-fA-F]\{8\}\).*$$/0x\1/p')

# find-substr is required for stages like romstage_null and romstage_xip to
# eliminate the _* part of the string
find-substr = $(word 1,$(subst _, ,$(1)))

# find-class is used to identify the class from the name of the stage
# The input to this macro can be something like romstage.x or romstage.x.y
# find-class recursively strips off the suffixes to extract the exact class name
# e.g.: if romstage.x is provided to find-class, it will remove .x and return romstage
# if romstage.x.y is provided, it will first remove .y, call find-class with romstage.x
# and remove .x the next time and finally return romstage
find-class = $(if $(filter $(1),$(basename $(1))),$(if $(CC_$(1)), $(1), $(call find-substr,$(1))),$(call find-class,$(basename $(1))))

# Bootblocks are not CBFS stages. coreboot is currently expecting the bss to
# be cleared by the loader of the stage. For ARM SoCs that means one needs to
# include the bss section in the binary so the BootROM clears the bss on
# loading of the bootblock stage. Achieve this by marking the bss section
# loadable,allocatable, and data. Do the same for the .data section in case
# the linker marked it NOBITS automatically because there are only zeroes in it.
preserve-bss-flags := --set-section-flags .bss=load,alloc,data --set-section-flags .data=load,alloc,data

# For Intel TXT files in the CBFS needs to be marked as 'Initial Boot Block'.
# As CBFS attributes aren't cheap, only mark them if TXT is enabled.
ifeq ($(CONFIG_INTEL_TXT),y)

TXTIBB := --ibb

else

TXTIBB :=

endif

ifeq ($(CONFIG_COMPRESS_BOOTBLOCK),y)

$(objcbfs)/bootblock.lz4: $(objcbfs)/bootblock.elf $(objutil)/cbfstool/cbfs-compression-tool
	@printf "    LZ4        $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) $(preserve-bss-flags) $< $@.tmp
	$(OBJCOPY_bootblock) -O binary $@.tmp
	$(objutil)/cbfstool/cbfs-compression-tool rawcompress $@.tmp $@.tmp2 lz4
	rm -f $@.tmp
	mv $@.tmp2 $@

# Put assembled decompressor+bootblock into bootblock.raw.elf so that SoC
# Makefiles wrapping the bootblock in a header can always key off the same file.
$(objcbfs)/bootblock.raw.elf: $(objcbfs)/decompressor.elf
	@printf "    OBJCOPY    $(notdir $(@))\n"
	$(OBJCOPY_bootblock) $(preserve-bss-flags) $< $@

else	# CONFIG_COMPRESS_BOOTBLOCK

$(objcbfs)/bootblock.raw.elf: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(notdir $(@))\n"
	$(OBJCOPY_bootblock) $(preserve-bss-flags) $< $@

endif	# CONFIG_COMPRESS_BOOTBLOCK

$(objcbfs)/bootblock.raw.bin: $(objcbfs)/bootblock.raw.elf
	@printf "    OBJCOPY    $(notdir $(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@

ifneq ($(CONFIG_HAVE_BOOTBLOCK),y)
$(objcbfs)/bootblock.bin:
	dd if=/dev/zero of=$@ bs=64 count=1
endif

$(objcbfs)/%.bin: $(objcbfs)/%.raw.bin
	cp $< $@

$(objcbfs)/%.map: $(objcbfs)/%.debug
	$(eval class := $(call find-class,$(@F)))
	$(NM_$(class)) -n $< | sort > $(basename $@).map

$(objcbfs)/%.elf: $(objcbfs)/%.debug $(objcbfs)/%.map
	$(eval class := $(call find-class,$(@F)))
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cp $< $@.tmp
	$(OBJCOPY_$(class)) --strip-debug $@.tmp
	$(OBJCOPY_$(class)) --add-gnu-debuglink=$< $@.tmp
	mv $@.tmp $@


###########################################################################
# Build the final rom image
###########################################################################

# extract_nth - Return a subsection of the $file string
#
# the input string looks like this:
# ./build/cbfs/fallback/romstage.elf|fallback/romstage|stage|none||64|--xip*-S*.car.data*-P*0x10000
#
# Sections:
# 1 - Path and name of file [FILENAME: Added to cbfs-files-y list variable]
# 2 - Name of file in cbfs  [$(FILENAME)-file]
# 3 - File type:            [$(FILENAME)-type]
#                bootblock, cbfs header, stage, payload, optionrom, bootsplash, raw, vsa,
#                mbi, microcode, fsp, mrc, cmos_default, cmos_layout, spd, mrc_cache,
#                mma, efi, deleted, null
# 4 - Compression type      [$(FILENAME)-compression]
#                      none, LZMA
# 5 - Base address          [$(FILANAME)-position]
# 6 - Alignment             [$(FILENAME)-align]
# 7 - cbfstool flags        [$(FILENAME)-options]
#
# Input:
#  $(1) = Section to extract
#  $(2) = Input string
#
# Steps:
# 1) replace all '|' characters with the sequence '- -' within the full string, prepended and appended with the character '-'
# 2) extract the specified section from the string - this gets us the section surrounded by '-' characters
# 3) remove the leading and trailing '-' characters
# 4) replace all '*' characters with spaces
extract_nth=$(subst *,$(spc),$(patsubst -%-,%,$(word $(1), $(subst |,- -,-$(2)-))))

# regions-for-file - Returns a cbfstool regions parameter
# $(call regions-for-file,$(filename))
# returns "REGION1,REGION2,..."
#
# This is the default implementation. When using a boot strategy employing
# multiple CBFSes in fmap regions, override it.
regions-for-file ?= COREBOOT

ifeq ($(CONFIG_CBFS_AUTOGEN_ATTRIBUTES),y)
	cbfs-autogen-attributes=-g
endif

# cbfs-add-cmd-for-region
# $(call cbfs-add-cmd-for-region,file in extract_nth format,region name)
#
# CBFSTOOL_ADD_CMD_OPTIONS can be used by arch/SoC/mainboard to supply
# add commands with any additional arguments for cbfstool.
# Example: --ext-win-base <base> --ext-win-size <size>
define cbfs-add-cmd-for-region
	$(CBFSTOOL) $@.tmp \
	add$(if $(filter stage,$(call extract_nth,3,$(1))),-stage)$(if \
		$(filter payload,$(call extract_nth,3,$(1))),-payload)$(if \
		$(filter flat-binary,$(call extract_nth,3,$(1))),-flat-binary) \
	-f $(call extract_nth,1,$(1)) \
	-n $(call extract_nth,2,$(1)) \
	$(if $(filter-out flat-binary payload stage,$(call \
		extract_nth,3,$(1))),-t $(call extract_nth,3,$(1))) \
	$(if $(call extract_nth,4,$(1)),-c $(call extract_nth,4,$(1))) \
	$(cbfs-autogen-attributes) \
	-r $(2) \
	$(if $(call extract_nth,6,$(1)),-a $(call extract_nth,6,$(file)), \
		$(if $(call extract_nth,5,$(file)),-b $(call extract_nth,5,$(file)))) \
		$(call extract_nth,7,$(1)) \
	$(CBFSTOOL_ADD_CMD_OPTIONS)

endef
# Empty line before endef is necessary so cbfs-add-cmd-for-region ends in a
# newline

# cbfs-add-cmd
# $(call cbfs-add-cmd,
#          file in extract_nth format,
#          region name,
#          non-empty if file removal requested)
define cbfs-add-cmd
	printf "    CBFS       $(call extract_nth,2,$(1))\n"
	$(if $(3),-$(CBFSTOOL) $@.tmp remove -n $(call extract_nth,2,$(file)) 2>/dev/null)
	$(call cbfs-add-cmd-for-region,$(1),$(2))
endef

# list of files to add (using their file system names, not CBFS names),
# for dependencies etc.
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))

# $(all-regions)
# returns full list of fmap regions that we add files to
all-regions = $(sort $(subst $(comma),$(spc), \
	$(foreach file,$(cbfs-files), \
		$(call regions-for-file,$(call extract_nth,2,$(file))))))

# $(call all-files-in-region,region name)
# returns elements in $(cbfs-files) that end up in that region, in the order
# they appear in $(cbfs-files)
all-files-in-region = $(foreach file,$(cbfs-files), \
	$(if $(filter $(1), \
		$(subst $(comma),$(spc),$(call regions-for-file,$(call extract_nth,2,$(file))))), \
		$(file)))

# $(call update-file-for-region,file string from $(cbfs-files),region name)
# Update position and alignment according to overrides for region
# Doesn't check for invalid configurations (eg. resetting neither or both
# position and align)
# Returns the updated file string
update-file-for-region = \
	$(subst $(spc),*,$(call extract_nth,1,$(1))|$(call extract_nth,2,$(1))|$(call extract_nth,3,$(1))|$(call extract_nth,4,$(1))|$($(call extract_nth,2,$(1))-$(2)-position)|$($(call extract_nth,2,$(1))-$(2)-align)|$(call extract_nth,7,$(1)))

# $(call placed-files-in-region,region name)
# like all-files-in-region, but updates the files to contain region overrides
# to position or alignment.
placed-files-in-region = $(foreach file,$(call all-files-in-region,$(1)), \
	$(if $($(call extract_nth,2,$(file))-$(1)-position), \
		$(if $($(call extract_nth,2,$(file))-$(1)-align), \
			$(error It is not allowed to specify both alignment and position for $(call extract_nth,2,$(file))-$(1))) \
		$(call update-file-for-region,$(file),$(1)), \
		$(if $($(call extract_nth,2,$(file))-$(1)-align), \
			$(call update-file-for-region,$(file),$(1)), \
			$(file))))

# $(call sort-files,subset of $(cbfs-files))
# reorders the files in the given set to list files at fixed positions first,
# followed by aligned files and finally those with no constraints.
sort-files = \
	$(eval _tmp_fixed:=) \
	$(eval _tmp_aligned:=) \
	$(eval _tmp_regular:=) \
	$(foreach file,$(1), \
		$(if $(call extract_nth,5,$(file)),\
			$(eval _tmp_fixed += $(file)), \
		$(if $(call extract_nth,6,$(file)), \
			$(eval _tmp_aligned += $(file)), \
			$(eval _tmp_regular += $(file))))) \
	$(_tmp_fixed) $(_tmp_aligned) $(_tmp_regular)

# command list to add files to CBFS
prebuild-files = $(foreach region,$(all-regions), \
	$(foreach file, \
		$(call sort-files,$(call placed-files-in-region,$(region))), \
		$(call cbfs-add-cmd,$(file),$(region),$(CONFIG_UPDATE_IMAGE))))

ifeq ($(CONFIG_FMDFILE),)
# For a description of the flash layout described by these variables, check
# the $(DEFAULT_FLASHMAP) .fmd files.
ifeq ($(CONFIG_ARCH_X86),y)
DEFAULT_FLASHMAP:=$(top)/util/cbfstool/default-x86.fmd
# entire flash
FMAP_ROM_ADDR := $(call int-subtract, 0x100000000 $(CONFIG_ROM_SIZE))
FMAP_ROM_SIZE := $(CONFIG_ROM_SIZE)
# entire "BIOS" region (everything directly of concern to the host system)
# relative to ROM_BASE
FMAP_BIOS_BASE := $(call int-align, $(call int-subtract, $(CONFIG_ROM_SIZE) $(CONFIG_CBFS_SIZE)), 0x10000)
FMAP_BIOS_SIZE := $(call int-align-down, $(shell echo $(CONFIG_CBFS_SIZE) | tr A-F a-f), 0x10000)
# position and size of flashmap, relative to BIOS_BASE

#
# X86 CONSOLE FMAP region
#
# position, size and entry line of CONSOLE relative to BIOS_BASE, if enabled

FMAP_CURRENT_BASE := 0

ifeq ($(CONFIG_CONSOLE_SPI_FLASH),y)
FMAP_CONSOLE_BASE := $(FMAP_CURRENT_BASE)
FMAP_CONSOLE_SIZE := $(CONFIG_CONSOLE_SPI_FLASH_BUFFER_SIZE)
FMAP_CONSOLE_ENTRY := CONSOLE@$(FMAP_CONSOLE_BASE) $(FMAP_CONSOLE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_CONSOLE_BASE) $(FMAP_CONSOLE_SIZE))
else
FMAP_CONSOLE_ENTRY :=
endif

ifeq ($(CONFIG_CACHE_MRC_SETTINGS),y)
FMAP_MRC_CACHE_BASE := $(call int-align, $(FMAP_CURRENT_BASE), 0x10000)
FMAP_MRC_CACHE_SIZE := $(CONFIG_MRC_SETTINGS_CACHE_SIZE)
FMAP_MRC_CACHE_ENTRY := RW_MRC_CACHE@$(FMAP_MRC_CACHE_BASE) $(FMAP_MRC_CACHE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_MRC_CACHE_BASE) $(FMAP_MRC_CACHE_SIZE))
else
FMAP_MRC_CACHE_ENTRY :=
endif

ifeq ($(CONFIG_SMMSTORE),y)
FMAP_SMMSTORE_BASE := $(call int-align, $(FMAP_CURRENT_BASE), 0x10000)
FMAP_SMMSTORE_SIZE := $(CONFIG_SMMSTORE_SIZE)
FMAP_SMMSTORE_ENTRY := SMMSTORE@$(FMAP_SMMSTORE_BASE) $(FMAP_SMMSTORE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_SMMSTORE_BASE) $(FMAP_SMMSTORE_SIZE))
else
FMAP_SMMSTORE_ENTRY :=
endif

ifeq ($(CONFIG_SPD_CACHE_IN_FMAP),y)
FMAP_SPD_CACHE_BASE := $(call int-align, $(FMAP_CURRENT_BASE), 0x4000)
FMAP_SPD_CACHE_SIZE := $(call int-multiply, $(CONFIG_DIMM_MAX) $(CONFIG_DIMM_SPD_SIZE))
FMAP_SPD_CACHE_SIZE := $(call int-align, $(FMAP_SPD_CACHE_SIZE), 0x1000)
FMAP_SPD_CACHE_ENTRY := $(CONFIG_SPD_CACHE_FMAP_NAME)@$(FMAP_SPD_CACHE_BASE) $(FMAP_SPD_CACHE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_SPD_CACHE_BASE) $(FMAP_SPD_CACHE_SIZE))
else
FMAP_SPD_CACHE_ENTRY :=
endif

ifeq ($(CONFIG_VPD),y)
FMAP_VPD_BASE := $(call int-align, $(FMAP_CURRENT_BASE), 0x4000)
FMAP_VPD_SIZE := $(CONFIG_VPD_FMAP_SIZE)
FMAP_VPD_ENTRY := $(CONFIG_VPD_FMAP_NAME)@$(FMAP_VPD_BASE) $(FMAP_VPD_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_VPD_BASE) $(FMAP_VPD_SIZE))
else
FMAP_VPD_ENTRY :=
endif

#
# X86 FMAP region
#
#
# position, size
FMAP_FMAP_BASE := $(FMAP_CURRENT_BASE)
FMAP_FMAP_SIZE := 0x200

#
# X86 COREBOOT default cbfs FMAP region
#
# position and size of CBFS, relative to BIOS_BASE
FMAP_CBFS_BASE := $(call int-add, $(FMAP_FMAP_BASE) $(FMAP_FMAP_SIZE))
FMAP_CBFS_SIZE := $(call int-subtract, $(FMAP_BIOS_SIZE) $(FMAP_CBFS_BASE))

else # ifeq ($(CONFIG_ARCH_X86),y)

DEFAULT_FLASHMAP:=$(top)/util/cbfstool/default.fmd
# entire flash
FMAP_ROM_ADDR := 0
FMAP_ROM_SIZE := $(CONFIG_ROM_SIZE)
# entire "BIOS" region (everything directly of concern to the host system)
# relative to ROM_BASE
FMAP_BIOS_BASE := 0
FMAP_BIOS_SIZE := $(CONFIG_CBFS_SIZE)
# position and size of flashmap, relative to BIOS_BASE
FMAP_FMAP_BASE := 0x20000
FMAP_FMAP_SIZE := 0x200

FMAP_CURRENT_BASE := $(call int-add, $(FMAP_FMAP_BASE) $(FMAP_FMAP_SIZE))

#
# NON-X86 CONSOLE FMAP region
#
# position, size and entry line of CONSOLE relative to BIOS_BASE, if enabled
ifeq ($(CONFIG_CONSOLE_SPI_FLASH),y)
FMAP_CONSOLE_BASE := $(FMAP_CURRENT_BASE)
FMAP_CONSOLE_SIZE := $(CONFIG_CONSOLE_SPI_FLASH_BUFFER_SIZE)
FMAP_CONSOLE_ENTRY := CONSOLE@$(FMAP_CONSOLE_BASE) $(FMAP_CONSOLE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_CONSOLE_BASE) $(FMAP_CONSOLE_SIZE))
else
FMAP_CONSOLE_ENTRY :=
endif

#
# NON-X86 RW_MRC_CACHE FMAP region
#
# position, size and entry line of MRC_CACHE relative to BIOS_BASE, if enabled
ifeq ($(CONFIG_CACHE_MRC_SETTINGS),y)
FMAP_MRC_CACHE_BASE := $(call int-align, $(FMAP_CURRENT_BASE), 0x10000)
FMAP_MRC_CACHE_SIZE := $(CONFIG_MRC_SETTINGS_CACHE_SIZE)
FMAP_MRC_CACHE_ENTRY := RW_MRC_CACHE@$(FMAP_MRC_CACHE_BASE) $(FMAP_MRC_CACHE_SIZE)
FMAP_CURRENT_BASE := $(call int-add, $(FMAP_MRC_CACHE_BASE) $(FMAP_MRC_CACHE_SIZE))
else
FMAP_MRC_CACHE_ENTRY :=
endif

#
# NON-X86 COREBOOT default cbfs FMAP region
#
# position and size of CBFS, relative to BIOS_BASE
FMAP_CBFS_BASE := $(FMAP_CURRENT_BASE)
FMAP_CBFS_SIZE := $(call int-subtract,$(FMAP_BIOS_SIZE) $(FMAP_CBFS_BASE))

endif # ifeq ($(CONFIG_ARCH_X86),y)

$(obj)/fmap.fmd: $(top)/Makefile.inc $(DEFAULT_FLASHMAP) $(obj)/config.h
	sed -e "s,##ROM_BASE##,$(FMAP_ROM_ADDR)," \
	    -e "s,##ROM_SIZE##,$(FMAP_ROM_SIZE)," \
	    -e "s,##BIOS_BASE##,$(FMAP_BIOS_BASE)," \
	    -e "s,##BIOS_SIZE##,$(FMAP_BIOS_SIZE)," \
	    -e "s,##FMAP_BASE##,$(FMAP_FMAP_BASE)," \
	    -e "s,##FMAP_SIZE##,$(FMAP_FMAP_SIZE)," \
	    -e "s,##CONSOLE_ENTRY##,$(FMAP_CONSOLE_ENTRY)," \
	    -e "s,##MRC_CACHE_ENTRY##,$(FMAP_MRC_CACHE_ENTRY)," \
	    -e "s,##SMMSTORE_ENTRY##,$(FMAP_SMMSTORE_ENTRY)," \
	    -e "s,##SPD_CACHE_ENTRY##,$(FMAP_SPD_CACHE_ENTRY)," \
	    -e "s,##VPD_ENTRY##,$(FMAP_VPD_ENTRY)," \
	    -e "s,##CBFS_BASE##,$(FMAP_CBFS_BASE)," \
	    -e "s,##CBFS_SIZE##,$(FMAP_CBFS_SIZE)," \
		$(DEFAULT_FLASHMAP) > $@.tmp
	mv $@.tmp $@
else # ifeq ($(CONFIG_FMDFILE),)
$(obj)/fmap.fmd: $(CONFIG_FMDFILE) $(obj)/config.h
	$(HOSTCC) $(PREPROCESS_ONLY) -include $(obj)/config.h $< -o $@.pre
	mv $@.pre $@
endif # ifeq ($(CONFIG_FMDFILE),)

# generated at the same time as fmap.fmap
$(obj)/fmap_config.h: $(obj)/fmap.fmap
	true
$(obj)/fmap.desc: $(obj)/fmap.fmap
	true

$(obj)/fmap.fmap: $(obj)/fmap.fmd $(FMAPTOOL)
	echo "    FMAP       $(FMAPTOOL) -h $(obj)/fmap_config.h $< $@"
	$(FMAPTOOL) -h $(obj)/fmap_config.h -R $(obj)/fmap.desc $< $@

ifeq ($(CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK),y)
TS_OPTIONS := -j $(CONFIG_INTEL_TOP_SWAP_BOOTBLOCK_SIZE)
endif

ifneq ($(CONFIG_ARCH_X86),y)
add_bootblock = $(CBFSTOOL) $(1) write -u -r BOOTBLOCK -f $(2)
endif

# coreboot.pre doesn't follow the standard Make conventions. It gets modified
# by multiple rules, and thus we can't compute the dependencies correctly.
$(shell rm -f $(obj)/coreboot.pre)

ifneq ($(CONFIG_UPDATE_IMAGE),y)
$(obj)/coreboot.pre: $$(prebuilt-files) $(CBFSTOOL) $(obj)/fmap.fmap $(obj)/fmap.desc $(objcbfs)/bootblock.bin
	$(CBFSTOOL) $@.tmp create -M $(obj)/fmap.fmap -r $(shell cat $(obj)/fmap.desc)
	printf "    BOOTBLOCK\n"
	$(call add_bootblock,$@.tmp,$(objcbfs)/bootblock.bin)
	$(prebuild-files) true
	mv $@.tmp $@
else # ifneq ($(CONFIG_UPDATE_IMAGE),y)
.PHONY: $(obj)/coreboot.pre
$(obj)/coreboot.pre: $$(prebuilt-files) $(CBFSTOOL)
	mv $(obj)/coreboot.rom $@.tmp || \
	(echo "Error: You have UPDATE_IMAGE set in Kconfig, but have no existing image to update." && \
	echo "Exiting." && \
	false)
	$(prebuild-files) true
	mv $@.tmp $@
endif # ifneq ($(CONFIG_UPDATE_IMAGE),y)

ifeq ($(CONFIG_HAVE_REFCODE_BLOB),y)
REFCODE_BLOB=$(obj)/refcode.rmod
$(REFCODE_BLOB): $(RMODTOOL)
	$(RMODTOOL) -i $(CONFIG_REFCODE_BLOB_FILE) -o $@
endif

ifeq ($(CONFIG_HAVE_RAMSTAGE),y)
RAMSTAGE=$(objcbfs)/ramstage.elf
else
RAMSTAGE=
endif

add_intermediate = \
	$(1): $(obj)/coreboot.pre $(2) | $(INTERMEDIATE) \
	$(eval INTERMEDIATE+=$(1)) $(eval PHONY+=$(1))

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(CBFSTOOL) $(IFITTOOL) $$(INTERMEDIATE)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
# The full ROM may be larger than the CBFS part, so create an empty
# file (filled with \377 = 0xff) and copy the CBFS image over it.
	dd if=/dev/zero bs=$(call _toint,$(CONFIG_ROM_SIZE)) count=1 2> /dev/null | tr '\000' '\377' > $@.tmp
	dd if=$(obj)/coreboot.pre of=$@.tmp bs=8192 conv=notrunc 2> /dev/null
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
# Print final FIT table
	$(IFITTOOL) -f $@.tmp -D -r COREBOOT
# Print final TS BOOTBLOCK FIT table
ifeq ($(CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK),y)
	@printf "    TOP SWAP FIT table\n"
	$(IFITTOOL) -f $@.tmp -D $(TS_OPTIONS) -r COREBOOT
endif # CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK
endif # CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE
	mv $@.tmp $@
	@printf "    CBFSLAYOUT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ layout
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print -r $(subst $(spc),$(comma),$(all-regions))
ifeq ($(CONFIG_CBFS_VERIFICATION),y)
	line=$$($(CBFSTOOL) $@ print -kv 2>/dev/null | grep -F '[CBFS VERIFICATION (COREBOOT)]') ;\
	if ! printf "$$line" | grep -q 'fully valid'; then \
		echo "CBFS verification error: $$line" ;\
		exit 1 ;\
	fi
endif # CONFIG_CBFS_VERIFICATION

cbfs-files-y += $(CONFIG_CBFS_PREFIX)/romstage
$(CONFIG_CBFS_PREFIX)/romstage-file := $(objcbfs)/romstage.elf
$(CONFIG_CBFS_PREFIX)/romstage-type := stage
$(CONFIG_CBFS_PREFIX)/romstage-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
$(CONFIG_CBFS_PREFIX)/romstage-options := -b 0
endif
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)
# Use a 64 byte alignment to provide a minimum alignment
# requirement for the overall romstage. While the first object within
# romstage could have a 4 byte minimum alignment that doesn't mean the linker
# won't decide the entire section should be aligned to a larger value. In the
# future cbfstool should add XIP files proper and honor the alignment
# requirements of the program segment.
#
# Make sure that segment for .car.data is ignored while adding romstage.
$(CONFIG_CBFS_PREFIX)/romstage-align := 64
$(CONFIG_CBFS_PREFIX)/romstage-options := -S ".car.data"

# If CAR does not support execution of code, romstage on x86 is expected to be
# xip.
ifneq ($(CONFIG_NO_XIP_EARLY_STAGES),y)
$(CONFIG_CBFS_PREFIX)/romstage-options += --xip

# For efficient MTRR utilisation use natural alignment for romstage.
ifeq ($(CONFIG_SETUP_XIP_CACHE),y)
$(CONFIG_CBFS_PREFIX)/romstage-options += --pow2page
endif    # CONFIG_SETUP_XIP_CACHE

endif   # CONFIG_NO_XIP_EARLY_STAGES
endif   # CONFIG_ARCH_ROMSTAGE_X86_32 / CONFIG_ARCH_ROMSTAGE_X86_64
ifeq ($(CONFIG_VBOOT_STARTS_IN_ROMSTAGE),y)
$(CONFIG_CBFS_PREFIX)/romstage-options += $(TXTIBB)
endif

cbfs-files-$(CONFIG_HAVE_RAMSTAGE) += $(CONFIG_CBFS_PREFIX)/ramstage
$(CONFIG_CBFS_PREFIX)/ramstage-file := $(RAMSTAGE)
$(CONFIG_CBFS_PREFIX)/ramstage-type := stage
$(CONFIG_CBFS_PREFIX)/ramstage-compression := $(CBFS_COMPRESS_FLAG)
# The AMD LPC SPI DMA controller requires source files to be 64 byte aligned.
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
$(CONFIG_CBFS_PREFIX)/ramstage-align := 64
endif

cbfs-files-$(CONFIG_HAVE_REFCODE_BLOB) += $(CONFIG_CBFS_PREFIX)/refcode
$(CONFIG_CBFS_PREFIX)/refcode-file := $(REFCODE_BLOB)
$(CONFIG_CBFS_PREFIX)/refcode-type := stage
$(CONFIG_CBFS_PREFIX)/refcode-compression := $(CBFS_COMPRESS_FLAG)

cbfs-files-$(CONFIG_SEABIOS_VGA_COREBOOT) += vgaroms/seavgabios.bin
vgaroms/seavgabios.bin-file := $(CONFIG_PAYLOAD_VGABIOS_FILE)
vgaroms/seavgabios.bin-type := raw

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += config
config-file := $(DOTCONFIG):config
config-type := raw
config-compression := LZMA

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += revision
revision-file := $(obj)/build.h
revision-type := raw

cbfs-files-y += build_info
build_info-file := $(obj)/build_info
build_info-type := raw

BOOTSPLASH_SUFFIX=$(suffix $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE)))
cbfs-files-$(CONFIG_BOOTSPLASH_IMAGE) += bootsplash$(BOOTSPLASH_SUFFIX)
bootsplash$(BOOTSPLASH_SUFFIX)-file := $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE))
bootsplash$(BOOTSPLASH_SUFFIX)-type := bootsplash

# Ensure that no payload segment overlaps with memory regions used by ramstage
# (not for x86 since it can relocate itself in that case)
ifneq ($(CONFIG_ARCH_X86),y)
check-ramstage-overlap-regions := ramstage
check-ramstage-overlap-files :=
ifneq ($(CONFIG_PAYLOAD_NONE),y)
check-ramstage-overlap-files += $(CONFIG_CBFS_PREFIX)/payload
endif

# will output one or more lines of "<load address in hex> <memlen in decimal>"
cbfs-get-segments-cmd = $(CBFSTOOL) $(obj)/coreboot.pre print -v | sed -n \
	'\%$(1)%,\%^[^ ]\{4\}%s%    .*load: \(0x[0-9a-fA-F]*\),.*length: [0-9]*/\([0-9]*\).*%\1 \2%p'

ramstage-symbol-addr-cmd = $(OBJDUMP_ramstage) -t $(objcbfs)/ramstage.elf | \
	sed -n '/ $(1)$$/s/^\([0-9a-fA-F]*\) .*/0x\1/p' | \
	uniq

$(call add_intermediate, check-ramstage-overlaps)
	programs=$$($(foreach file,$(check-ramstage-overlap-files), \
		$(call cbfs-get-segments-cmd,$(file)) ; )) ; \
	regions=$$($(foreach region,$(check-ramstage-overlap-regions), \
		echo $(region) ; \
		$(call ramstage-symbol-addr-cmd,_$(region)) ; \
		$(call ramstage-symbol-addr-cmd,_e$(region)) ; )) ; \
	pstart= ; pend= ; \
	for x in $$programs; do \
	    if [ -z $$pstart ]; then pstart=$$(($$x)) ; continue ; fi ; \
	    pend=$$(($$pstart + $$x)) ; \
	    rname= ; rstart= ; rend= ; \
	    for y in $$regions ; do \
	        if [ -z $$rname ]; then rname=$$y ; continue ; fi ; \
	        if [ -z $$rstart ]; then rstart=$$(($$y)) ; continue ; fi ; \
	        rend=$$(($$y)) ; \
	        if [ $$pstart -lt $$rend -a $$rstart -lt $$pend ]; then \
	            echo "ERROR: Ramstage region _$$rname overlapped by:" \
	                 $(check-ramstage-overlap-files) ; \
	            exit 1 ; \
	        fi ; \
	        rname= ; rstart= ; rend= ; \
	    done ; \
	    pstart= ; pend= ; \
	done

endif
