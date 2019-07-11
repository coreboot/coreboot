## SPDX-License-Identifier: GPL-2.0-only

TOOLCHAIN_ARCHES := i386 x64 arm aarch64 riscv ppc64 nds32le

help_toolchain help::
	@echo  '*** Toolchain targets ***'
	@echo  '  crossgcc        - Build coreboot cross-compilers for all platforms'
	@echo  '  crosstools      - Build coreboot cross-compiler and GDB for all platforms'
	@echo  '  crossgcc-clean  - Remove all built coreboot cross-compilers'
	@echo  '  iasl            - Build coreboot IASL compiler (built by all cross targets)'
	@echo  '  clang           - Build coreboot clang compiler'
	@echo  '  nasm            - Build coreboot nasm'
	@echo  '  test-toolchain  - Reports if toolchain components are out of date'
	@echo  '  crossgcc-ARCH   - Build cross-compiler for specific architecture'
	@echo  '  crosstools-ARCH - Build cross-compiler with GDB for specific architecture'
	@echo  '  ARCH can be "$(subst $(spc),"$(comma) ",$(TOOLCHAIN_ARCHES))"'
	@echo  '  Use "make [target] CPUS=#" to build toolchain using multiple cores'
	@echo  '  Use "make [target] DEST=some/path" to install toolchain there'
	@echo

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use BUILDGCC_OPTIONS= to set any crossgcc command line options
# Example: BUILDGCC_OPTIONS='-t' will keep temporary files after build
crossgcc: clean-for-update
	$(MAKE) -C util/crossgcc all_without_gdb SKIP_CLANG=1

.PHONY: crossgcc crossgcc-i386 crossgcc-x64 crossgcc-arm crossgcc-aarch64 \
	crossgcc-riscv crossgcc-power8 crossgcc-clean iasl \
	clang crosstools-i386 crosstools-x64 crosstools-arm \
	crosstools-aarch64 crosstools-riscv crosstools-power8 \
	jenkins-build-toolchain nasm

$(foreach arch,$(TOOLCHAIN_ARCHES),crossgcc-$(arch)): clean-for-update
	$(MAKE) -C util/crossgcc $(patsubst crossgcc-%,build-%,$@) build_iasl SKIP_GDB=1

crosstools: clean-for-update
	$(MAKE) -C util/crossgcc all_with_gdb SKIP_CLANG=1

iasl: clean-for-update
	$(MAKE) -C util/crossgcc build_iasl

clang: clean-for-update
	$(MAKE) -C util/crossgcc build_clang

nasm: clean-for-update
	$(MAKE) -C util/crossgcc build_nasm

$(foreach arch,$(TOOLCHAIN_ARCHES),crosstools-$(arch)): clean-for-update
	$(MAKE) -C util/crossgcc $(patsubst crosstools-%,build-%,$@) build_iasl

crossgcc-clean: clean-for-update
	$(MAKE) -C util/crossgcc clean

test-toolchain:
ifeq ($(COMPILER_OUT_OF_DATE),1)
	echo "The coreboot toolchain is not the current version."
	$(error )
else
	echo "The coreboot toolchain is the current version."
endif # ifeq ($(COMPILER_OUT_OF_DATE),1)

# This target controls what the jenkins builder tests
jenkins-build-toolchain: BUILDGCC_OPTIONS ?= -y --nocolor
jenkins-build-toolchain:
	$(MAKE) crossgcc clang KEEP_SOURCES=1 BUILDGCC_OPTIONS='$(BUILDGCC_OPTIONS)'
	#TODO: Re-enable gdb build after the builders can build it again.
	#$(MAKE) crosstools clang KEEP_SOURCES=1 BUILDGCC_OPTIONS='$(BUILDGCC_OPTIONS)'
	PATH=$(if $(DEST),$(DEST)/bin,$(top)/util/crossgcc/xgcc/bin):$$PATH; $(MAKE) what-jenkins-does
	PATH=$(if $(DEST),$(DEST)/bin,$(top)/util/crossgcc/xgcc/bin):$$PATH; $(MAKE) test-toolchain
