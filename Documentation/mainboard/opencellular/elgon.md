# Elgon

This page describes how to run coreboot on the [Elgon] compute board
from [OpenCellular].

## TODO

* Add hard reset control

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | W25Q128    |
+---------------------+------------+
| Size                | 16 MiB     |
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

The EVT board does have a pinheader to flash the SOIC-8 in circuit.
Directly connecting a Pomona test-clip on the flash is also possible.

**Total board view of EVT**

![][elgon1]

[elgon1]: elgon1.jpg

**Closeup view of SOIC-8 flash IC and USB serial connector of EVT (marked blue)**

![][elgon2]

[elgon2]: elgon2.jpg


**SPI header (marked blue)**

![][elgon_conn_j9_pcb]

[elgon_conn_j9_pcb]: elgon_conn_j9_pcb.jpg

**SPI header pinout**

Dediprog compatible pinout.

![][elgon_conn_j9]

[elgon_conn_j9]: elgon_conn_j9.png

## Technology

```{eval-rst}
+---------------+----------------------------------------+
| SoC           | :doc:`../../soc/cavium/cn81xx/index`   |
+---------------+----------------------------------------+
| CPU           | Cavium ARMv8-Quadcore `CN81XX`_        |
+---------------+----------------------------------------+

.. _CN81XX: https://www.cavium.com/product-octeon-tx-cn80xx-81xx.html
```

[Elgon]: https://github.com/Telecominfraproject/OpenCellular
[OpenCellular]: https://code.fb.com/connectivity/introducing-opencellular-an-open-source-wireless-access-platform/
[flashrom]: https://flashrom.org/
