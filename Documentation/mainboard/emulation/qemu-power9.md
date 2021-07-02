# QEMU PPC64 emulator
This page describes how to build and run coreboot for QEMU/PPC64.
You can configure ELF payload via `make menuconfig` (make sure "ROM chip size"
is large enough).

## Running coreboot in QEMU
```bash
qemu-system-ppc64 -M powernv,hb-mode=on \
                  -cpu power9 \
                  -bios build/coreboot.rom \
                  -drive file=build/coreboot.rom,if=mtd \
                  -serial stdio \
                  -display none
```

- The default CPU in QEMU for AArch64 is a 604. You specify a suitable
PowerPC CPU via `-cpu power9`.
- By default Hostboot mode is off and it needs to be turned on to run coreboot
as a firmware rather than like an OS.
- `-bios` specifies initial program (bootloader should suffice, but whole image
works fine too).
- `-drive` specifies image for emulated flash device.

## Building coreboot
```bash
make defconfig KBUILD_DEFCONFIG=configs/config.emulation_qemu_power9
make
```

This builds coreboot with no payload.
