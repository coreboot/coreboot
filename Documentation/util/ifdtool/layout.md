# IFD Layout

A coreboot image for an Intel SoC contains two separate definitions of the
layout of the flash. The Intel Flash Descriptor (IFD) which defines offsets and
sizes of various regions of flash and the [coreboot FMAP](../../lib/flashmap.md).

The FMAP should define all of the of the regions defined by the IFD to ensure
that those regions are accounted for by coreboot and will not be accidentally
modified.

## IFD mapping

The names of the IFD regions in the FMAP should follow the convention of
starting with the prefix `SI_` which stands for `silicon initialization` as a
way to categorize anything required by the SoC but not provided by coreboot.

```eval_rst
+------------+------------------+-----------+-------------------------------------------+
| IFD Region | IFD Region name  | FMAP Name | Notes                                     |
| index      |                  |           |                                           |
+============+==================+===========+===========================================+
| 0          | Flash Descriptor | SI_DESC   | Always the top 4 KiB of flash             |
+------------+------------------+-----------+-------------------------------------------+
| 1          | BIOS             | SI_BIOS   | This is the region that contains coreboot |
+------------+------------------+-----------+-------------------------------------------+
| 2          | Intel ME         | SI_ME     |                                           |
+------------+------------------+-----------+-------------------------------------------+
| 3          | Gigabit Ethernet | SI_GBE    |                                           |
+------------+------------------+-----------+-------------------------------------------+
| 4          | Platform Data    | SI_PDR    |                                           |
+------------+------------------+-----------+-------------------------------------------+
| 8          | EC Firmware      | SI_EC     | Most ChromeOS devices do not use this    |
|            |                  |           | region; EC firmware is stored in BIOS     |
|            |                  |           | region of flash                           |
+------------+------------------+-----------+-------------------------------------------+
```

## Validation

The ifdtool can be used to manipulate a firmware image with a IFD. This tool
will not take into account the FMAP while modifying the image which can lead to
unexpected and hard to debug issues with the firmware image. For example if the
ME region is defined at 6 MiB in the IFD but the FMAP only allocates 4 MiB for
the ME, then when the ME is added by the ifdtool 6 MiB will be written which
could overwrite 2 MiB of the BIOS.

In order to validate that the FMAP and the IFD are compatible the ifdtool
provides --validate (-t) option. `ifdtool -t` will read both the IFD and the
FMAP in the image and for every non empty region in the IFD if that region is
defined in the FMAP but the offset or size is different then the tool will
return an error.

Example:

```console
foo@bar:~$ ifdtool -t bad_image.bin
Region mismatch between bios and SI_BIOS
 Descriptor region bios:
  offset: 0x00400000
  length: 0x01c00000
 FMAP area SI_BIOS:
  offset: 0x00800000
  length: 0x01800000
Region mismatch between me and SI_ME
 Descriptor region me:
  offset: 0x00103000
  length: 0x002f9000
 FMAP area SI_ME:
  offset: 0x00103000
  length: 0x006f9000
Region mismatch between pd and SI_PDR
 Descriptor region pd:
  offset: 0x003fc000
  length: 0x00004000
 FMAP area SI_PDR:
  offset: 0x007fc000
  length: 0x00004000
```
