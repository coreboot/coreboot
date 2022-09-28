# SPDX-License-Identifier: GPL-2.0-only

# ccache integration
ifeq ($(CONFIG_CCACHE),y)

CCACHE:=$(word 1,$(wildcard $(addsuffix /ccache,$(subst :, ,$(PATH)))))
ifeq ($(CCACHE),)
$(error ccache selected, but not found in PATH)
endif

export CCACHE_COMPILERCHECK=content
export CCACHE_BASEDIR=$(top)

$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval CC_$(arch):=$(CCACHE) $(CC_$(arch))))

HOSTCC:=$(CCACHE) $(HOSTCC)
HOSTCXX:=$(CCACHE) $(HOSTCXX)
endif

# scan-build integration
ifneq ($(CCC_ANALYZER_OUTPUT_FORMAT),)

ifeq ($(CCC_ANALYZER_ANALYSIS),)
export CCC_ANALYZER_ANALYSIS := -analyzer-opt-analyze-headers
endif

$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval CC_$(arch):=CCC_CC="$(CC_$(arch))" $(CC) ))

HOSTCC:=CCC_CC="$(HOSTCC)" $(CC)
HOSTCXX:=CCC_CXX="$(HOSTCXX)" $(CXX)
endif

# include-what-you-use integration
ifeq ($(CONFIG_IWYU),y)

IWYU:=$(word 1,$(wildcard $(addsuffix /iwyu,$(subst :, ,$(PATH)))))
ifeq ($(IWYU),)
$(error include-what-you-use selected, but not found in PATH)
endif

$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval CC_$(arch):=$(IWYU) ))

CFLAGS_common := -Xiwyu --prefix_header_includes=remove  -Xiwyu --no_comments  -Xiwyu --no_fwd_decls $(CFLAGS_common)
NOCOMPILE := 1
endif

COREBOOT_STANDARD_STAGES := decompressor bootblock verstage romstage ramstage
MAP-decompressor := bootblock

ARCHDIR-i386	:= x86
ARCHDIR-x86_32	:= x86
ARCHDIR-x86_64	:= x86
ARCHDIR-arm	:= arm
ARCHDIR-arm64	:= arm64
ARCHDIR-riscv	:= riscv
ARCHDIR-ppc64	:= ppc64

CFLAGS_arm	+=
CFLAGS_arm64	+= -mgeneral-regs-only
CFLAGS_riscv	+=
CFLAGS_x86_32	+=
CFLAGS_x86_64	+= -mcmodel=large -mno-red-zone
CFLAGS_ppc64	+=

GCC_ADAFLAGS_arm	+=
GCC_ADAFLAGS_arm64	+= -mgeneral-regs-only
GCC_ADAFLAGS_riscv	+=
GCC_ADAFLAGS_x86_32	+=
GCC_ADAFLAGS_x86_64	+= -mcmodel=large -mno-red-zone
GCC_ADAFLAGS_ppc64	+=

# Some boards only provide 2K stacks, so storing lots of data there leads to
# problems. Since C rules don't allow us to statically determine the maximum
# stack use, we use 1.5K as heuristic, assuming that we typically have lots
# of tiny stack frames and the odd large one.
#
# Store larger buffers in BSS, use static to share data in cache-as-ram
# on x86.
# Since GCCs detection of dynamic array bounds unfortunately seems to be
# very basic, you'll sometimes have to use a static upper bound for the
# size and an assert() to make sure it's honored (see gpio_base3_value()
# for an example).
# (If you absolutely need a larger stack frame and are 100% sure it cannot
# cause problems, you can whitelist it with #pragma diagnostic.)
ifeq ($(CONFIG_COMPILER_GCC),y)
CFLAGS_arm	+= -Wstack-usage=1536
CFLAGS_arm64	+= -Wstack-usage=1536
CFLAGS_riscv	+= -Wstack-usage=1536
CFLAGS_ppc64	+= -Wstack-usage=1536
endif

toolchain_to_dir = \
	$(foreach arch,$(ARCH_SUPPORTED),\
	$(eval CPPFLAGS_$(arch) += \
	-Isrc/arch/$(ARCHDIR-$(arch))/include))

# set_stage_toolchain: Decides the toolchain to be used by every stage
# E.g.: If bootblock is x86_32, it sets ARCH-BOOTBLOCK-y = x86_32, whereas
# ARCH-BOOTBLOCK-n = armv7. Then, ARCH-BOOTBLOCK-y can be used anywhere to
# decide the compiler toolchain for bootblock stage
# This step is essential for initializing the toolchain for coreboot standard
# stages i.e. bootblock, romstage and ramstage, since it acts as the second
# parameter to create_class_compiler below in init_standard_toolchain
map_stage = $(strip $(if $(MAP-$(1)),$(MAP-$(1)),$(1)))
set_stage_toolchain= \
	$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval ARCH-$(1)-$($(shell \
		echo CONFIG_ARCH_$(call map_stage,$(1))_$(arch) | \
		tr '[:lower:]' '[:upper:]')) := $(arch)))

