# Gigabyte GA-H61M-S2PV

This page describes how to run coreboot on the [Gigabyte GA-H61M-S2PV] desktop
from [Gigabyte].

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | MX25L3206E |
+---------------------+------------+
| Size                | 4 MiB      |
+---------------------+------------+
| In circuit flashing | yes        |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | No         |
+---------------------+------------+
| Dual BIOS feature   | Yes        |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

### Internal programming

The main SPI flash can be accessed using [flashrom].

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
| EC               |                                                  |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Gigabyte GA-H61M-S2PV]: https://www.gigabyte.com/us/Motherboard/GA-H61M-S2PV-rev-10
[Gigabyte]: https://www.gigabyte.com
[flashrom]: https://flashrom.org/Flashrom
