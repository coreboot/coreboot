## SPDX-License-Identifier: GPL-2.0-only

unexport $(COREBOOT_EXPORTS)

DTC ?= dtc

LINUX_ARCH-$(CONFIG_LINUXBOOT_X86_64)     = x86_64
LINUX_ARCH-$(CONFIG_LINUXBOOT_X86)        = i386
LINUX_ARCH-$(CONFIG_LINUXBOOT_ARM64)      = arm64
LINUX_ARCH-$(CONFIG_LINUXBOOT_RISCV_RV32) = riscv
LINUX_ARCH-$(CONFIG_LINUXBOOT_RISCV_RV64) = riscv

ifeq ($(CONFIG_LINUXBOOT_COMPILE_KERNEL),y)
include targets/linux.mk
endif
ifeq ($(CONFIG_LINUXBOOT_UROOT),y)
include targets/u-root.mk
endif

ifeq ($(CONFIG_LINUXBOOT_KERNEL_BZIMAGE),y)

build/Image: $(kernel_dir)/arch/x86/boot/bzImage | build/
	cp $< $@

else ifeq ($(CONFIG_LINUXBOOT_KERNEL_UIMAGE),y)

build/target.dtb: $(CONFIG_LINUXBOOT_DTS_FILE) | build/
	$(DTC) -o $@ $<

build/Image: build/vmlinux.bin.lzma build/initramfs build/target.dtb $(LINUX_ARCH-y)/kernel_fdt_lzma.its | build/
	mkimage -f $(LINUX_ARCH-y)/kernel_fdt_lzma.its $@

else ifneq ($(CONFIG_LINUXBOOT_COMPILE_KERNEL),y)

build/Image: $(CONFIG_LINUXBOOT_KERNEL_PATH) | build/
	cp $< $@

endif

build/initramfs: build/initramfs_u-root.cpio | build/
ifeq ($(CONFIG_LINUXBOOT_INITRAMFS_COMPRESSION_XZ),y)
	xz --stdout --keep --force --check=crc32 --lzma2=dict=1MiB $< > $@
else
	cp $< $@
endif

build/:
	mkdir -p build

clean:
	rm -rf build/kernel*
	rm -f  build/u-root
	rm -f  build/initramfs*
	rm -f  build/bzImage
	rm -f  build/uImage

distclean:
	rm -rf build

.PHONY: linuxboot clean distclean
