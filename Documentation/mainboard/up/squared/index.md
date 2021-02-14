# Squared

## Overview
### Top
![][overview_top]

### Bottom
![][overview_bottom]

* **Legend**
	* [BLUE][header_cn16_link]: UART0 / USB connector
	* [GREEN][header_gpio_link]: UART1 / GPIO header
	* [RED][header_cn22_link]: SPI header
	* YELLOW: Indicates pin 1

## Mainboard components
### Platform
```eval_rst
+------------------+----------------------------------+
| CPU              | Intel Atom, Celeron, Pentium     |
+------------------+----------------------------------+
| PCH              | Intel Apollo Lake                |
+------------------+----------------------------------+
| EC / Super IO    | N/A                              |
+------------------+----------------------------------+
| Coprocessor      | Intel TXE 3.0                    |
+------------------+----------------------------------+
```

### Flash chip
```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q128FW  |
+---------------------+------------+
| Voltage             | 1.8V       |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | No         |
+---------------------+------------+
| Internal flashing   | No         |
+---------------------+------------+
| In curcuit flashing | Yes        |
+---------------------+------------+
```

### Debugging
#### UART0 (CN16)
This connector is located on the **bottom** side (see [here][overview_bottom_link]).
![][header_cn16]


#### UART1 (GPIO header)
The GPIO header is located on the **bottom** side (see [here][overview_bottom_link]).
![][header_gpio]

## Building and flashing coreboot
### Using the SPI header
The SPI header is located on the **bottom** side (see [here][overview_bottom_link]).
![][header_cn22]

### Preperations
In order to build coreboot, it's neccessary to extract some files from the vendor firmware. Make sure that you have a fully working dump.
```bash
[upsquared]$ ls
firmware_vendor.rom
```

```bash
[upsquared]$ mkdir extracted && cd extracted
[extracted]$ ifdtool -x ../firmware_vendor.rom
File ../firmware_vendor.rom is 16777216 bytes
Peculiar firmware descriptor, assuming Ibex Peak compatibility.
  Flash Region 0 (Flash Descriptor): 00000000 - 00000fff
  Flash Region 1 (BIOS): 00001000 - 00efefff
  Flash Region 2 (Intel ME): 07fff000 - 00000fff (unused)
  Flash Region 3 (GbE): 07fff000 - 00000fff (unused)
  Flash Region 4 (Platform Data): 07fff000 - 00000fff (unused)
  Flash Region 5 (Reserved): 00eff000 - 00ffefff
  Flash Region 6 (Reserved): 07fff000 - 00000fff (unused)
  Flash Region 7 (Reserved): 07fff000 - 00000fff (unused)
  Flash Region 8 (EC): 07fff000 - 00000fff (unused)
```

```bash
flashregion_0_flashdescriptor.bin
flashregion_1_bios.bin
flashregion_5_reserved.bin
```

### Clean up
```bash
[coreboot]$ make distclean
```

### Configuring
```bash
[coreboot]$ touch .config
[coreboot]$ ./util/scripts/config --enable VENDOR_UP
[coreboot]$ ./util/scripts/config --enable BOARD_UP_SQUARED
[coreboot]$ ./util/scripts/config --enable NEED_IFWI
[coreboot]$ ./util/scripts/config --enable HAVE_IFD_BIN
[coreboot]$ ./util/scripts/config --set-str IFWI_FILE_NAME "<flashregion_1_bios.bin>"
[coreboot]$ ./util/scripts/config --set-str IFD_BIN_PATH "<flashregion_0_flashdescriptor.bin>"
[coreboot]$ make olddefconfig
```

### Building
```bash
[coreboot]$ make
```

Now you should have a working and ready to use coreboot build at `build/coreboot.rom`.

### Flashing
```bash
[coreboot]$ flashrom -p <your_programmer> -w build/coreboot.rom
```

## Board status
### Working
- bootblock, romstage, ramstage
- Serial console UART0, UART1
- SPI flash console
- iGPU init with libgfxinit
- LAN1, LAN2
- USB2, USB3
- HDMI, DisplayPort
- eMMC
- flashing with flashrom externally

### Work in progress
- Documentation
- ACPI

### Not working / Known issues
- Generally SeaBIOS works, but it can't find the CBFS region and therefore it can't load seavgabios. This is because of changes at the Apollolake platform.

### Untested
- GPIO pin header
- 60 pin EXHAT
- Camera interface
  - MIPI-CSI2 2-lane (2MP)
  - MIPI-CSI2 4-lane (8MP)
- SATA3
- USB3 OTG
- embedded DisplayPort
- M.2 slot
- mini PCIe
- flashing with flashrom internally using Linux


[header_cn16]: header_cn16_10pin_uart0.svg
[header_cn16_link]: #uart0-cn16
[header_cn22]: header_cn22_12pin_spi.svg
[header_cn22_link]: #using-the-spi-header
[header_gpio]: header_40pin_gpio_uart1.svg
[header_gpio_link]: #uart1-gpio-header
[overview_top]: top.jpg
[overview_bottom]: bottom.jpg
[overview_bottom_link]: #bottom
