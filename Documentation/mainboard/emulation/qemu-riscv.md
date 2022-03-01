# QEMU RISC-V emulator

## Building coreboot and running it in QEMU

- Configure coreboot and run `make` as usual
- Run `util/riscv/make-spike-elf.sh build/coreboot.rom build/coreboot.elf` to
  convert coreboot to an ELF that QEMU can load
- Run `qemu-system-riscv64 -M virt -m 1024M -nographic -kernel build/coreboot.elf`
