# ASUS P3B-F

This page describes how to run coreboot on the ASUS P3B-F mainboard.

## Flashing coreboot

```{eval-rst}
+---------------------+---------------------------+
| Type                | Value                     |
+=====================+===========================+
| Model               | SST 39SF020A (or similar) |
+---------------------+---------------------------+
| Protocol            | Parallel                  |
+---------------------+---------------------------+
| Size                | 256 KiB                   |
+---------------------+---------------------------+
| Package             | DIP-32                    |
+---------------------+---------------------------+
| Socketed            | yes                       |
+---------------------+---------------------------+
| Write protection    | See below                 |
+---------------------+---------------------------+
| Internal flashing   | yes                       |
+---------------------+---------------------------+
```

flashrom supports this mainboard since commit c7e9a6e15153684672bbadd1fc6baed8247ba0f6.
If you are using older versions of flashrom, below has to be done (with ACPI disabled!)
before flashrom can detect the flash chip:

```bash
  #  rmmod w83781d
  #  modprobe i2c-dev
  #  i2cset 0 0x48 0x80 0x80
```

Upon power up, flash chip is inaccessible until flashrom has been run once.
Since flashrom does not support reversing board enabling steps,
once it detects the flash chip, there will be no write protection until
the next power cycle.

### CPU microcode considerations

By default, this board includes microcode updates for 5 families of Intel CPUs
because of the wide variety of CPUs the board supports, directly or with an
adapter. These take up a third of the total flash space leaving only 20kB free
in the final cbfs image. It may be necessary to build a custom microcode update
file by manually concatenating files in 3rdparty/intel-microcode/intel-ucode
for only CPU models that the board will actually be run with.

## Working

- Slot 1 and Socket 370 CPUs and their L1/L2 caches
- PS/2 keyboard with SeaBIOS (See [Known issues])
- IDE hard drives
- USB
- PCI add-on cards
- AGP graphics cards
- Serial ports 1 and 2
- Reboot

## Known issues

- PS/2 keyboard may not be usable until Linux has completely booted. With SeaBIOS
  as payload, setting keyboard initialization timeout to 2500ms may help.

- The coreboot+SeaBIOS combination boots so quickly some IDE hard drives are not
  yet ready by the time SeaBIOS attempts to boot from them.

- i440BX does not support 256Mbit RAM modules. If installed, coreboot
  will attempt to initialize them at half their capacity anyway
  whereas vendor firmware will not boot at all.

- ECC memory can be used, but ECC support is still pending.

## Untested

- Floppy
- Parallel port
- EDO memory
- ECC memory
- Infrared
- PC speaker

## Not working

- ACPI (Support is currently [under gerrit review](https://review.coreboot.org/c/coreboot/+/41098))

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | Intel I440BX                                     |
+------------------+--------------------------------------------------+
| Southbridge      | i82371eb                                         |
+------------------+--------------------------------------------------+
| CPU              | P6 family for Slot 1 and Socket 370              |
|                  | (all models from model_63x to model_6bx)         |
+------------------+--------------------------------------------------+
| Super I/O        | winbond/w83977tf                                 |
+------------------+--------------------------------------------------+
```

## Extra resources

[flashrom]: https://flashrom.org/
