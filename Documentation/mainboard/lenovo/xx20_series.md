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
* Update the EC firmware, as there's no support for EC updates in coreboot.
* Do **NOT** accidently swap pins or power on the board while a SPI flasher
  is connected. It will destroy your device.
* It's recommended to only flash the BIOS region. In that case you don't
  need to extract blobs from vendor firmware.
  If you want to flash the whole chip, you need blobs when building
  coreboot.
* The shipped *Flash layout* allocates 3MiB to the BIOS region, which is the space
  usable by coreboot.
* ROM chip size should be set to 8MiB.

```eval_rst
Please also have a look at :doc:`../../flash_tutorial/index`.
```

## Flash layout
There's one 8MiB flash which contains IFD, GBE, ME and BIOS regions.
On Lenovo's UEFI the EC firmware update is placed at the start of the BIOS
region. The update is then written into the EC once.

![][fl]

[fl]: flashlayout_xx20.svg

