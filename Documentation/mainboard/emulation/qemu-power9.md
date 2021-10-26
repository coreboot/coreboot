# QEMU PPC64 emulator
This page describes how to build and run coreboot for QEMU/PPC64.

## Building coreboot
```bash
make defconfig KBUILD_DEFCONFIG=configs/config.emulation_qemu_power9
make
```

This builds coreboot with no payload.

## Payloads
You can configure ELF or `skiboot` payload via `make menuconfig`. In either case
you might need to adjust "ROM chip size" and make it large enough to accommodate
the payload (see how much space it needs in the error you get if it doesn't
fit).

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
- By default Hostboot mode is off and needs to be turned on to run coreboot
as a firmware rather than like an OS.
- `-bios` specifies initial program (bootloader should suffice, but whole image
works fine too).
- `-drive` specifies image for emulated flash device.

## Running with a kernel
Loading `skiboot` (built automatically by coreboot or otherwise) allows
specifying kernel and root file system to be run.

```bash
qemu-system-ppc64 -M powernv,hb-mode=on \
                  -cpu power9 \
                  -bios build/coreboot.rom \
                  -drive file=build/coreboot.rom,if=mtd \
                  -serial stdio \
                  -display none \
                  -kernel zImage \
                  -initrd initrd.cpio.xz
```

- Specify path to your kernel via `-kernel`.
- Specify path to your rootfs via `-initrd`.
