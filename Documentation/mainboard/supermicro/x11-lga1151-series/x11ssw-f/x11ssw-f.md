# Supermicro X11SSW-F

This section details how to run coreboot on the [Supermicro X11SSW-F].

## Flashing coreboot

Flashing was performed through the BMC web interface, when a valid license was entered.

## Tested and working

- SeaBIOS version 1.16.2 Payload loading Ubuntu 22.04, kernel 5.4.0-162-generic
- ECC ram (Linux' ie31200 driver works)
- USB ports
- Ethernet
- SATA ports
- RS232 external
- Left PCIe slot
- BMC (IPMI)
- VGA on Aspeed

## Untested
- Right PCIe slot
- NVMe
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
| PCIe slots       | 1x 3.0 x16 (Left Riser)                          |
|                  | 1x 3.0 x4  (Right Riser in x16)                  |
|                  | 1x 3.0 x4  (NVMe)                                |
+------------------+--------------------------------------------------+
| USB slots        | 2x USB 2.0 (ext)                                 |
|                  | 2x USB 3.0 (ext)                                 |
|                  | 1x USB 3.0 (int)                                 |
|                  | 1x dual USB 3.0 header                           |
|                  | 2x dual USB 2.0 header                           |
+------------------+--------------------------------------------------+
| SATA slots       | 6x S-ATA III                                     |
+------------------+--------------------------------------------------+
| Other slots      | 1x RS232 (ext)                                   |
|                  | 1x RS232 header                                  |
|                  | 1x TPM header                                    |
|                  | 1x Power SMB header                              |
|                  | 6x PWM Fan connector                             |
|                  | 1x Chassis Intrusion Header                      |
+------------------+--------------------------------------------------+
```

## Extra links

- [Supermicro X11SSW-F]
- [Board manual]

[Supermicro X11SSW-F]: https://www.supermicro.com/en/products/motherboard/x11ssw-f
[Board manual]: https://www.supermicro.com/manuals/motherboard/C236/MNL-1784.pdf
[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
[IPMI]: ../../../../drivers/ipmi_kcs.md
