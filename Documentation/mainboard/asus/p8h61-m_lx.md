# ASUS P8H61-M LX

This page describes how to run coreboot on the [ASUS P8H61-M LX].

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | yes        |
+---------------------+------------+
| Model               | W25Q32BV   |
+---------------------+------------+
| Size                | 4 MiB      |
+---------------------+------------+
| Package             | DIP-8      |
+---------------------+------------+
| Write protection    | no         |
+---------------------+------------+
| Dual BIOS feature   | no         |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

### Internal programming

The main SPI flash can be accessed using [flashrom]. By default, only
the BIOS region of the flash is writable. If you wish to change any
other region (Management Engine or flash descriptor), then an external
programmer is required.

The following command may be used to flash coreboot:

```
$ sudo flashrom --noverify-all --ifd -i bios -p internal -w coreboot.rom
```

The use of `--noverify-all` is required since the Management Engine
region is not readable even by the host.

## Known issues

- S3 suspend/resume does not work. This is the case for both coreboot
  and the vendor firmware, tested with Linux 4.9, Linux 4.17, and
  OpenBSD 6.3. Interestingly, it is possible to resume from S3 with
  Linux, but _only_ if the resume is started immediately after the
  suspend.

- There is no automatic, OS-independent fan control. This is because
  the Super I/O hardware monitor can only obtain valid CPU temperature
  readings from the PECI agent, whose complete initialisation is not
  publicly documented. The `coretemp` driver can still be used for
  accurate CPU temperature readings.

## Untested

- PCIe graphics
- parallel port
- PS/2 keyboard
- EHCI debug
- S/PDIF audio

## Working

- USB
- Gigabit Ethernet
- integrated graphics
- PCIe
- SATA
- PS/2 mouse
- serial port
- hardware monitor (see [Known issues](#known-issues) for caveats)
- onboard audio
- front panel audio
- native raminit (2 x 2GB, DDR3-1333)
- native graphics init (libgfxinit)
- flashrom under the vendor firmware
- flashrom under coreboot
- Wake-on-LAN
- Using `me_cleaner` (add `-S --whitelist EFFS,FCRS` if not using
  `me_cleaner` as part of the coreboot build process).

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x                                          |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6776                                  |
+------------------+--------------------------------------------------+
| EC               | None                                             |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

## Extra resources

- [Board manual]
- [Flash chip datasheet][W25Q32BV]

[ASUS P8H61-M LX]: https://www.asus.com/Motherboards/P8H61M_LX/
[W25Q32BV]: https://web.archive.org/web/20211002141814/https://www.winbond.com/resource-files/w25q32bv_revi_100413_wo_automotive.pdf
[flashrom]: https://flashrom.org/Flashrom
[Board manual]: http://dlcdnet.asus.com/pub/ASUS/mb/LGA1155/P8H61_M_LX/E6803_P8H61-M_LX.zip
