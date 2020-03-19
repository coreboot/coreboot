# Intel Kaby lake RVP11

## Specs

* 1 SATA cable connect
* 1 SATAe direct
* 2 USB2.0 connector
* 4 USB3.0 connector
* 1 Gigabit Ethernet
* 1 x4 PCIe slot
* 1 x1 PCIe slot
* 1 X16 PEG slot
* UART debug DB9 connector
* 4 DIMMS with DDR4 memory
* SPI flash
* Audio Jack
* PS2 Keyboard and Mouse
* Display: HDMI, DP, VGA

## Target Audience

* OEMs, internal only

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q128FV  |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | No         |
+---------------------+------------+
| Dual BIOS feature   | No         |
+---------------------+------------+
```

### Instruction to flash coreboot to SPI

### Internal programming

The SPI flash can be accessed internally using [flashrom].
The following command is used to flash BIOS region.

```bash
$ flashrom -p internal --ifd -i bios -w coreboot.rom --noverify-all
```

### External programming

1. Dediprog SF600 with adapter B is used.
2. Make sure power supply is disconnected from board.
3. Connect Dediprog SF600 to header at J7H1.
4. Ensure that "currently working on" is in "application memory chip 1"
5. Go to "file" and select the .rom file (16 MiB) to program chip1.
6. Execute the batch operation to erase and program the chip.

## Technology

```eval_rst
+------------------+---------------------------------------------------+
| CPU              | Kaby lake H (i7-7820EQ)                           |
+------------------+---------------------------------------------------+
| PCH              | Skylake PCH-H (called SPT-H)                      |
+------------------+---------------------------------------------------+
| Coprocessor      | Intel ME                                          |
+------------------+---------------------------------------------------+
```

[W25Q128FV]: https://www.winbond.com/resource-files/w25q128fv%20rev.m%2005132016%20kms.pdf
[flashrom]: https://flashrom.org/Flashrom
