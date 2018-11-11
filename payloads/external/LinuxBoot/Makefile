## This file is part of the coreboot project.
##
## Copyright (C) 2017 Facebook Inc.
## Copyright (C) 2018 9elements Cyber Security
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

project_dir=linuxboot
kernel_dir=$(project_dir)/kernel

unexport $(COREBOOT_EXPORTS)
unexport MAKEFLAGS

all: linuxboot

u-root:
	$(MAKE) -f targets/u-root.mk

ifeq ($(CONFIG_LINUXBOOT_BUILD_INITRAMFS),y)
ifeq ($(CONFIG_LINUXBOOT_UROOT),y)
initramfs: u-root
endif
else
ifneq ($(CONFIG_LINUXBOOT_INITRAMFS),)
initramfs:
	if [[ ! -f "$(top)/$(CONFIG_LINUXBOOT_INITRAMFS)" ]]; then \
	echo "<< Linux initramfs '$(CONFIG_LINUXBOOT_INITRAMFS)' is missing. >>"; \
	exit 1; \
	fi
endif
endif

ifeq ($(CONFIG_LINUXBOOT_COMPILE_KERNEL),y)
ifeq ($(CONFIG_LINUXBOOT_ARCH),arm64)
kernel: initramfs
	$(MAKE) -f targets/linux.mk
else
kernel:
	$(MAKE) -f targets/linux.mk
endif
else
kernel:
	if [[ ! -f "$(top)/$(CONFIG_LINUXBOOT_KERNEL)" ]]; then \
	echo "<< Linux kernel '$(CONFIG_LINUXBOOT_KERNEL)' is missing. >>"; \
	exit 1; \
	fi
endif

linuxboot: kernel initramfs

clean:
	if [ -d "$(kernel_dir)" ]; then rm -rf $(kernel_dir); fi
	rm -f $(project_dir)/initramfs.cpio.xz

distclean:
	rm -rf $(project_dir)

.PHONY: linuxboot kernel initramfs u-root clean distclean