# standard-archs: Tell which architectures are used by the standard stages.
standard-archs = $(sort $(foreach stagearch, \
			  $(patsubst %,ARCH-%-y,$(COREBOOT_STANDARD_STAGES)), \
			  $($(stagearch))))

# create_class_compiler: Used to create compiler tool set for
# special classes
# @1: special class
# @2: compiler set to be used
# e.g.: smm special class uses i386 as compiler set
define create_class_compiler
$(if $(2),,$(warning *** The toolchain architecture for $(1) is unknown.) \
	$(warning CONFIG_ARCH_$(1)_* settings in $(DOTCONFIG):) \
	$(error $(shell grep CONFIG_ARCH_$(1)_ $(DOTCONFIG))))
CC_$(1) := $(CC_$(2))
GCC_$(1) := $(GCC_CC_$(2))
LD_$(1) := $(LD_$(2))
NM_$(1) := $(NM_$(2))
AR_$(1) := $(AR_$(2))
GNATBIND_$(1) := $(GNATBIND_$(2))
OBJCOPY_$(1) := $(OBJCOPY_$(2))
OBJDUMP_$(1) := $(OBJDUMP_$(2))
STRIP_$(1) := $(STRIP_$(2))
READELF_$(1) := $(READELF_$(2))
CFLAGS_$(1) = $$(CFLAGS_common) $$(CFLAGS_$(2))
ADAFLAGS_$(1) = --RTS=$$(obj)/libgnat-$(2)/ $$(ADAFLAGS_common) $$(GCC_ADAFLAGS_$(2))
CPPFLAGS_$(1) = $$(CPPFLAGS_common) $$(CPPFLAGS_$(2)) -D__ARCH_$(2)__
COMPILER_RT_$(1) := $$(COMPILER_RT_$(2))
COMPILER_RT_FLAGS_$(1) := $$(COMPILER_RT_FLAGS_$(2))
LDFLAGS_$(1) = $$(LDFLAGS_common) $$(LDFLAGS_$(2))
endef

# define_class: Allows defining any program as dynamic class and compiler tool
# set for the same based on the architecture for which the program is to be
# compiled
# @1: program (class name)
# @2: architecture for which the program needs to be compiled
# IMP: Ensure that define_class is called before any .c or .S files are added to
# the class of the program. Check subdirs-y for order of subdirectory inclusions
define define_class
classes-y += $(1)
$(eval $(call create_class_compiler,$(1),$(2)))
endef

# initialize standard toolchain (CC,AS and others) for give stage
# @1 : stage for which the toolchain is to be initialized
init_standard_toolchain = \
	$(eval $(call set_stage_toolchain,$(1))) \
	$(eval $(call create_class_compiler,$(1),$(ARCH-$(1)-y)))

init_stages = \
	$(foreach stage,$(COREBOOT_STANDARD_STAGES), \
	$(eval $(call init_standard_toolchain,$(stage))))

$(eval $(call toolchain_to_dir))

$(call init_stages)

# Test for coreboot toolchain (except when explicitly not requested)
ifneq ($(NOCOMPILE),1)
# Only run if we're doing a build (not for tests, kconfig, ...)
# rationale: gcc versions by Linux distributions tend to be quite messed up
# llvm/clang also needs patches supplied by the coreboot build
COMPILERFAIL:=0
IASLFAIL:=0

ifneq ($(CONFIG_ANY_TOOLCHAIN),y)
# Verify that the coreboot toolchain is being used.
$(foreach arch,$(sort $(foreach stage,\
	$(COREBOOT_STANDARD_STAGES),$(ARCH-$(stage)-y))), \
	$(if $(shell if [ -n "$(CC_$(arch))" ]; then \
		$(CC_$(arch)) -v 2>&1 | grep -q "coreboot toolchain" || \
		echo not-coreboot; else echo not-coreboot; fi), \
		$(eval COMPILERFAIL:=1)\
		$(warning The coreboot toolchain for '$(arch)'\
			architecture was not found.)))
# If iasl doesn't match the current coreboot version, fail the test
# TODO: Figure out if iasl is even needed for the build.
$(if $(shell if [ -n "$(IASL)" ]; then \
	$(IASL) -v 2>&1 | grep -q "coreboot toolchain" || \
	echo not-coreboot; else echo not-coreboot; fi), \
	$(eval COMPILERFAIL:=1)$(eval IASLFAIL:=1)\
	$(warning The coreboot toolchain version of iasl \
		'$(shell util/crossgcc/buildgcc -s iasl)' was not found))
else #$(CONFIG_ANY_TOOLCHAIN)
# If the coreboot toolchain isn't being used, verify that there is A toolchain
$(foreach arch,$(sort \
	$(foreach stage,$(COREBOOT_STANDARD_STAGES),$(ARCH-$(stage)-y))), \
	$(if $(CC_$(arch)),, $(eval COMPILERFAIL:=1) \
	$(warning No compiler found for '$(arch)' architecture. \
		Install one or use the coreboot toolchain?)) )
