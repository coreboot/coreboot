# Squared

## Overview
### Top
![][overview_top]

### Bottom
![][overview_bottom]

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

## Building and flashing coreboot
### Building

```bash
make distclean
touch .config
./util/scripts/config --enable VENDOR_UP
./util/scripts/config --enable BOARD_UP_SQUARED
./util/scripts/config --enable NEED_IFWI
./util/scripts/config --enable HAVE_IFD_BIN
./util/scripts/config --set-str IFWI_FILE_NAME "<path_to_your_bios_region>"
./util/scripts/config --set-str IFD_BIN_PATH "<path_to_your_ifd_region>"
make olddefconfig
```

### Flashing

[overview_top]: top.jpg
[overview_bottom]: bottom.jpg
