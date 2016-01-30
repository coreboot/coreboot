##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

help_toolchain help::
	@echo  '*** Toolchain targets ***'
	@echo  '  crossgcc        - Build coreboot cross-compilers for all platforms'
	@echo  '  crosstools      - Build coreboot cross-compiler and GDB for all platforms'
	@echo  '  crossgcc-clean  - Remove all built coreboot cross-compilers'
	@echo  '  iasl            - Build coreboot IASL compiler (built by all cross targets)'
	@echo  '  clang           - Build coreboot clang compiler'
	@echo  '  test-toolchain  - Reports if toolchain components are out of date'
	@echo  '  crossgcc-ARCH   - Build cross-compiler for specific architecture'
	@echo  '  crosstools-ARCH - Build cross-compiler with GDB for specific architecture'
	@echo  '  ARCH can be "i386", "x64", "arm", "aarch64", "mips", "riscv", or "power8"'
	@echo  '  Use "make [target] CPUS=#" to build toolchain using multiple cores'
	@echo

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use BUILDGCC_OPTIONS= to set any crossgcc command line options
# Example: BUILDGCC_OPTIONS='-t' will keep temporary files after build
crossgcc: clean-for-update
	$(MAKE) -C util/crossgcc all_without_gdb SKIP_CLANG=1

.PHONY: crossgcc crossgcc-i386 crossgcc-x64 crossgcc-arm crossgcc-aarch64 \
	crossgcc-mips crossgcc-riscv crossgcc-power8 crossgcc-clean iasl \
	clang crosstools-i386 crosstools-x64 crosstools-arm \
	crosstools-aarch64 crosstools-mips crosstools-riscv crosstools-power8

crossgcc-i386: clean-for-update
	$(MAKE) -C util/crossgcc build-i386 SKIP_GDB=1

crossgcc-x64: clean-for-update
	$(MAKE) -C util/crossgcc build-x64 SKIP_GDB=1

crossgcc-arm: clean-for-update
	$(MAKE) -C util/crossgcc build-arm SKIP_GDB=1

crossgcc-aarch64: clean-for-update
	$(MAKE) -C util/crossgcc build-aarch64 SKIP_GDB=1

crossgcc-mips: clean-for-update
	$(MAKE) -C util/crossgcc build-mips SKIP_GDB=1

crossgcc-riscv: clean-for-update
	$(MAKE) -C util/crossgcc build-riscv SKIP_GDB=1

crossgcc-power8: clean-for-update
	$(MAKE) -C util/crossgcc build-power8 SKIP_GDB=1

crosstools: clean-for-update
	$(MAKE) -C util/crossgcc all_with_gdb SKIP_CLANG=1

iasl: clean-for-update
	$(MAKE) -C util/crossgcc build_iasl

clang: clean-for-update
	$(MAKE) -C util/crossgcc build_clang

crosstools-i386: clean-for-update
	$(MAKE) -C util/crossgcc build-i386

crosstools-x64: clean-for-update
	$(MAKE) -C util/crossgcc build-x64

crosstools-arm: clean-for-update
	$(MAKE) -C util/crossgcc build-arm

crosstools-aarch64: clean-for-update
	$(MAKE) -C util/crossgcc build-aarch64

crosstools-mips: clean-for-update
	$(MAKE) -C util/crossgcc build-mips

crosstools-riscv: clean-for-update
	$(MAKE) -C util/crossgcc build-riscv

crosstools-power8: clean-for-update
	$(MAKE) -C util/crossgcc build-power8

crossgcc-clean: clean-for-update
	$(MAKE) -C util/crossgcc clean

test-toolchain:
ifeq ($(COMPILER_OUT_OF_DATE),1)
	echo "The coreboot toolchain is not the current version."
	$(error )
else
	echo "The coreboot toolchain is the current version."
endif # ifeq ($(COMPILER_OUT_OF_DATE),1)
