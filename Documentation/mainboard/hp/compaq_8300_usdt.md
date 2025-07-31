# HP Compaq Elite 8300 USDT

This page describes how to run coreboot on the [Compaq Elite 8300 USDT] desktop
from [HP].

## Flashing coreboot

```{eval-rst}
+---------------------+-------------+
| Type                | Value       |
+=====================+=============+
| Socketed flash      | no          |
+---------------------+-------------+
| Model               | W25Q128BVFG |
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
```

### Internal programming

Internal programming is possible. Shorting the Flash Descriptor Override
(FDO) jumper bypasses all write protections.

### External programming

Remove the lid. The flash chip can be found on the edge opposite to the CPU.
There is a spot for a "ROM RCVRY" header next to the flash chip but it is
unpopulated. If you don't feel like using a clip, you can easily solder
a standard pin header there yourself and use it for programming.

Programming powers some parts of the board. Programming when
Wake on LAN is active works great.

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x                                          |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| SuperIO          | NPCD379HAKFX                                     |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

### SuperIO

This board has a Nuvoton NPCD379 SuperIO chip. Fan speed and PS/2 keyboard work
fine using coreboot's existing code for
<project:../../superio/nuvoton/npcd378.md>.

[Compaq Elite 8300 USDT]: https://support.hp.com/gb-en/product/hp-compaq-elite-8300-ultra-slim-pc/5232866
[HP]: https://www.hp.com/
