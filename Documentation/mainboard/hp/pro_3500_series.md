# HP Pro 3500 Series

This page describes how to run coreboot on the [Pro 3500 Series]
desktop from [HP].

## State

All peripherals should work. Automatic fan control as well as S3 are
working. The board was tested to boot Linux and Windows. EHCI debug
is untested. When using MrChromebox edk2 with secure boot build in, the
board will hang on each boot for about 20 seconds before continuing.
With disabled ME, the SuperIO will not get CPU temperatures via PECI and
therefore the automatic fan control will not increase the fan speed.

## Flashing coreboot

```{eval-rst}
+---------------------+-------------------------+
| Type                | Value                   |
+=====================+=========================+
| Socketed flash      | No                      |
+---------------------+-------------------------+
| Model               | W25Q64FVSIG             |
+---------------------+-------------------------+
| Size                | 8 MiB                   |
+---------------------+-------------------------+
| In circuit flashing | Yes                     |
+---------------------+-------------------------+
| Package             | SOIC-8                  |
+---------------------+-------------------------+
| Write protection    | See below               |
+---------------------+-------------------------+
| Dual BIOS feature   | No                      |
+---------------------+-------------------------+
| Internal flashing   | Yes                     |
+---------------------+-------------------------+
```

### Flash layout
The original layout of the flash should look like this:
```
00000000:00000fff fd
00400000:007fffff bios
00001000:003fffff me
00fff000:00000fff gbe
00fff000:00000fff pd
```

### Internal programming

The SPI flash can be accessed using [flashrom] (although it reports as
"N25Q064..3E", it works fine).

With a missing FDO jumper, `fd` region is read-only, `bios` region is
read-write and `me` region is locked. Vendor firmware will additionally
protect the flash chip. After shorting the FDO jumper (E2) full
read-write access is granted.

Do **NOT shutdown** the operating system **after flashing** coreboot
from the vendor firmware! This will brick your device because the bios
region will be modified on shutdown. Cut the AC power or do a restart
from the OS.

**Position of FDO jumper (E2) close to the F_USB3**
![][pro_3500_jumper]

[pro_3500_jumper]: pro_3500_series_jumper.avif

### External programming

External programming with an SPI adapter and [flashrom] does work, but
it powers the whole southbridge complex. The average current will be
400mA but spikes may be higher. Connect the power to the flash or the
programming header next to the flash otherwise programming is unstable.
The supply needs to quickly reach 3V3 or else the chip is also unstable
until cleanly power cycled.

**Position of SOIC-8 flash and pin-header near ATX power connector**
![][pro_3500_flash]

[pro_3500_flash]: pro_3500_series_flash.avif

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x (bd82h61)                                |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| SuperIO          | IT8779E (identifies as IT8772F via register)     |
+------------------+--------------------------------------------------+
| EC               | Fixed function as part of SuperIO                |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Pro 3500 Series]: https://support.hp.com/us-en/document/c03364089
[HP]: https://www.hp.com/
[flashrom]: https://flashrom.org/Flashrom
