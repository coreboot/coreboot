# ASUS A88XM-E

This page describes how to run coreboot on the [ASUS A88XM-E].

## Technology

Both "Trinity" and "Richland" FM2 desktop processing units are working,
the CPU architecture in these CPUs/APUs are [Piledriver],
and their GPU is [TeraScale 3] (VLIW4-based).

Kaveri is non-working at the moment (FM2+),
the CPU architecture in these CPUs/APUs are [Steamroller],
and their GPU is [Sea Islands] (GCN2-based).

A10 Richland is recommended for the best performance and working IOMMU.

```{eval-rst}
+------------------+--------------------------------------------------+
| A88XM-E          |                                                  |
+------------------+--------------------------------------------------+
| DDR voltage IC   | Nuvoton 3101S                                    |
+------------------+--------------------------------------------------+
| Network          | Realtek RTL8111G                                 |
+------------------+--------------------------------------------------+
| Northbridge      | Integrated into CPU with IMC and GPU (APUs only) |
+------------------+--------------------------------------------------+
| Southbridge      | Bolton-D4                                        |
+------------------+--------------------------------------------------+
| Sound IC         | Realtek ALC887                                   |
+------------------+--------------------------------------------------+
| Super I/O        | ITE IT8603E                                      |
+------------------+--------------------------------------------------+
| VRM controller   | DIGI VRM ASP1206                                 |
+------------------+--------------------------------------------------+
```

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | yes        |
+---------------------+------------+
| Model               | [GD25Q64]  |
+---------------------+------------+
| Size                | 8 MiB      |
+---------------------+------------+
| Package             | DIP-8      |
+---------------------+------------+
| Write protection    | yes        |
+---------------------+------------+
| Dual BIOS feature   | no         |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

### Internal programming

The main SPI flash can be accessed using [flashrom], if the
AmdSpiRomProtect modules have been deleted in the factory image previously.

### External flashing

Using a PLCC Extractor or any other appropriate tool, carefully remove the
DIP-8 BIOS chip from its' socket while avoiding the bent pins, if possible.
To flash it, use a [flashrom]-supported USB CH341A programmer - preferably with a
green PCB - and double check that it's giving a 3.3V voltage on the socket pins.

## Integrated graphics

### Retrieve the VGA optionrom ("Retrieval via Linux kernel" method)

Make sure a proprietary UEFI is flashed and boot Linux with iomem=relaxed flag.
Some Linux drivers (e.g. radeon for AMD) make option ROMs like the video blob
available to user space via sysfs. To use that to get the blob you need to
enable it first. To that end you need to determine the path within /sys
corresponding to your graphics chip. It looks like this:

    # /sys/devices/pci<domain>:<bus>/<domain>:<bus>:<slot>.<function>/rom.

You can get the respective information with lspci, for example:

    # lspci -tv
    # -[0000:00]-+-00.0  Advanced Micro Devices, Inc. [AMD] Family 16h Processor Root Complex
    #            +-01.0  Advanced Micro Devices, Inc. [AMD/ATI] Kabini [Radeon HD 8210]
    # ...

Here the the needed bits (for the ROM of the Kabini device) are:

    # PCI domain: (almost always) 0000
    # PCI bus: (also very commonly) 00
    # PCI slot: 01 (logical slot; different from any physical slots)
    # PCI function: 0 (a PCI device might have multiple functions... shouldn't matter here)

To enable reading of the ROM you need to write 1 to the respective file, e.g.:

    # echo 1 > /sys/devices/pci0000:00/0000:00:01.0/rom

The same file should then contain the video blob and it should be possible to simply copy it, e.g.:

    # cp /sys/devices/pci0000:00/0000:00:01.0/rom vgabios.bin

romheaders should print reasonable output for this file.

This version is usable for all the GPUs.
    1002,9901 Trinity (Radeon HD 7660D)
    1002,9904 Trinity (Radeon HD 7560D)
    1002,990c Richland (Radeon HD 8670D)
    1002,990e Richland (Radeon HD 8570D)
    1002,9991 Trinity (Radeon HD 7540D)
    1002,9993 Trinity (Radeon HD 7480D)
    1002,9996 Richland (Radeon HD 8470D)
    1002,9998 Richland (Radeon HD 8370D)
    1002,999d Richland (Radeon HD 8550D)
    1002,130f Kaveri (Radeon R7)

## Known issues

- AHCI hot-plug
- S3 resume (sometimes)
- Windows 7 can't boot because of the incomplete ACPI implementation
- XHCI

### XHCI ports can break after using any of the blobs, restarting the
board with factory image makes it work again as fallback.
Tested even with/without the Bolton and Hudson blobs.

## Untested

- audio over HDMI

## TODOs

- one ATOMBIOS module for all the integrated GPUs
- manage to work with Kaveri/Godavary (they are using a binaryPI)
- IRQ routing is done incorrect way - common problem of fam15h boards

## Working

- ACPI
- CPU frequency scaling
- flashrom under coreboot
- Gigabit Ethernet
- Hardware monitoring
- Integrated graphics
- KVM virtualization
- Onboard audio
- PCI
- PCIe
- PS/2 keyboard mouse (during payload, bootloader)
- SATA
- Serial port
- SuperIO based fan control
- USB (disabling XHCI controller makes to work as fallback USB2.0 ports)
- IOMMU

## Extra resources

- [Board manual]

[ASUS A88XM-E]: https://www.asus.com/Motherboards/A88XME/
[Board manual]: https://dlcdnets.asus.com/pub/ASUS/mb/SocketFM2/A88XM-E/E9125_A88XM-E.pdf
[flashrom]: https://flashrom.org/
[GD25Q64]: http://www.elm-tech.com/ja/products/spi-flash-memory/gd25q64/gd25q64.pdf
[Piledriver]: https://en.wikipedia.org/wiki/Piledriver_%28microarchitecture%29#APU_lines
[Sea Islands]: https://en.wikipedia.org/wiki/Graphics_Core_Next#GCN_2nd_generation
[Steamroller]: https://en.wikipedia.org/wiki/Steamroller_(microarchitecture)
[TeraScale 3]: https://en.wikipedia.org/wiki/TeraScale_%28microarchitecture%29#TeraScale_3
