# Gigabyte GA-H61M-S2PV

This page describes how to run coreboot on the [Gigabyte GA-H61M-S2PV] desktop
from [Gigabyte].

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | No         |
+---------------------+------------+
| Model               | MX25L3206E |
+---------------------+------------+
| Size                | 4 MiB      |
+---------------------+------------+
| In circuit flashing | Yes        |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | No         |
+---------------------+------------+
| Dual BIOS feature   | Yes        |
+---------------------+------------+
| Internal flashing   | Yes        |
+---------------------+------------+
```

### Internal programming

The main SPI flash can be accessed using [flashrom]. The DualBIOS backup flash
chip is accessible as well using the `dualbiosindex` programmer parameter.
Since the flash recovery mechanism works even with coreboot installed on the
main flash chip (it still restores the vendor UEFI though), it is useful to
leave the backup chip untouched.

### Notes about the original firmware

The original IFD defines the BIOS region as the whole flash chip. While this is
not an issue if flashing a complete image, it confuses flashrom and trashes the
flash chip's contents when using the --ifd option. However, this can be easily
fixed by reading the IFD with flashrom, editing the correct values into it with
ifdtool and then reflashing it.

Create a layout.txt with the following contents:

	00000000:00000fff fd
	00180000:003fffff bios
	00001000:0017ffff me

After that, simply run:

```bash
sudo flashrom -p internal --ifd -i fd -r ifd.rom
ifdtool -n layout.txt ifd.rom
sudo flashrom -p internal --ifd -i fd -w ifd.rom.new
```

After flashing, power cycle the computer to ensure the new IFD is being used.
If only a reboot is done, the old IFD layout is still seen by flashrom, even if
the IFD on the flash chip is correctly defining the new region layout.

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x                                          |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| SuperIO          | ITE IT8728F                                      |
+------------------+--------------------------------------------------+
| EC               | None                                             |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Gigabyte GA-H61M-S2PV]: https://www.gigabyte.com/us/Motherboard/GA-H61M-S2PV-rev-10
[Gigabyte]: https://www.gigabyte.com
[flashrom]: https://flashrom.org/Flashrom
