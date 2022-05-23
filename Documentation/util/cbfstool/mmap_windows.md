# cbfstool: Handling memory mapped boot media

`cbfstool` is a utility used for managing coreboot file system (CBFS)
components in a ROM image. x86 platforms are special since they have
the SPI flash boot media memory mapped into host address space at
runtime. This requires `cbfstool` to deal with two separate address
spaces for any CBFS components that are eXecute-In-Place (XIP) - one
is the SPI flash address space and other is the host address space
where the SPI flash gets mapped.

By default, all x86 platforms map a maximum of 16MiB of SPI flash at
the top of 4G in host address space. If the flash is greater than
16MiB, then only the top 16MiB of the flash is mapped in the host
address space. If the flash is smaller than 16MiB, then the entire SPI
flash is mapped at the top of 4G and the rest of the space remains
unused.

In more recent platforms like Tiger Lake (TGL), it is possible to map
more than 16MiB of SPI flash. Since the host address space has legacy
fixed device addresses mapped below `4G - 16M`, the SPI flash is split
into separate windows when being mapped to the host address space.
Default decode window of maximum 16MiB size still lives just below the
4G boundary. The additional decode window is free to live in any
available MMIO space that the SoC chooses.

Following diagram shows different combinations of SPI flash being
mapped into host address space when using multiple windows:

![MMAP window combinations with different flash sizes][mmap_windows]

*(a) SPI flash of size 16MiB (b) SPI flash smaller than 16MiB (c) SPI flash
of size (16MiB+ext window size) (d) SPI flash smaller than (16MiB+ext
window size)*

The location of standard decode window is fixed in host address space
`(4G - 16M) to 4G`. However, the platform is free to choose where the
extended window lives in the host address space. Since `cbfstool`
needs to know the exact location of the extended window, it allows the
platform to pass in two parameters `ext-win-base` and `ext-win-size`
that provide the base and the size of the extended window in host
address space.

`cbfstool` creates two memory map windows using the knowledge about the
standard decode window and the information passed in by the platform
about the extended decode window. These windows are useful in
converting addresses from one space to another (flash space and host
space) when dealing with XIP components.

## Assumptions

1. Top 16MiB is still decoded in the fixed decode window just below 4G
   boundary.
1. Rest of the SPI flash below the top 16MiB is mapped at the top of
   the extended window. Even though the platform might support a
   larger extended window, the SPI flash part used by the mainboard
   might not be large enough to be mapped in the entire window. In
   such cases, the mapping is assumed to be in the top part of the
   extended window with the bottom part remaining unused.

## Example

If the platform supports extended window and the SPI flash size is
greater, then `cbfstool` creates a mapping for the extended window as
well.

```
ext_win_base = 0xF8000000
ext_win_size = 32 * MiB
ext_win_limit = ext_win_base + ext_win_size - 1 = 0xF9FFFFFF
```

If SPI flash is 32MiB, then top 16MiB is mapped from `0xFF000000 -
0xFFFFFFFF` whereas the bottom 16MiB is mapped from `0xF9000000 -
0xF9FFFFFF`. The extended window `0xF8000000 - 0xF8FFFFFF` remains
unused.

[mmap_windows]: mmap_windows.svg