# If iasl isn't present, fail
# TODO: Figure out if iasl is even needed for the build.
$(if $(IASL),, $(eval COMPILERFAIL:=1)$(eval IASLFAIL:=1) \
	$(warning iasl not found. \
		Please install it or use the coreboot toolchain.))
endif

# If the compiler check failed, print out warnings
ifeq ($(COMPILERFAIL),1)
ifneq ($(XGCCPATH),)
$(warning )
$(warning Path to your toolchain is currently set to '$(XGCCPATH)')
endif
$(warning )
$(warning To build the entire coreboot toolchain: run 'make crossgcc')
ifeq ($(IASLFAIL),1)
$(warning To build just IASL: run 'make iasl')
endif #($(IASLFAIL),1)
$(warning For more toolchain build targets: run 'make help_toolchain')
$(warning )
ifneq ($(CONFIG_ANY_TOOLCHAIN),y)
$(warning To try to use any toolchain in your path, \
	run 'make menuconfig', then select)
$(warning the config option: 'General setup', \
	and 'Allow building with any toolchain')
$(warning Note that this is NOT supported.  \
	Using it means you're on your own.)
$(warning )
endif #($(CONFIG_ANY_TOOLCHAIN),y)
$(error Halting the build)
endif #($(COMPILERFAIL),1)

endif #($(NOCOMPILE),1)

# Run the toolchain version checks if the requested target is 'test-toolchain'
# Checks the versions of GCC, binutils, clang, and IASL
ifneq ($(MAKECMDGOALS),)
ifneq ($(filter test-toolchain,$(MAKECMDGOALS)),)
$(foreach arch, $(ARCH_SUPPORTED), \
	$(if $(shell if [ -n "$(GCC_CC_$(arch))" ]; then \
		$(GCC_CC_$(arch)) --version 2>&1 | head -n1 | rev | \
		cut -d ' ' -f 1 | rev | \
		grep -q "$$(util/crossgcc/buildgcc -s gcc)" || \
		echo not-current; fi), \
		$(eval COMPILER_OUT_OF_DATE:=1) \
		$(warning The coreboot toolchain version of gcc for '$(arch)' \
			architecture is not the current version.) \
		$(warning $(arch) gcc version from buildgcc: \
			$(shell util/crossgcc/buildgcc -s gcc)) \
		$(warning $(arch) version of gcc executable: \
			$(shell $(GCC_CC_$(arch)) --version | head -n1 | \
			rev | cut -d ' ' -f 1 | rev))) \
	$(if $(shell if [ -n "$(CLANG_CC_$(arch))" ]; then \
		$(CLANG_CC_$(arch)) --version  2>&1 | \
			sed 's/.*clang version/clang version/' | \
			head -n1 | cut -d ' ' -f 3 | \
		grep -q "$(shell util/crossgcc/buildgcc -s clang)" || \
		echo not-current; fi), \
		$(eval COMPILER_OUT_OF_DATE:=1) \
		$(warning The coreboot toolchain version of clang for \
			'$(arch)' architecture is not the current version.) \
		$(warning $(arch) clang version from buildgcc: \
			$(shell util/crossgcc/buildgcc -s clang)) \
		$(warning $(arch) version of clang executable: \
			$(shell $(CLANG_CC_$(arch)) --version 2>&1 | \
			sed 's/.*clang version/clang version/' | \
			head -n1 | cut -d ' ' -f 3))) \
	$(if $(shell if [ "$(OBJDUMP_$(arch))" != "invalidobjdump" ]; then \
		$(OBJDUMP_$(arch)) -v 2>&1 | \
		grep -q "$(shell util/crossgcc/buildgcc -s binutils)" || \
		echo not-current; fi), \
		$(eval COMPILER_OUT_OF_DATE:=1)\
		$(warning The coreboot toolchain version of binutils for \
			'$(arch)' architecture is not the current version.)) \
)
$(if $(shell if [ -n "$(IASL)" ]; then $(IASL) -v 2>&1 | \
	grep -q "$(shell util/crossgcc/buildgcc -s iasl)" || \
	echo not-coreboot; fi), \
	$(eval COMPILER_OUT_OF_DATE:=1)\
	$(warning The coreboot toolchain version of iasl \
		is not the current version) \
		$(warning $(arch) iasl version from buildgcc: \
			$(shell util/crossgcc/buildgcc -s iasl)) \
		$(warning $(arch) version of iasl executable: \
			$(shell $(IASL) -v 2>&1 | \
			grep ASL+ | rev | cut -f 1 -d ' ' | rev)))
$(eval UPDATED_SUBMODULES:=1)
endif #($(filter crossgcc_check%,$(MAKECMDGOALS)),)
endif #($(MAKECMDGOALS),)
