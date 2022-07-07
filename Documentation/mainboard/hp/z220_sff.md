# HP Z220 SFF Workstation

This page describes how to run coreboot on the [HP Z220 SFF Workstation] desktop
from [HP].

## TODO

The following things are still missing from this coreboot port:

- Extended HWM reporting
- Advanced LED control
- Advanced power configuration in S3

## Flashing coreboot

```eval_rst
+---------------------+-------------+
| Type                | Value       |
+=====================+=============+
| Socketed flash      | no          |
+---------------------+-------------+
| Model               | N25Q128..3E |
+---------------------+-------------+
| Size                | 16 MiB      |
+---------------------+-------------+
| In circuit flashing | yes         |
+---------------------+-------------+
| Package             | SOIC-16     |
+---------------------+-------------+
| Write protection    | No          |
+---------------------+-------------+
| Dual BIOS feature   | No          |
+---------------------+-------------+
| Internal flashing   | yes         |
+---------------------+-------------+
```

### Internal programming

The SPI flash can be accessed using [flashrom].

### External programming

External programming with an SPI adapter and [flashrom] does work, but it powers the
whole southbridge complex. You need to supply enough current through the programming adapter.

If you want to use a SOIC pomona test clip, you have to cut the 2nd DRAM DIMM holder, as
otherwise there's not enough space near the flash.

In both case, if ME has not been completely disabled, ME/AMT Flash Override jumper had better
be temporary closed for flashing to disable the locking of regions, and prevent ME to run and
interfere.

## Side note
The mainboard of [HP Compaq Elite 8300 SFF] is very similar to the one of Z220 SFF, except
that Compaq Elite 8300 uses Q77 instead of C216 for its PCH, and their boot firmwares are
even interchangeable, so should do coreboot images built for them.

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

[HP Z220 SFF Workstation]: https://support.hp.com/za-en/document/c03386950
[HP Compaq Elite 8300 SFF]: https://support.hp.com/us-en/document/c03345460
[HP]: https://www.hp.com/
[flashrom]: https://flashrom.org/Flashrom
