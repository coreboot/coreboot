## SPDX-License-Identifier: GPL-2.0-only

project_dir=linuxboot
kernel_names=$(shell ls $(project_dir) | grep kernel)

unexport $(COREBOOT_EXPORTS)

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

initramfs_compressed: initramfs
ifeq ($(CONFIG_LINUXBOOT_INITRAMFS_COMPRESSION_XZ),y)
	xz --keep --force --check=crc32 --lzma2=dict=1MiB $(top)/$(CONFIG_LINUXBOOT_INITRAMFS)
endif

ifeq ($(CONFIG_LINUXBOOT_COMPILE_KERNEL),y)
ifeq ($(CONFIG_LINUXBOOT_ARCH),arm64)
kernel: initramfs
	if [[ ! -f "$(top)/$(CONFIG_LINUXBOOT_DTB_FILE)" ]]; then \
	echo "<< Linux kernel devicetree is missing. >>"; \
	exit 1; \
	fi
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

linuxboot: kernel initramfs_compressed

clean:
	if [ ! -z "$(kernel_names)" ]; then \
	       	rm -rf $(project_dir)/kernel*; \
	fi
	rm -f $(project_dir)/u-root
	rm -f $(project_dir)/initramfs*
	rm -f $(project_dir)/bzImage

distclean:
	rm -rf $(project_dir)

.PHONY: linuxboot kernel initramfs_compressed initramfs u-root clean distclean
