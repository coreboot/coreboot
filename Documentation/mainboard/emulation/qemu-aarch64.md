# QEMU AArch64 emulator
This page describes how to build and run coreboot for QEMU/AArch64.
You can use LinuxBoot via `make menuconfig` or an arbitrary FIT image
as a payload for QEMU/AArch64.

## Running coreboot in QEMU
```bash
qemu-system-aarch64 -bios ./build/coreboot.rom \
    -M virt,secure=on,virtualization=on -cpu cortex-a53 \
    -nographic -m 8192M
```

- The default CPU in QEMU for AArch64 is a cortex-a15 which is 32-bit
ARM CPU. You need to specify 64-bit ARM CPU via `-cpu cortex-a53`.
- The default privilege level in QEMU for AArch64 is EL1 that we can't
have the right to access EL3/EL2 registers. You need to enable EL3/EL2
via `-machine secure=on,virtualization=on`.
- You need to specify the size of memory more than 544 MiB because 512
MiB is reserved for the kernel.
- The maximum size of memory is 255GiB (-m 261120).

## Building coreboot with an arbitrary FIT payload
There are 3 steps to make coreboot.rom for QEMU/AArch64. If you select
LinuxBoot, step 2 and 3 have done by LinuxBoot.
1. Get a DTB (Device Tree Blob)
2. Build a FIT image with a DTB
3. Add a FIT image to coreboot.rom

### 1. Get a DTB
You can get the DTB from QEMU with the following command.
```
$ qemu-system-aarch64 \
    -M virt,dumpdtb=virt.dtb,secure=on,virtualization=on \
    -cpu cortex-a53 -nographic -m 8192M
```

### 2. Build a FIT image with a DTB
You need to write an image source file that has an `.its` extension to
configure kernels, ramdisks, and DTBs.
See [Flattened uImage Tree documentation](../../lib/payloads/fit.md) for more details.

### 3. Add a FIT image to coreboot.rom
You can use cbfstool to add the payload you created in step 2 to
the coreboot.rom.
```
$ ./build/cbfstool ./build/coreboot.rom add -f <path-to-a-payload>/uImage \
    -n fallback/payload -t fit -c lzma
```
