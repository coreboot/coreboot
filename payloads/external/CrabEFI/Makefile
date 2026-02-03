## SPDX-License-Identifier: GPL-2.0-only

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES
unexport $(COREBOOT_EXPORTS)

CRABEFI_DIR := $(dir $(lastword $(MAKEFILE_LIST)))CrabEFI
CRABEFI_REPO ?= https://github.com/ArthurHeymans/CrabEFI
CRABEFI_TAG_OR_REV ?= origin/master
CRABEFI_CARGO_TARGET := x86_64-unknown-none
CRABEFI_CARGO_PACKAGE := crabefi-coreboot

ifeq ($(CONFIG_CRABEFI_DEBUG),y)
CRABEFI_BUILD_TYPE := debug
CRABEFI_CARGO_FLAGS :=
else
CRABEFI_BUILD_TYPE := release
CRABEFI_CARGO_FLAGS := --release
endif

ifeq ($(CONFIG_CRABEFI_UI),y)
CRABEFI_CARGO_FLAGS += --features ui
endif

CRABEFI_ELF := $(CRABEFI_DIR)/target/$(CRABEFI_CARGO_TARGET)/$(CRABEFI_BUILD_TYPE)/crabefi

build: $(CRABEFI_ELF)

$(CRABEFI_DIR):
	git clone "$(CRABEFI_REPO)" "$(CRABEFI_DIR)"
ifneq ($(CRABEFI_TAG_OR_REV),)
	git -C "$(CRABEFI_DIR)" checkout $(CRABEFI_TAG_OR_REV)
endif

$(CRABEFI_ELF): $(CRABEFI_DIR)
	cd "$(CRABEFI_DIR)" && cargo build -p $(CRABEFI_CARGO_PACKAGE) --target $(CRABEFI_CARGO_TARGET) $(CRABEFI_CARGO_FLAGS)

clean:
	test -d "$(CRABEFI_DIR)" && cd "$(CRABEFI_DIR)" && cargo clean || true

distclean:
	rm -rf "$(CRABEFI_DIR)"

.PHONY: build clean distclean
