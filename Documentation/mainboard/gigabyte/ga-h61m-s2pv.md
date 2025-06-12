# Gigabyte GA-H61M-S2PV

This page describes how to run coreboot on the [Gigabyte GA-H61M-S2PV] desktop
from [Gigabyte].

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | No         |
+---------------------+------------+
| Model               | MX25L3206E |
+---------------------+------------+
| Size                | 4 MiB      |
+---------------------+------------+
| In circuit flashing | No         |
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
flash chip's contents when using the `--ifd` option. A possible workaround is
to create a `layout.txt` file with a non-overlapping BIOS region:

	00000000:00000fff fd
	00180000:003fffff bios
	00001000:0017ffff me

After that, use flashrom with the new layout file. For example, to create a
backup of the BIOS region and then flash a `coreboot.rom`, do:

```bash
sudo flashrom -p internal -l layout.txt -i bios -r backup.rom
sudo flashrom -p internal -l layout.txt -i bios -w coreboot.rom
```

Modifying the IFD so that the BIOS region does not overlap would work as well.
However, this makes DualBIOS unable to recover from a bad flash for some reason.

## Technology

```{eval-rst}
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
[flashrom]: https://flashrom.org/
