# Blobs used in Intel Broadwell boards

All Broadwell boards supported by coreboot require two proprietary blobs.
In a coreboot image of a Broadwell board, the blobs are named `mrc.bin` and
`refcode` in CBFS.

`mrc.bin` is run in romstage to initialize the memory. It is placed at a fixed
address in CBFS and is loaded at a fixed address in memory.

`refcode` is run in ramstage to initialize the system agent and the PCH. It is
a relocatable ELF object.

## Obtaining the blobs

Both `mrc.bin` and `refcode` can be obtained from a coreboot image of a Broadwell
board, for example a Purism Librem 13 v1 coreboot image from [MrChromebox].

    cbfstool coreboot_*.rom extract -f broadwell-mrc.bin -n mrc.bin
    cbfstool coreboot_*.rom extract -m x86 -f broadwell-refcode.elf -n fallback/refcode

## SPD Addresses

The SPD addresses in Broadwell `pei_data` struct are similar to [Haswell].

## Intel GbE support

Unlike Haswell boards, the `pei_data` struct of Broadwell doesn't have `gbe_enable`
field. For boards with an Intel GbE device, a modification of `refcode` is needed,
otherwise `refcode` will disable the Intel GbE device and the OS cannot find it
in the list of PCI devices.

## Use Broadwell SoC code for Haswell ULT boards

Haswell ULT boards can use Broadwell SoC code. To use Broadwell code for Haswell ULT
boards, `devicetree.cb` file and `pei_data` code need to be ported to Broadwell, and
build the code with Broadwell `mrc.bin` and `refcode` instead of using Haswell `mrc.bin`.

Broadwell SoC code doesn't support non-ULT Haswell or non-ULT Broadwell boards.

[MrChromebox]: https://mrchromebox.tech/
[Haswell]: ../../../northbridge/intel/haswell/mrc.bin.md
