# ASUS P2B-LS

This page describes how to run coreboot on the ASUS P2B-LS mainboard.

## Variants

- P2B-LS
- P2B-L (Same circuit board with SCSI components omitted)
- P2B-S (Same circuit board with ethernet components omitted)

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
| Write protection    | no                        |
+---------------------+---------------------------+
| Dual BIOS feature   | no                        |
+---------------------+---------------------------+
| Internal flashing   | yes                       |
+---------------------+---------------------------+
```

[flashrom] works out of the box since 0.9.2.
Because of deficiency in vendor firmware, user needs to override the laptop
warning as prompted. Once coreboot is in place there will be no further issue.

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
- Ethernet (-LS, -L; Intel 82558)
- SCSI (-LS, -S; Adaptec AIC7890)
- USB
- ISA add-on cards
- PCI add-on cards
- AGP graphics card
- Floppy
- Serial ports 1 and 2
- Reboot
- Soft off

## Known issues

- PS/2 keyboard may not be usable until Linux has completely booted.
  With SeaBIOS as payload, setting keyboard initialization timeout to
  500ms may fix the issue.

- i440BX does not support 256Mbit RAM modules. If installed, coreboot
  will attempt to initialize them at half their capacity anyway
  whereas vendor firmware will not boot at all.

- ECC memory can be used, but ECC support is still pending.

- Termination is enabled for all SCSI ports (if equipped). Support to
  disable termination is pending. Note that the SCSI-68 port is
  always terminated, even with vendor firmware.

## Untested

- Parallel port
- EDO memory
- Infrared
- PC speaker

## Not working

- S3 suspend to RAM

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
