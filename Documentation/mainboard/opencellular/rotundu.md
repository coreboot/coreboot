# Rutundu

This page describes how to run coreboot on the [Rotundu] compute board
from [OpenCellular].

## TODO

* Configure UART
* EC interface

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | W25Q128    |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| In circuit flashing | yes        |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | No         |
+---------------------+------------+
| Dual BIOS feature   | No         |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

### Internal programming

The SPI flash can be accessed using [flashrom].

### External programming

The GBCv1 board does have a pinheader to flash the SOIC-8 in circuit.
Directly connecting a Pomona test-clip on the flash is also possible.

**Closeup view of SOIC-8 flash IC**

![][rotundu_flash]

[rotundu_flash]: rotundu_flash.jpg

**SPI header**

![][rotundu_header2]

[rotundu_header2]: rotundu_header2.jpg

**SPI header pinout**

Dediprog compatible pinout.

![][rotundu_j16]

[rotundu_j16]: rotundu_j16.png

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| SoC              | Intel Baytrail                                   |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Rotundu]: https://github.com/Telecominfraproject/OpenCellular
[OpenCellular]: https://code.fb.com/connectivity/introducing-opencellular-an-open-source-wireless-access-platform/
[flashrom]: https://flashrom.org/Flashrom
