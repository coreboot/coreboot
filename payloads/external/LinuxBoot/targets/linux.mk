## SPDX-License-Identifier: GPL-2.0-only

SHELL := /bin/bash

ARCH-$(CONFIG_LINUXBOOT_X86_64)=x86_64
ARCH-$(CONFIG_LINUXBOOT_X86)=x86
ARCH-$(CONFIG_LINUXBOOT_ARM64)=arm64

TAG-$(CONFIG_LINUXBOOT_KERNEL_MAINLINE)=mainline
TAG-$(CONFIG_LINUXBOOT_KERNEL_STABLE)=stable
TAG-$(CONFIG_LINUXBOOT_KERNEL_LONGTERM)=longterm

pwd:=$(shell pwd)
top:=../../..
project_dir=linuxboot
tarball_dir:=$(project_dir)/tarball
decompress_flag=.done

OBJCOPY:=$(LINUXBOOT_CROSS_COMPILE)objcopy
KERNEL_MAKE_FLAGS = \
	ARCH=$(ARCH-y) \
	KBUILD_BUILD_USER="coreboot" \
	KBUILD_BUILD_HOST="reproducible" \
	KBUILD_BUILD_TIMESTAMP="$(shell perl -e 'print scalar gmtime($(SOURCE_DATE_EPOCH))')" \
	KBUILD_BUILD_VERSION="0"

ifeq ($(CONFIG_LINUXBOOT_KERNEL_CUSTOM),y)
	kernel_version:=$(CONFIG_LINUXBOOT_KERNEL_CUSTOM_VERSION)
else
	kernel_version:=$(shell curl -sS -k https://www.kernel.org/feeds/kdist.xml | \
		sed -n -e 's@.*<guid isPermaLink="false">\(.*\)</guid>.*@\1@p' | \
		awk -F ',' '/$(TAG-y)/{ print $$3 }' | \
		head -n 1)
endif

kernel_dir=$(project_dir)/kernel-$(subst .,_,$(kernel_version))
kernel_tarball=linux-$(kernel_version).tar
kernel_mirror=https://mirrors.edge.kernel.org/pub/linux/kernel

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

all: kernel

lookup:
ifeq ($(kernel_version),)
	$(error kernel version lookup failed for $(TAG-y) release)
endif
	@echo "    WWW        Kernel [$(TAG-y)] $(kernel_version)"

fetch:
ifneq ($(shell [[ -d "$(kernel_dir)" && -f "$(kernel_dir)/$(decompress_flag)" ]];echo $$?),0)
	mkdir -p $(tarball_dir)
	if [[ ! -f $(tarball_dir)/$(kernel_tarball).xz && ! -f $(tarball_dir)/$(kernel_tarball).xz ]]; then \
	echo "    WWW        $(kernel_tarball).xz"; \
	cd $(tarball_dir); \
	curl -OLSs "$(kernel_mirror_path)/$(kernel_tarball).xz"; \
	cd $(pwd); \
	fi
endif

unpack: fetch
	if [[ -d "$(kernel_dir)" && ! -f "$(kernel_dir)/$(decompress_flag)" ]]; then \
	rm -rf $(kernel_dir); \
	fi
	if [[ ! -d "$(kernel_dir)" ]]; then \
	mkdir $(kernel_dir); \
	echo "    XZ         $(kernel_tarball).xz"; \
	tar xJf $(tarball_dir)/$(kernel_tarball).xz --strip 1 -C $(kernel_dir); \
	fi
	touch $(kernel_dir)/$(decompress_flag)

$(kernel_dir)/.config: unpack
	@echo "    CONFIG     Linux $(kernel_version)"
ifeq ($(CONFIG_LINUXBOOT_KERNEL_CUSTOM_CONFIG),y)
	cp $(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE) $(kernel_dir)/.config
else
	cp $(ARCH-y)/defconfig $(kernel_dir)/.config
endif
	$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) olddefconfig

build: $(kernel_dir)/.config
	@echo "    MAKE       Linux $(kernel_version)"
ifeq ($(CONFIG_LINUXBOOT_KERNEL_BZIMAGE),y)
	$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) CROSS_COMPILE=$(LINUXBOOT_CROSS_COMPILE) bzImage
else
ifeq ($(CONFIG_LINUXBOOT_KERNEL_UIMAGE),y)
	$(MAKE) -C $(kernel_dir) $(KERNEL_MAKE_FLAGS) CROSS_COMPILE=$(LINUXBOOT_CROSS_COMPILE) vmlinux
endif
endif

ifeq ($(CONFIG_LINUXBOOT_KERNEL_BZIMAGE),y)
$(top)/$(CONFIG_LINUXBOOT_KERNEL): build
	@echo "    CP         bzImage"
	cp $(kernel_dir)/arch/x86/boot/bzImage $@
else
ifeq ($(CONFIG_LINUXBOOT_KERNEL_UIMAGE),y)
$(project_dir)/target.dtb: $(top)/$(CONFIG_LINUXBOOT_DTB_FILE)
	cp $< $@
$(project_dir)/vmlinux.bin: $(kernel_dir)/vmlinux
	$(OBJCOPY) -O binary $< $@
$(project_dir)/vmlinux.bin.lzma: $(project_dir)/vmlinux.bin
	xz -c -k -f --format=lzma --lzma1=dict=1MiB,lc=3,lp=0,pb=3 $< > $@
$(top)/$(CONFIG_LINUXBOOT_KERNEL): build $(project_dir)/vmlinux.bin.lzma $(project_dir)/target.dtb
	cp $(project_dir)/../arm64/kernel_fdt_lzma.its $(project_dir)
	cp $(top)/$(CONFIG_LINUXBOOT_INITRAMFS)$(CONFIG_LINUXBOOT_INITRAMFS_SUFFIX) $(project_dir)/initramfs
	mkimage -f $(project_dir)/kernel_fdt_lzma.its $@
else
	$(error Kernel image format not found)
	exit 1
endif
endif

ifneq ($(TAG-y),)
kernel: lookup $(top)/$(CONFIG_LINUXBOOT_KERNEL)
else
kernel: $(top)/$(CONFIG_LINUXBOOT_KERNEL)
endif

.PHONY: all kernel build unpack fetch check
