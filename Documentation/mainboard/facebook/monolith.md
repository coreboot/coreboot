# Facebook Monolith

This page describes how to run coreboot on the Facebook Monolith.

Please note: the coreboot implementation for this boards is in its
Alpha state and isn't fully tested yet.

## Required blobs

This board currently requires:
fsp blobs	3rdparty/fsp/KabylakeFspBinPkg/Fsp_M.fd
		3rdparty/fsp/KabylakeFspBinPkg/Fsp_S.fd

Microcode	3rdparty/intel-microcode/intel-ucode

## Flashing coreboot

### Internal programming

The SPI flash can be accessed using [flashrom].

### External programming

The system has an internal flash chip which is a 16 MiB soldered SOIC-8 chip.
Specifically, it's a Winbond W25Q128JVSIQ (3.3V).

The system has an external flash chip which is a 16 MiB soldered SOIC-8 chip.
Specifically, it's a Winbond W25Q128JVSIM (3.3V).

Flashing of these devices is very difficult, disassembling the system destroys the cooling
solution. Wires need to be connected to be able to flash using an external programmer.

## Known issues

- None

## Untested

- Hardware monitor
- SDIO
- Full Embedded Controller support
- eMMC
- SATA

## Working

- USB
- Gigabit Ethernet
- Graphics (Using FSP GOP)
- flashrom
- PCIe
- EC serial port
- SMBus
- Initialization with FSP
- SeaBIOS payload (commit a5cab58e9a3fb6e168aba919c5669bea406573b4)
- TianoCore payload (commit a5cab58e9a3fb6e168aba919c5669bea406573b4)

All of the above has been briefly tested by booting Linux from the TianoCore payload.
SeaBios has been checked to the extend that it runs to the boot selection and provides display
output.

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| SoC              | Intel Kaby Lake U                                |
+------------------+--------------------------------------------------+
| CPU              | Intel i3-7100U                                   |
+------------------+--------------------------------------------------+
| Super I/O, EC    | ITE8528                                          |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

[W25Q128JVSIQ]: https://www.winbond.com/resource-files/w25q128jv%20revf%2003272018%20plus.pdf
[W25Q128JVSIM]: https://www.winbond.com/resource-files/w25q128jv%20dtr%20revb%2011042016.pdf
[flashrom]: https://flashrom.org/Flashrom
