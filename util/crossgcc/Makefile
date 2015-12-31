# if no architecture is specified, set a default
BUILD_PLATFORM ?= i386-elf

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use KEEP_SOURCES=1 to keep temporary files after the build
# use BUILDGCC_OPTIONS= to set any other crossgcc command line options
# Example: BUILDGCC_OPTIONS=-c to remove temporary files before build

all all_with_gdb:
	$(MAKE) build-i386
	$(MAKE) SKIP_IASL=1 \
		 build-x64 build-armv7a build-mips build-riscv build-aarch64 \
		 build_clang

all_without_gdb:
	$(MAKE) SKIP_GDB=1 build-i386
	$(MAKE) SKIP_IASL=1 SKIP_GDB=1 \
		 build-x64 build-armv7a build-mips build-riscv build-aarch64 \
		 build_clang

build_tools: build_gcc build_iasl build_gdb

###########################################################
### targets to do buildgcc builds

build_gcc:
	bash ./buildgcc -p $(BUILD_PLATFORM) $(if $(CPUS),-j $(CPUS)) $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS) \
			$(if $(BUILD_LANGUAGES),-l $(BUILD_LANGUAGES))

build_gdb:
ifeq ($(SKIP_GDB),)
	bash ./buildgcc -p $(BUILD_PLATFORM) -P gdb $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS)
endif

build_iasl:
ifeq ($(SKIP_IASL),)
	bash ./buildgcc -P iasl $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS)
endif

build_clang:
ifeq ($(SKIP_CLANG),)
	bash ./buildgcc -P clang $(if $(CPUS),-j $(CPUS))  $(if $(KEEP_SOURCES),-t) $(BUILDGCC_OPTIONS)
endif

###########################################################
build-i386:
	@$(MAKE) build_tools BUILD_PLATFORM=i386-elf

build-x64:
	@$(MAKE) build_tools BUILD_PLATFORM=x86_64-elf

build-armv7a:
	@$(MAKE) build_tools BUILD_PLATFORM=armv7a-eabi

build-aarch64:
	@$(MAKE) build_tools BUILD_PLATFORM=aarch64-elf

build-mips:
	@$(MAKE) build_tools BUILD_PLATFORM=mipsel-elf

build-riscv:
	@$(MAKE) build_tools BUILD_PLATFORM=riscv-elf

build-power8:
	@$(MAKE) build_tools BUILD_PLATFORM=powerpc64le-linux-gnu

clean_tempfiles:
	rm -rf build-*
	rm -rf binutils-* gcc-* gmp-* libelf-* mpc-* mpfr-*
	rm -rf llvm-* clang-tools-* cfe-* compiler-rt-*
	rm -rf acpica-*
	rm -rf gdb-*
	rm -f  getopt

clean: clean_tempfiles
	rm -rf xgcc

distclean: clean
	rm -rf tarballs

.PHONY: build_gcc build_iasl build_gdb build_clang \
	all all_with_gdb all_without_gdb build_tools \
	build-i386 build-x64 build-armv7a build-aarch64 build-mips build-riscv build-power8 \
	clean distclean clean_tempfiles
