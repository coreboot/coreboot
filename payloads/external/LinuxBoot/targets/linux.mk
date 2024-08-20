## SPDX-License-Identifier: GPL-2.0-only

SHELL := /bin/sh

OBJCOPY:=$(CONFIG_LINUXBOOT_CROSS_COMPILE_PATH)objcopy
KERNEL_MAKE_FLAGS = \
	CROSS_COMPILE=$(CONFIG_LINUXBOOT_CROSS_COMPILE_PATH) \
	ARCH=$(LINUX_ARCH-y) \
	KBUILD_BUILD_USER="coreboot" \
	KBUILD_BUILD_HOST="reproducible" \
	KBUILD_BUILD_TIMESTAMP="$(shell perl -e 'print scalar gmtime($(SOURCE_DATE_EPOCH))')" \
	KBUILD_BUILD_VERSION="0"

kernel_version = $(CONFIG_LINUXBOOT_KERNEL_VERSION)
kernel_dir = build/kernel-$(subst .,_,$(kernel_version))
kernel_tarball = linux-$(kernel_version).tar
kernel_mirror  = https://mirrors.edge.kernel.org/pub/linux/kernel

ifeq ($(findstring x2.6.,x$(kernel_version)),x2.6.)
kernel_mirror_path := $(kernel_mirror)/v2.6
else ifeq ($(findstring x3.,x$(kernel_version)),x3.)
kernel_mirror_path := $(kernel_mirror)/v3.x
else ifeq ($(findstring x4.,x$(kernel_version)),x4.)
kernel_mirror_path := $(kernel_mirror)/v4.x
else ifeq ($(findstring x5.,x$(kernel_version)),x5.)
kernel_mirror_path := $(kernel_mirror)/v5.x
else ifeq ($(findstring x6.,x$(kernel_version)),x6.)
kernel_mirror_path := $(kernel_mirror)/v6.x
endif

build/$(kernel_tarball).xz: | build
	echo "    Test $(kernel_version)"
	echo "    WWW        $(kernel_mirror_path)/$(kernel_tarball).xz";
	curl -OLSs --output-dir build "$(kernel_mirror_path)/$(kernel_tarball).xz";

$(kernel_dir): build/$(kernel_tarball).xz
	echo "    XZ         $(kernel_tarball).xz";
	mkdir $(kernel_dir);
	tar xJf build/$(kernel_tarball).xz --strip 1 -C $(kernel_dir);

$(kernel_dir)/.config: $(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE) | $(kernel_dir)
	@echo "    CONFIG     Linux $(kernel_version)"
	cp $(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE) $(kernel_dir)/.config
	$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) olddefconfig

$(kernel_dir)/vmlinux : $(kernel_dir)/.config | $(kernel_dir)
	@echo "    MAKE       Linux $(kernel_version)"
	echo "$(MAKE) -j $(CPUS) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) vmlinux"
	$(MAKE) -j $(CPUS) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) vmlinux

build/vmlinux.bin: $(kernel_dir)/vmlinux | build
	$(OBJCOPY) -O binary $< $@

build/vmlinux.bin.lzma: build/vmlinux.bin
	xz -c -k -f --format=lzma --lzma1=dict=1MiB,lc=3,lp=0,pb=3 $< > $@

$(kernel_dir)/arch/x86/boot/bzImage: $(kernel_dir)/.config
	@echo "    MAKE       Linux $(kernel_version)"
	echo "$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) bzImage"
	$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) bzImage

.PHONY: kernel
