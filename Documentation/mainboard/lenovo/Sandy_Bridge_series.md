# Lenovo Sandy Bridge series

## Flashing coreboot
```eval_rst
+---------------------+--------------------+
| Type                | Value              |
+=====================+====================+
| Socketed flash      | no                 |
+---------------------+--------------------+
| Size                | 8 MiB              |
+---------------------+--------------------+
| In circuit flashing | Yes                |
+---------------------+--------------------+
| Package             | SOIC-8             |
+---------------------+--------------------+
| Write protection    | No                 |
+---------------------+--------------------+
| Dual BIOS feature   | No                 |
+---------------------+--------------------+
| Internal flashing   | Yes                |
+---------------------+--------------------+
```

## Installation instructions

Flashing coreboot for the first time needs to be done using an external
programmer, because vendor firmware prevents rewriting the BIOS region.

* Update the EC firmware, as there's no support for EC updates in coreboot.
* Do **NOT** accidentally swap pins or power on the board while a SPI flasher
  is connected. It will destroy your device.
* It's recommended to only flash the BIOS region. In that case you don't
  need to extract blobs from vendor firmware.
  If you want to flash the whole chip, you need blobs when building
  coreboot.
* The shipped *Flash layout* allocates 3MiB to the BIOS region, which is the space
  usable by coreboot.
* ROM chip size should be set to 8MiB.

Please also have a look at the [flashing tutorial]

## Flash layout
There's one 8MiB flash which contains IFD, GBE, ME and BIOS regions.
On Lenovo's UEFI the EC firmware update is placed at the start of the BIOS
region. The update is then written into the EC once.

![][fl]

[fl]: flashlayout_Sandy_Bridge.svg

## Reducing Intel Management Engine firmware size

It is possible to reduce the Intel ME firmware size to free additional
space for the `bios` region. This is usually referred to as *cleaning the ME* or
*stripping the ME*.
After reducing the Intel ME firmware size you must modify the original IFD
and then write a full ROM using an [external programmer].
Have a look at [me_cleaner] for more information.

Tests on Lenovo X220 showed no issues with a stripped ME firmware.

**Modified flash layout:**

![][fl2]

[fl2]: flashlayout_Sandy_Bridge_stripped_me.svg

The overall size of the `gbe`, `me,` `ifd` region is less than 128KiB, leaving
the remaining space for the `bios` partition.


[me_cleaner]: ../../northbridge/intel/sandybridge/me_cleaner.md
[external programmer]: ../../tutorial/flashing_firmware/index.md
[flashing tutorial]: ../../tutorial/flashing_firmware/index.md
