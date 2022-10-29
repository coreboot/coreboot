## SPDX-License-Identifier: GPL-2.0-only
# if no architecture is specified, set a default
BUILD_PLATFORM ?= i386-elf
DEST ?= $(CURDIR)/xgcc

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use KEEP_SOURCES=1 to keep temporary files after the build
# use BUILDGCC_OPTIONS= to set any other crossgcc command line options
# Example: BUILDGCC_OPTIONS=-c to remove temporary files before build

all:
	$(MAKE) build-i386 build-x64 build-arm \
		build-riscv build-aarch64 build-ppc64 build-nds32le \
		build_clang build_iasl build_nasm

###########################################################
### targets to do buildgcc builds

build_gcc:
	bash ./buildgcc -p $(BUILD_PLATFORM) $(if $(CPUS),-j $(CPUS)) $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) \
			$(if $(BUILD_LANGUAGES),-l $(BUILD_LANGUAGES)) -d $(DEST)

build_iasl:
	bash ./buildgcc -P iasl $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)

build_clang:
ifeq ($(SKIP_CLANG),)
	bash ./buildgcc -P clang $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)
endif

build_nasm:
	bash ./buildgcc -P nasm $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)

###########################################################
build-i386:
	@$(MAKE) build_gcc build_nasm BUILD_PLATFORM=i386-elf

build-x64:
	@$(MAKE) build_gcc build_nasm BUILD_PLATFORM=x86_64-elf

build-arm:
	@$(MAKE) build_gcc BUILD_PLATFORM=arm-eabi

build-aarch64:
	@$(MAKE) build_gcc BUILD_PLATFORM=aarch64-elf

build-riscv:
	@$(MAKE) build_gcc BUILD_PLATFORM=riscv-elf

build-ppc64:
	@$(MAKE) build_gcc BUILD_PLATFORM=powerpc64-linux-gnu

build-nds32le:
	@$(MAKE) build_gcc BUILD_PLATFORM=nds32le-elf

clean_tempfiles:
	rm -rf build-*
	rm -rf binutils-* gcc-* gmp-* mpc-* mpfr-*
	rm -rf llvm-* clang-tools-* cfe-* compiler-rt-*
	rm -rf acpica-*
	rm -f  getopt

clean: clean_tempfiles
	rm -rf xgcc

distclean: clean
	rm -rf tarballs

.PHONY: all build_gcc build_iasl build_clang build_nasm \
	clean distclean clean_tempfiles \
	build-i386 build-x64 build-arm build-aarch64 \
	build-riscv build-ppc64 build-nds32le

.NOTPARALLEL:
