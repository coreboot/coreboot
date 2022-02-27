# Hermes

Hermes is a regular ATX board designed for workstation PCs.

The board features:
* 5 PCIe 16x Gen3 slots
* 4 ECC capable DDR4 DIMMs
* 5 dedicated Ethernet ports
* 1 BMC Ethernet port
* VGA
* COM port
* 2 COM port headers
* 4 SATA ports,
* NVMe M2 slot
* CNVi M2 slot
* 3 optional DisplayPort outputs
* optional TPM2

## Required proprietary blobs

- [Intel FSP2.0]
- Intel SPS

## Flashing coreboot

* The BIOS flash can be updated over the BMC, but the update file has a proprietary format
* For development a dediprog compatible pinheader is present which allows to use an EM100

## Known issues

- MRC caching does not work on cold boot with Intel SPS (see [Intel FSP2.0])

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| CPU              | CoffeeLake + CoffeeLake R (Core + Xeon)          |
+------------------+--------------------------------------------------+
| PCH              | Intel C246                                       |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel SPS (server version of the ME)             |
+------------------+--------------------------------------------------+
| Super I/O        | none                                             |
+------------------+--------------------------------------------------+
| BMC              | Aspeed AST2500                                   |
+------------------+--------------------------------------------------+
```

## Extra links

[flashrom]: https://flashrom.org/Flashrom
[flashing tutorial]: ../../../../tutorial/flashing_firmware/ext_power.md
[Intel FSP2.0]: ../../../../soc/intel/fsp/index.md
[AST2500]: https://www.aspeedtech.com/products.php?fPath=20&rId=440
