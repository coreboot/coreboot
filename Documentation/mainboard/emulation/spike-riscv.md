# Spike RISC-V emulator

[Spike], also known as riscv-isa-sim, is a commonly used [RISC-V] emulator.


## Installation

- Download `riscv-fesvr` and `riscv-isa-sim` from <https://github.com/riscv/>
- Apply the two patches in <https://github.com/riscv/riscv-isa-sim/pull/53>,
  which are necessary in order to have a serial console
- Compile `riscv-fesvr` and then `riscv-isa-sim`


## Building coreboot and running it in Spike

- Configure coreboot and run `make` as usual
- Run `util/riscv/make-spike-elf.sh build/coreboot.rom build/coreboot.elf` to
  convert coreboot to an ELF that Spike can load
- Run `spike -m1024 build/coreboot.elf`


[Spike]: https://github.com/riscv/riscv-isa-sim
[RISC-V]: https://riscv.org/
