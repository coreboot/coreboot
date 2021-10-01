# Flashing firmware tutorial

Updating the firmware is possible using the **internal method**, where the updates
happen from a running system, or using the **external method**, where the system
is in a shut down state and an external programmer is attached to write into the
flash IC.

## Contents

* [Flashing internally](int_flashrom.md)
* [Flashing firmware standalone](ext_standalone.md)
* [Flashing firmware externally supplying direct power](ext_power.md)
* [Flashing firmware externally without supplying direct power](no_ext_power.md)

## General advice

* It's recommended to only flash the BIOS region.
* Always verify the firmware image.
* If you flash externally and have transmission errors:
  * Use short wires
  * Reduce clock frequency
  * Check power supply
  * Make sure that there are no other bus masters (EC, ME, SoC, ...)

## Internal method

This method using [flashrom] is available on many platforms, as long as they
aren't locked down.

There are various protection schemes that make it impossible to modify or
replace a firmware from a running system. coreboot allows to disable these
mechanisms, making it possible to overwrite (or update) the firmware from a
running system.

Usually you must use the **external method** once to install a retrofitted
coreboot and then you can use the **internal method** for future updates.

There are multiple ways to update the firmware:
* Using flashrom's *internal* programmer to directly write into the firmware
  flash IC, running on the target machine itself
* A proprietary software to update the firmware, running on the target machine
  itself
* A UEFI firmware update capsule

More details on flashrom's
* [internal programmer](int_flashrom.md)

## External method

External flashing is possible on many platforms, but requires disassembling
the target hardware. You need to buy a flash programmer, that
exposes the same interface as your flash IC (likely SPI).

Please also have a look at the mainboard-specific documentation for details.

After exposing the firmware flash IC, read the schematics and use one of the
possible methods:

* [Flashing firmware standalone](ext_standalone.md)
* [Flashing firmware externally supplying direct power](ext_power.md)
* [Flashing firmware externally without supplying direct power](no_ext_power.md)

**WARNING:** Using the wrong method or accidentally using the wrong pinout might
  permanently damage your hardware!

**WARNING:** Do not rely on dots *painted* on flash ICs to orient the pins!
Any dots painted on flash ICs may only indicate if they've been tested.  Dots
that appear in datasheets to indicate pin 1 correspond to some kind of physical
marker, such as a drilled hole, or one side being more flat than the other.

## Using a layout file
On platforms where the flash IC is shared with other components you might want
to write only a part of the flash IC. On Intel for example there are IFD, ME and
GBE which don't need to be updated to install coreboot.
To make [flashrom] only write the *bios* region, leaving Intel ME and Intel IFD
untouched, you can use a layout file, which can be created with ifdtool and a backup
of the original firmware.

```bash
ifdtool -f rom.layout backup.rom
```

and looks similar to:

```
00000000:00000fff fd
00500000:00bfffff bios
00003000:004fffff me
00001000:00002fff gbe
```

By specifying *-l* and *-i* [flashrom] writes a single region:
```bash
flashrom -l rom.layout -i bios -w coreboot.rom -p <programmer>
```

## Using an IFD to determine the layout
flashrom version 1.0 supports reading the layout from the IFD (first 4KiB of
the ROM). You don't need to manually specify a layout it, but it only works
under the following conditions:

* Only available on Intel ICH7+
* There's only one flash IC when flashing externally

```bash
flashrom --ifd -i bios -w coreboot.rom -p <programmer>
```

**TODO** explain FMAP regions, normal/fallback mechanism, flash lock mechanisms

[flashrom]: https://www.flashrom.org/Flashrom
