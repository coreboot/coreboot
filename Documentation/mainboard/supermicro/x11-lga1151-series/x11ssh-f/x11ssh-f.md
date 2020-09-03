# Supermicro X11SSH-F

This section details how to run coreboot on the [Supermicro X11SSH-F].

## Flashing coreboot

The board can be flashed externally. [STM32-based programmers] worked.

The flash IC "W25Q128.V" (detected by flashrom) can be found near PCH PCIe Slot 4. It is sometime
socketed, and covered by a sticker, hindering the observation of its precise model.

It can be programmed in-system with a clip like pomona 5250.

## BMC (IPMI)

This board has an ASPEED [AST2400], which has BMC/[IPMI] functionality. The BMC firmware resides in a
32 MiB SOIC-16 chip in the corner of the mainboard near the PCH PCIe Slot 4. This chip is a
[MX25L25635F].

## IGD

If an IGD is integrated with CPU, it will be enabled on this board. Though there is no video output
for it (The onboard VGA port is connected to BMC), it is said to be capable of being used for compute
tasks, or for offloading graphics rendering via "muxless" [vga_witcheroo].

## Tested and working

- SeaBIOS payload to boot Kali Linux live USB
- ECC ram (Linux' ie31200 driver works)
- Integrated graphics device available without output
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
- S3 resume not working (vendor and coreboot)
- SeaBIOS cannot make use of VGA on Aspeed (even if IGD is disabled)

## ToDo

- Fix known issues
- Testing other payloads

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
|                  | 1x 3.0 x4 (in x8)                                |
|                  | 1x 3.0 x2 (in M.2 slot with key M)               |
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
|                  | 1x XDP Port (connector may absent)               |
|                  | 1x External BMC I2C Header (for IPMI card)       |
|                  | 1x Chassis Intrusion Header                      |
+------------------+--------------------------------------------------+
```

## Extra links

- [Supermicro X11SSH-F]
- [Board manual]

[Supermicro X11SSH-F]: https://www.supermicro.com/en/products/motherboard/X11SSH-F
[Board manual]: https://www.supermicro.com/manuals/motherboard/C236/MNL-1778.pdf
[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
[IPMI]: ../../../../drivers/ipmi_kcs.md
[MX25L25635F]: https://media.digikey.com/pdf/Data%20Sheets/Macronix/MX25L25635F.pdf
[STM32-based programmers]: https://github.com/dword1511/stm32-vserprog
[vga_switcheroo]: https://01.org/linuxgraphics/gfx-docs/drm/gpu/vga-switcheroo.html
