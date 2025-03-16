# HP Compaq Pro 6300 Series

This page describes how to run coreboot on the HP [Compaq Pro 6300 Series] desktop.

These come in two versions: Microtower or Small Form Factor (SFF). They share the same
mainboard (657239-001) and Maintenance and Service Guide (690362-003/c04034127). This port has
been tested on a SFF unit.

## Working
- i3-3220 / e3-1225v2 CPUs
- SeaBIOS (version rel-1.16.3-0-ga6ed6b70)
- EDK2 (MrChromebox/2502)
- Fedora-mate with Linux kernels 6.11.4, 6.13.6; KDE neon with Linux kernel 6.8.0 / 6.11.0
- Mixed memory configurations from single 4GB to 24GB total with 1.35v & 1.5v modules
- Integrated Ethernet
- Serial port
- PS/2 keyboard and mouse
- Integrated graphics over DisplayPort and VGA port
- libgfxinit textmode (SeaBIOS) / framebuffer (EDK2)
- discrete GPU's show boot screen using SeaBIOS / EDK2 (LOAD_OPTION_ROMS=TRUE)
- All SATA ports
- All USB2 & USB3 ports
- PCI slot (Realtek RTL8169 GbE card)
- PCIe 3.0 x16 using 2.0 8x 10Gb Intel X540-AT2 / 1.0 16x nVidia GeForce(6200 LE / GT640-2GD3)
- PCIe 2.0 x1 using 2.0 1x 2.5Gb Realtek RTL8125
- PCIe 2.0 x1 using 1.0 1x 1Gb Intel 82574L (SeaBIOS loads option rom)
- Audio built-in speaker (plays music in OS compared to legacy bleep pc-speaker)
- Front panel audio ports (front headphone port overrides built-in speaker; only microphone
  works with combo mic/headphone with TRRS plug)
- Back panel audio ports
- Sensors CPU and 4 DIMM jc42-i2c sensors
- Booting USB / SATA(HDD/DVD)
- LEDs HDD, Power(blinks on suspend)
- Shutdown, Reboot, Suspend & Wake (USB keyboard & LAN)
- Strip down Intel ME/TXE firmware and hide MEI device

## Untested
- Parallel port

## Not working
- Simultaneous use of discrete and integrated graphics

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
| Model               | MT25Q128A  |
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
motherboard using a jumper to temporarily disable write protections while on the
stock firmware. Remove the jumper once coreboot is installed.

### External programming

External programming with an SPI adapter and [flashrom] does work, but it powers the
whole southbridge complex. You need to supply enough current through the programming adapter.

If you want to use a SOIC Pomona test clip, you have to cut the 2nd DRAM DIMM holder,
as otherwise there's not enough space near the flash.

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x (Q75)                                    |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| SuperIO          | Nuvoton NPCD379                                  |
+------------------+--------------------------------------------------+
| EC               |                                                  |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel ME                                         |
+------------------+--------------------------------------------------+
```

[Compaq Pro 6300 Series]: https://support.hp.com/us-en/product/details/hp-compaq-pro-6300-small-form-factor-pc/model/5232884
[flashrom]: https://flashrom.org/Flashrom
