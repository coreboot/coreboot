# HP Compaq 8300 Elite SFF

This page describes how to run coreboot on the [Compaq 8300 Elite SFF] desktop
from [HP].

## TODO

The following things are still missing from this coreboot port:

- Extended HWM reporting
- Advanced LED control
- Advanced power configuration in S3

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | MT25QL128  |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| In circuit flashing | yes        |
+---------------------+------------+
| Package             | SOIC-16    |
+---------------------+------------+
| Write protection    | Yes        |
+---------------------+------------+
| Dual BIOS feature   | No         |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

### Internal programming

Internal flashing is possible. The SPI flash can be accessed using [flashrom],
but you have to short the FDO pins located near the rear USB3 ports on the
motherboard using a jumper, to temporarily disable write protections while on the
stock firmware. Once the coreboot rom is flashed, one should remove the jumper.

### External programming

External programming with an SPI adapter and [flashrom] does work, but it powers the
whole southbridge complex. You need to supply enough current through the programming adapter.

If you want to use a SOIC pomona test clip, you have to cut the 2nd DRAM DIMM holder,
as otherwise there's not enough space near the flash.

## Technology

```{eval-rst}
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

[Compaq 8300 Elite SFF]: https://support.hp.com/us-en/document/c03345460
[HP]: https://www.hp.com/
[flashrom]: https://flashrom.org/Flashrom
