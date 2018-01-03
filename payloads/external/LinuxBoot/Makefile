## This file is part of the coreboot project.
##
## Copyright (C) 2017 Facebook Inc.
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

kernel_tarball=https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-$(CONFIG_LINUXBOOT_KERNEL_VERSION).tar.xz
project_dir=linuxboot
kernel_dir=$(project_dir)/kernel

unexport $(COREBOOT_EXPORTS)

all: payload

$(kernel_dir)/.config:
	echo "    WWW        Download Linux $(CONFIG_LINUXBOOT_KERNEL_VERSION)"
	mkdir -p $(kernel_dir)
ifeq ("$(wildcard $(kernel_dir)/README)","")
	wget -qO- $(kernel_tarball) | tar xJ -C $(kernel_dir) --strip 1
endif

config: $(kernel_dir)/.config
	echo "    CONFIG     Linux $(CONFIG_LINUXBOOT_KERNEL_VERSION)"
ifneq ($(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE),)
	cp $(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE) $(kernel_dir)/.config
endif
ifeq ($(CONFIG_LINUXBOOT_ARCH),386)
	cp x86/defconfig $(kernel_dir)/.config
else ifeq ($(CONFIG_LINUXBOOT_ARCH),amd64)
	cp x86_64/defconfig $(kernel_dir)/.config
endif

$(project_dir)/kernel-image: config
	echo "    MAKE       Kernel $(CONFIG_LINUXBOOT_KERNEL_VERSION)"
	$(MAKE) -C $(kernel_dir) olddefconfig
	$(MAKE) -C $(kernel_dir) -j $(CPUS)
ifeq ($(CONFIG_LINUXBOOT_ARCH),386)
	cp $(kernel_dir)/arch/x86/boot/bzImage $(project_dir)/kernel-image
else ifeq ($(CONFIG_LINUXBOOT_ARCH),amd64)
	cp $(kernel_dir)/arch/x86/boot/bzImage $(project_dir)/kernel-image
endif

payload: $(project_dir)/kernel-image
ifeq ($(CONFIG_LINUXBOOT_UROOT),y)
	$(MAKE) -f targets/u-root.mk
endif

clean:
	if [ -d "$(kernel_dir)" ]; then make -C $(kernel_dir) clean; fi
	rm -f $(project_dir)/initramfs.cpio.xz

distclean:
	rm -rf $(project_dir)

.PHONY: config patch payload clean distclean clone fetch all
