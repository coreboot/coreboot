# X11 LGA1151 series

The [Supermicro X11 LGA1151 series] are mostly the same boards with some minor differences in
internal and external interfaces like available PCIe slots, NICs (1 GbE, 10 GbE), IPMI, RAID
Controller etc.

## Supported boards

- [X11SSH-TF](x11ssh-tf/x11ssh-tf.md)
- [X11SSH-F/LN4F](x11ssh-f/x11ssh-f.md)
- [X11SSM-F](x11ssm-f/x11ssm-f.md)

## Required proprietary blobs

- [Intel FSP2.0]
- Intel ME

## De-blobbing

- [Intel FSP2.0] can not be removed as long as there is no free replacement
- Intel ME can be cleaned using me_cleaner (~4.5 MiB more free space)
- Intel Ethernet Controller Firmware can be removed when it's extended functionality is not
  needed. For more details refer to the respective datasheet (e.g 333016-008 for I210).
- Boards with [AST2400] BMC/IPMI: Firmware can be replaced by [OpenBMC]

## Flashing coreboot

Look at the [flashing tutorial] and the board-specific section.

## Known issues

These issues apply to all boards. Have a look at the board-specific issues, too.

- MRC caching does not work on cold boot with Intel SPS (see [Intel FSP2.0])

## ToDo

- Fix issues above
- Fix issues in board specific sections
- Add more boards! :-)

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| CPU              | Intel Kaby Lake                                  |
+------------------+--------------------------------------------------+
| PCH              | Intel C232/C236                                  |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel SPS (server version of the ME)             |
+------------------+--------------------------------------------------+
```

## Extra links

[Supermicro X11 LGA1151 series]: https://www.supermicro.com/products/motherboard/Xeon3000/#1151
[OpenBMC]: https://www.openbmc.org/
[flashrom]: https://flashrom.org/Flashrom
[flashing tutorial]: ../../../../tutorial/flashing_firmware/ext_power.md
[Intel FSP2.0]: ../../../../soc/intel/fsp/index.md
[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
