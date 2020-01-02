# ASUS P5Q

This page describes how to run coreboot on the [ASUS P5Q] desktop board.

## TODO

The following things are working in this coreboot port:

+ PCI slots
+ PCI-e slots
+ Onboard Ethernet
+ USB
+ Onboard sound card
+ PS/2 keyboard
+ All 4 DIMM slots
+ S3 suspend and resume
+ Red SATA ports

The following things are still missing from this coreboot port:

+ PS/2 mouse support
+ PATA aka IDE (because of buggy IDE controller)
+ Fan control (will be working on 100% power)
+ TPM module (support not implemented)

The following things are untested on this coreboot port:

+ S/PDIF
+ CD Audio In
+ Floppy disk drive
+ FireWire: PCI device shows up and driver loads, no further test


## Flashing coreboot

```eval_rst
+-------------------+----------------+
| Type              | Value          |
+===================+================+
| Socketed flash    | Yes            |
+-------------------+----------------+
| Model             | MX25L8005      |
+-------------------+----------------+
| Size              | 1 MiB          |
+-------------------+----------------+
| Package           | Socketed DIP-8 |
+-------------------+----------------+
| Write protection  | No             |
+-------------------+----------------+
| Dual BIOS feature | No             |
+-------------------+----------------+
| Internal flashing | Yes            |
+-------------------+----------------+
```

You can flash coreboot into your motherboard using [this guide].

## Technology

```eval_rst
+------------------+---------------------------------------------------+
| Northbridge      | Intel P45 (called x4x in coreboot code)           |
+------------------+---------------------------------------------------+
| Southbridge      | Intel ICH10R (called i82801jx in coreboot code)   |
+------------------+---------------------------------------------------+
| CPU (LGA775)     | Model f4x, f6x, 6fx, 1067x (Pentium 4, d, Core 2) |
+------------------+---------------------------------------------------+
| SuperIO          | Winbond W83667HG                                  |
+------------------+---------------------------------------------------+
| Coprocessor      | No                                                |
+------------------+---------------------------------------------------+
| Clockgen (CK505) | ICS 9LPRS918JKLF                                  |
+------------------+---------------------------------------------------+
```

[ASUS P5Q]: https://www.asus.com/Motherboards/P5Q
[this guide]: https://doc.coreboot.org/flash_tutorial/int_flashrom.html
