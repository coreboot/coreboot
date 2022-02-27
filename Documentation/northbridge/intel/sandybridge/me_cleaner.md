# ME Cleaner
It's possible to 'clean' the ME partition within the flash medium as part
of the build process. While cleaning as much code as possible is removed
from the ME firmware partition. In this state the ME errors out and doesn't
operate any more.

**Using a 'cleaned' ME partition may lead to issues and its use should be
carefully evaluated.**

## Observations with 'cleaned' ME

* Instable LPC bus
  * SuperIO is malfunctioning
  * TPM is malfunctioning
  * Random system shutdowns on high bus activity

## Filing bug reports

Always test with unmodified IFD and ME section before reporting bugs to the
coreboot project.

## Tutorial reducing the Intel ME firmware size

By default the cleaned ME firmware will still occupy the same space in
the firmware image. It's possible to change the firmware partition layout
and reclaim the space for the use by coreboot.
With the reduced Intel ME firmware the `ifd`, `gbe` and `me` regions require
less than 128 KiB of space in the ROM, which leaves the remaining for the
`bios` region.

This tutorial will guide you through the steps necessary.

### 1. Obtain a full ROM

You need a full and working ROM with a full Intel ME firmware.

### 2. Running me_cleaner

You need to run the *me_cleaner* on a full ROM, here called `fulldump.rom`:
The full ROM contains:
* IFD
* fully working Intel ME
* GbE (optional)
* BIOS (any firmware)

Running the command will generate two new files:
```console
./util/me_cleaner/me_cleaner.py -D patched_desciptor.bin -M stripped_me.bin fulldump.rom -t -r -S
```

The generated files are:
* a patched IFD called `patched_desciptor.bin`
* stripped Intel ME called `stripped_me.bin`

The patched IFD has the *AltMeDisable* bit set and a modified flash layout.


*Note:* coreboot allows to select `CONFIG_ME_CLEANER` as part of the
build-process, but that doesn't rework the flash layout, it only removes
files from ME and sets the *AltMeDisable*-bit.

### 3. Build coreboot

1. Now include the two new files from the previous step into coreboot's
   build system.
2. Make sure to also increase the CBFS size
   * 0x7E0000 for a 8MiB ROM
   * 0xBE0000 for a 12MiB ROM
   * 0xFE0000 for a 16MiB ROM
3. Make sure to **not** enable me_cleaner in Kconfig again as
   you have already run it

### 4. Flashing the ROM

As you have modified the layout you need to write the **full ROM** to flash
using an [external programmer].
Make sure to include all partitions into the ROM:
* IFD
* EC (might be unused)
* GbE (might be unused)
* ME
* BIOS

[external programmer]: ../../../tutorial/flashing_firmware/index.md
