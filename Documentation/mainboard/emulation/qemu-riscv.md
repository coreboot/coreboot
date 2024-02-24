# QEMU RISC-V emulator

## Building coreboot and running it in QEMU

- Configure coreboot and run `make` as usual

Run QEMU
```
qemu-system-riscv64 -M virt -m 1G -nographic -bios build/coreboot.rom \
                    -drive if=pflash,file=./build/coreboot.rom,format=raw
```
