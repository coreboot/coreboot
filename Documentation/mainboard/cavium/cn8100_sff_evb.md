# CN81xx Evaluation-board SFF

## Specs

* 3 mini PCIe slots
* 4 SATA ports
* one USB3.0 A connector
* 20Pin JTAG
* 4 Gigabit Ethernet
* 2 SFP+ connectors
* PCIe x4 slot
* UART over USB
* eMMC Flash or MicroSD card slot for on-board storage
* 1 Slot with DDR-4 memory with ECC support
* SPI flash
* MMC and uSD-card

## Flashing coreboot

```eval_rst
+---------------------+----------------+
| Type                | Value          |
+=====================+================+
| Socketed flash      | no             |
+---------------------+----------------+
| Model               | Micron 25Q128A |
+---------------------+----------------+
| Size                | 8 MiB          |
+---------------------+----------------+
| In circuit flashing | no             |
+---------------------+----------------+
| Package             | SOIC-8         |
+---------------------+----------------+
| Write protection    | No             |
+---------------------+----------------+
| Dual BIOS feature   | No             |
+---------------------+----------------+
| Internal flashing   | ?              |
+---------------------+----------------+
```

## Notes about the hardware

1. Cavium connected *GPIO10* to a global reset line.
   It's unclear which chips are connected, but at least the PHY and SATA chips
   are connected.

2. The 4 QLMs can be configured using DIP switches (SW1). That means only a
   subset of of the available connectors is working at time.

3. The boot source can be configure using DIP switches (SW1).

4. The core and system clock frequency can be configured using DIP switches
   (SW3 / SW2).

5. The JTAG follows Cavium's own protocol. Support for it is missing in
   OpenOCD. You have to use ARMs official hardware and software.

## Technology

```eval_rst
+---------------+----------------------------------------+
| SoC           | :doc:`../../soc/cavium/cn81xx/index`   |
+---------------+----------------------------------------+
| CPU           | Cavium ARMv8-Quadcore `CN81XX`_        |
+---------------+----------------------------------------+

.. _CN81XX: https://www.cavium.com/product-octeon-tx-cn80xx-81xx.html

```

## Picture

![][cn81xx_board]

[cn81xx_board]: cavium_cn81xx_sff_evb.jpg
