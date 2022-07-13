# Facebook Monolith

This page describes how to run coreboot on the Facebook Monolith.

Please note: the coreboot implementation for this board is in its
Beta state and isn't fully tested yet.

## Required blobs

Mainboard is based on the Intel Kaby Lake U SoC.
Intel company provides [Firmware Support Package (2.0)](../../soc/intel/fsp/index.md)
(intel FSP 2.0) to initialize this generation silicon. Please see this
[document](../../soc/intel/code_development_model/code_development_model.md).

FSP Information:

```eval_rst
+-----------------------------+-------------------+-------------------+
| FSP Project Name            | Directory         | Specification     |
+-----------------------------+-------------------+-------------------+
| 7th Generation Intel® Core™ | KabylakeFspBinPkg | 2.0               |
| processors  and chipsets    |                   |                   |
| (formerly Kaby Lake)        |                   |                   |
+-----------------------------+-------------------+-------------------+
```

Microcode: 3rdparty/intel-microcode/intel-ucode

## Flash components

To create a complete flash image, the flash descriptor, GBE and ME blobs are required. The
complete image can be used when e.g. a blank flash should be programmed. In other cases (when
only coreboot needs to be replaced) placeholders can be used for the GBE and ME regions.

These can be extracted from the original flash image as follows:
1) Read the complete image from flash.
2) Create a layout file with the following content:
```
00000000:00000fff fd
00700000:00ffffff bios
00003000:006FFFFF me
00001000:00002fff gbe
```
3) Use `ifdtool -n <layout_file> <flash_image>` to resize the *bios* region from the default 6 MiB
   to 9 MiB, this is required to create sufficient space for LinuxBoot.
   NOTE: Please make sure only the firmware descriptor (*fd*) region is changed. Older versions
   	 of the ifdtool corrupt the *me* region.
4) Use `ifdtool -x <resized_flash_image>` to extract the components.

The regions extracted can be used to generate a full flash image. The *bios* region is
not needed as this is replaced by the coreboot image.

NOTE: The gbe region contains the MAC address so be careful. When updating the flash using
      flashrom it is advisable to leave out the *gbe* area.

## Flashing coreboot

### Internal programming

The SPI flash can be accessed using [flashrom].

The descriptor area needs to be updated once to resize the *bios* region.
`flashrom -p internal --ifd -i fd -w <coreboot.bin>`

After that only the bios area should to be updated.
`flashrom -p internal --ifd -i bios -w <coreboot.bin>`

The *gbe* and *me* regions should not be updated.

NOTE: As `flashrom --ifd` uses the flash descriptor it is required to update the
      descriptor and bios regions in the right sequence. Don't update both in one command.

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
- Full Embedded Controller support
- SATA
- xDCI

## Working

- USB
- Gigabit Ethernet (i219 and i210)
- Graphics (Using FSP GOP)
- flashrom
- PCIe including hotplug on FPGA root port
- EC serial port
- EC CPU temperature
- SMBus
- Initialization with FSP
- SeaBIOS payload (commit a5cab58e9a3fb6e168aba919c5669bea406573b4)
- edk2 payload (commit 860a8d95c2ee89c9916d6e11230f246afa1cd629)
- LinuxBoot (kernel kernel-4_19_97) (uroot commit 9c9db9dbd6b532f5f91a511a0de885c6562aadd7)
- eMMC

All of the above has been briefly tested by booting Linux from eMMC using the edk2 payload
and LinuxBoot.

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
