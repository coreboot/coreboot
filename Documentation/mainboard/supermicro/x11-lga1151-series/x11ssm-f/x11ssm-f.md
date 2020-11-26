# Supermicro X11SSM-F

This section details how to run coreboot on the [Supermicro X11SSM-F].

## Flashing coreboot

The board can be flashed externally. FTDI FT2232H and FT232H based programmers worked. For this,
one needs to add a diode between VCC and the flash chip. The flash IC [MX25L12873F] can be found
near PCH PCIe Slot 4.

Flashing is also possible through the BMC web interface, when a valid license was entered.

## BMC (IPMI)

This board has an ASPEED [AST2400], which has BMC/[IPMI] functionality. The BMC firmware resides in a
32 MiB SOIC-16 chip in the corner of the mainboard near the PCH PCIe Slot 4. This chip is a
[MX25L25635F].

## Disabling LAN firmware

To disable the proprietary LAN firmware, the undocumented jumper J6 can be set to 2-3.

## Tested and working

- GRUB2 payload with Debian testing and kernel 5.2
- ECC ram (Linux' ie31200 driver works)
- USB ports
- Ethernet
- SATA ports
- RS232 external
- PCIe slots
- BMC (IPMI)
- VGA on Aspeed
- TPM on TPM expansion header

## Known issues

- See general issue section
- LNXTHERM missing
- S3 resume not working

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| CPU              | Intel Kaby Lake                                  |
+------------------+--------------------------------------------------+
| PCH              | Intel C236                                       |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel SPS (server version of the ME)             |
+------------------+--------------------------------------------------+
| Super I/O        | ASPEED AST2400                                   |
+------------------+--------------------------------------------------+
| Ethernet         | 2x Intel I210-AT 1 GbE                           |
|                  | 1x dedicated BMC                                 |
+------------------+--------------------------------------------------+
| PCIe slots       | 1x 3.0 x8                                        |
|                  | 1x 3.0 x8 (in x16)                               |
|                  | 2x 3.0 x4 (in x8)                                |
+------------------+--------------------------------------------------+
| USB slots        | 2x USB 2.0 (ext)                                 |
|                  | 2x USB 3.0 (ext)                                 |
|                  | 1x USB 3.0 (int)                                 |
|                  | 1x dual USB 3.0 header                           |
|                  | 2x dual USB 2.0 header                           |
+------------------+--------------------------------------------------+
| SATA slots       | 8x S-ATA III                                     |
+------------------+--------------------------------------------------+
| Other slots      | 1x RS232 (ext)                                   |
|                  | 1x RS232 header                                  |
|                  | 1x TPM header                                    |
|                  | 1x Power SMB header                              |
|                  | 5x PWM Fan connector                             |
|                  | 2x I-SGPIO                                       |
|                  | 2x S-ATA DOM Power connector                     |
|                  | 1x XDP Port                                      |
|                  | 1x External BMC I2C Header (for IPMI card)       |
|                  | 1x Chassis Intrusion Header                      |
+------------------+--------------------------------------------------+
```

## Extra links

- [Supermicro X11SSM-F]
- [Board manual]

[Supermicro X11SSM-F]: https://www.supermicro.com/en/products/motherboard/X11SSM-F
[Board manual]: https://www.supermicro.com/manuals/motherboard/C236/MNL-1785.pdf
[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
[IPMI]: ../../../../drivers/ipmi_kcs.md
[MX25L25635F]: https://media.digikey.com/pdf/Data%20Sheets/Macronix/MX25L25635F.pdf
[MX25L12873F]: https://media.digikey.com/pdf/Data%20Sheets/Macronix/MX25L12873F.pdf
