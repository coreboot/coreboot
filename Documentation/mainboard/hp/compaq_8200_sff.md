# HP Compaq 8200 Elite SFF

This page describes how to run coreboot on the [Compaq 8200 Elite SFF] desktop
from [HP].

## TODO

The following things are still missing from this coreboot port:

- Extended HWM reporting
- Advanced LED control
- Advanced power configuration in S3

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | MX25L6406E |
+---------------------+------------+
| Size                | 8 MiB      |
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

External programming with an SPI adapter and [flashrom] does work, but it powers the
whole southbridge complex. You need to supply enough current through the programming adapter.

If you want to use a SOIC pomona test clip, you have to cut the 2nd DRAM DIMM holder,
as otherwise there's not enough space near the flash.

**Position of SOIC-8 flash IC near 2nd DIMM holder**
![][compaq_8200_flash1]

[compaq_8200_flash1]: compaq_8200_sff_flash1.jpg

**Closeup view of SOIC-8 flash IC**
![][compaq_8200_flash2]

[compaq_8200_flash2]: compaq_8200_sff_flash2.jpg

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x                                          |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| SuperIO          | :doc:`../../superio/nuvoton/npcd378`             |
+------------------+--------------------------------------------------+
| EC               |                                                  |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Compaq 8200 Elite SFF]: https://support.hp.com/us-en/document/c03414707
[HP]: https://www.hp.com/
[flashrom]: https://flashrom.org/Flashrom
