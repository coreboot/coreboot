# if no architecture is specified, set a default
BUILD_PLATFORM ?= i386-elf
DEST ?= $(CURDIR)/xgcc

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use KEEP_SOURCES=1 to keep temporary files after the build
# use BUILDGCC_OPTIONS= to set any other crossgcc command line options
# Example: BUILDGCC_OPTIONS=-c to remove temporary files before build

all all_with_gdb:
	$(MAKE) build-i386 build-x64 build-arm build-mips \
		build-riscv build-aarch64 build-power8 build-nds32le \
		build_clang build_iasl build_make

all_without_gdb:
	$(MAKE) SKIP_GDB=1 build-i386 build-x64 build-arm build-mips \
		build-riscv build-aarch64 build-power8 build-nds32le \
		build_clang build_iasl build_make

build_tools: build_gcc build_gdb

###########################################################
### targets to do buildgcc builds

build_gcc:
	bash ./buildgcc -p $(BUILD_PLATFORM) $(if $(CPUS),-j $(CPUS)) $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) \
			$(if $(BUILD_LANGUAGES),-l $(BUILD_LANGUAGES)) -d $(DEST)

build_gdb:
ifeq ($(SKIP_GDB),)
	bash ./buildgcc -p $(BUILD_PLATFORM) -P gdb $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) \
			$(BUILDGCC_OPTIONS) -d $(DEST)
endif

build_iasl:
	bash ./buildgcc -P iasl $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)

build_clang:
ifeq ($(SKIP_CLANG),)
	bash ./buildgcc -P clang $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)
endif

build_make:
	bash ./buildgcc -P make $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) -d $(DEST)

###########################################################
build-i386:
	@$(MAKE) build_tools BUILD_PLATFORM=i386-elf

build-x64:
	@$(MAKE) build_tools BUILD_PLATFORM=x86_64-elf

build-arm:
	@$(MAKE) build_tools BUILD_PLATFORM=arm-eabi

build-aarch64:
	@$(MAKE) build_tools BUILD_PLATFORM=aarch64-elf

build-mips:
	@$(MAKE) build_tools BUILD_PLATFORM=mipsel-elf

build-riscv:
	# GDB is currently not supported on RISC-V
	@$(MAKE) build_gcc BUILD_PLATFORM=riscv-elf

build-power8:
	@$(MAKE) build_tools BUILD_PLATFORM=powerpc64-linux-gnu

build-nds32le:
	@$(MAKE) build_tools BUILD_PLATFORM=nds32le-elf

clean_tempfiles:
	rm -rf build-*
	rm -rf binutils-* gcc-* gmp-* mpc-* mpfr-*
	rm -rf llvm-* clang-tools-* cfe-* compiler-rt-*
	rm -rf acpica-*
	rm -rf gdb-*
	rm -f  getopt

clean: clean_tempfiles
	rm -rf xgcc

distclean: clean
	rm -rf tarballs

.PHONY: build_gcc build_iasl build_gdb build_clang all all_with_gdb \
	all_without_gdb build_tools build-i386 build-x64 build-arm  \
	build-aarch64 build-mips build-riscv build-power8 build-nds32le \
	clean distclean clean_tempfiles
.NOTPARALLEL:
