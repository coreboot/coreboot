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

For the refcode binary extracted from Purism Librem 13 v1 (SHA256:
8a919ffece61ba21664b1028b0ebbfabcd727d90c1ae2f72b48152b8774323a4,
.program section starts at file offset 0x2040), we can see the
following code sequence:

    1e06b:       c6 43 0c 00             movb   $0x0,0xc(%ebx)
    1e06f:       c6 83 7e 03 00 00 00    movb   $0x0,0x37e(%ebx)
    1e076:       c6 83 70 03 00 00 01    movb   $0x1,0x370(%ebx)
    1e07d:       66 89 43 0a             mov    %ax,0xa(%ebx)
    1e081:       c6 83 da 01 00 00 01    movb   $0x1,0x1da(%ebx)
    1e088:       c6 83 86 03 00 00 01    movb   $0x1,0x386(%ebx)

The code at 0x1e06f sets the field that is to enable the GbE to the
hardcoded 0 value. Change the byte at 0x1e075 (file offset 0x200b5)
to 0x01 to make the refcode support Intel GbE:

    cp refcode.elf refcode_gbe.elf
    printf '\x01' | dd of=refcode_gbe.elf bs=1 seek=131253 count=1 conv=notrunc

## Use Broadwell SoC code for Haswell ULT boards

Haswell ULT boards can use Broadwell SoC code. To use Broadwell code for Haswell ULT
boards, `devicetree.cb` file and `pei_data` code need to be ported to Broadwell, and
build the code with Broadwell `mrc.bin` and `refcode` instead of using Haswell `mrc.bin`.

Broadwell SoC code doesn't support non-ULT Haswell or non-ULT Broadwell boards.

[MrChromebox]: https://mrchromebox.tech/
[Haswell]: ../../../northbridge/intel/haswell/mrc.bin.md
