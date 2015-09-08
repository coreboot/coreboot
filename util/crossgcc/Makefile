# if no architecture is specified, set a default
BUILD_PLATFORM ?= i386-elf

all:
	$(MAKE) BUILDGCC_OPTIONS=-t build-i386 build-x64 build-armv7a build-mips build-riscv build-aarch64 \
	        build_clang
	$(MAKE) clean_tempfiles

all_without_gdb:
	$(MAKE) BUILDGCC_OPTIONS=-t build-i386-without-gdb build-x64-without-gdb build-armv7a-without-gdb \
	        build-mips-without-gdb build-riscv-without-gdb build-aarch64-without-gdb build_clang
	$(MAKE) clean_tempfiles

build_tools: build_gcc build_iasl build_gdb

build_tools_without_gdb: build_gcc build_iasl

###########################################################
### targets to do buildgcc builds

build_gcc:
	bash ./buildgcc -p $(BUILD_PLATFORM) $(if $(BUILDJOBS),-j $(BUILDJOBS)) $(BUILDGCC_OPTIONS) \
			$(if $(BUILD_LANGUAGES),-l $(BUILD_LANGUAGES))

build_gdb:
	bash ./buildgcc -p $(BUILD_PLATFORM) -P gdb $(if $(BUILDJOBS),-j $(BUILDJOBS)) $(BUILDGCC_OPTIONS)

build_iasl:
	bash ./buildgcc -P iasl $(if $(BUILDJOBS),-j $(BUILDJOBS)) $(BUILDGCC_OPTIONS)

build_clang:
	bash ./buildgcc -P clang $(if $(BUILDJOBS),-j $(BUILDJOBS)) $(BUILDGCC_OPTIONS)

###########################################################
### Build with GDB ###
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

###########################################################
### Build without GDB
build-i386-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=i386-elf

build-x64-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=x86_64-elf

build-armv7a-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=armv7a-eabi

build-aarch64-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=aarch64-elf

build-mips-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=mipsel-elf

build-riscv-without-gdb:
	@$(MAKE) build_tools_without_gdb BUILD_PLATFORM=riscv-elf

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

.PHONY: build_gcc build_iasl build_gdb build_tools build_tools_without_gdb \
	build-i386-without-gdb build-x64-without-gdb build-armv7a-without-gdb \
	build-aarch64-without-gdb build-mips-without-gdb build-riscv-without-gdb \
	all build clean distclean clean_tempfiles all_without_gdb
