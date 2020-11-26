# Supermicro X11SSH-TF

This section details how to run coreboot on the [Supermicro X11SSH-TF].

## Flashing coreboot

The board can be flashed externally using *some* programmers. The CH341 was found working, while
Dediprog didn't detect the chip.

The flash IC can be found between the two PCIe slots near the southbridge:
![](x11ssh-tf_flash.jpg)

## BMC (IPMI)

This board has an ASPEED [AST2400], which has BMC/[IPMI] functionality. The BMC firmware resides
in a 32 MiB SOIC-16 chip in the corner of the mainboard near the [AST2400]. This chip is an
[MX25L25635F].

## Tested and working

- USB ports
- Ethernet
- SATA ports
- RS232 external
- ECC DRAM detection
- PCIe slots
- M.2 2280 NVMe slot
- BMC (IPMI)
- VGA on Aspeed
- TPM on TPM expansion header

## Known issues

See general issue section.

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
| Ethernet         | 2x Intel® X550 10GBase-T Ethernet                |
|                  | 1x dedicated BMC                                 |
+------------------+--------------------------------------------------+
| PCIe slots       | 1x 3.0 x8                                        |
|                  | 1x 3.0 x2 (in x4)                                |
|                  | 1x 3.0 M.2 2260 x4 (Key M, with S-ATA)           |
+------------------+--------------------------------------------------+
| USB slots        | 2x USB 2.0 (ext)                                 |
|                  | 2x USB 3.0 (ext)                                 |
|                  | 1x USB 3.0 (int)                                 |
|                  | 1x dual USB 3.0 header                           |
|                  | 2x dual USB 2.0 header                           |
+------------------+--------------------------------------------------+
| SATA slots       | 8x SATA III                                      |
+------------------+--------------------------------------------------+
| Other slots      | 1x RS232 (ext)                                   |
|                  | 1x RS232 header                                  |
|                  | 1x TPM header                                    |
|                  | 1x Power SMB header                              |
|                  | 6x PWM Fan connector                             |
|                  | 2x I-SGPIO                                       |
|                  | 2x S-ATA DOM Power connector                     |
|                  | 1x XDP Port                                      |
|                  | 1x External BMC I2C Header (for IPMI card)       |
|                  | 1x Chassis Intrusion Header                      |
+------------------+--------------------------------------------------+
```

## Extra links

- [Supermicro X11SSH-TF]
- [Board manual]

[Supermicro X11SSH-TF]: https://www.supermicro.com/en/products/motherboard/X11SSH-TF
[Board manual]: https://www.supermicro.com/manuals/motherboard/C236/MNL-1783.pdf
[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
[IPMI]: ../../../../drivers/ipmi_kcs.md
[MX25L25635F]: https://media.digikey.com/pdf/Data%20Sheets/Macronix/MX25L25635F.pdf
[N25Q128A]: https://www.micron.com/~/media/Documents/Products/Data%20Sheet/NOR%20Flash/Serial%20NOR/N25Q/n25q_128mb_3v_65nm.pdf
