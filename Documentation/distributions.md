# Distributions

coreboot doesn't provide binaries but provides a toolbox that others can use
to build boot firmware for all kinds of purposes. These third-parties can be
broadly separated in two groups: Those shipping coreboot on their hardware,
and those providing after-market firmware to extend the usefulness of devices.

## Shipping coreboot on hardware

### Purism

[Purism](https://www.puri.sm) sells laptops with a focus on privacy and
part of that is their push to remove as much unaccounted code (that is,
binary only) from their devices as possible.

### Chromebooks

All Chromebooks (and related devices) that hit the market after 2013 are
using coreboot as their main firmware. And even the Embedded Controller,
a small microcontroller to support various peripherals (like battery
management or the keyboard) is running open source firmware.


## After-market firmware

### Libreboot

[Libreboot](https://libreboot.org) is a project that provides ready-made
binaries for platforms where those can be built entirely from source
code. Their copy of the coreboot repository is therefore stripped of
all devices that require binary components to boot.

### Mr. Chromebox

[Matt Devo](https://mrchromebox.tech/) provides replacement firmware for
various Chromebooks. Why replace coreboot with coreboot?  You might want
to do different things than what the Google engineers prepared for the
mass market, that's why. This firmware is "with training wheels off".

### John Lewis

[John Lewis](https://johnlewis.ie/custom-chromebook-firmware) also
provides replacements for Chromebook firmware, for the same reasons
as Mr. Chromebox. It's a somewhat different set of devices, and with
different configurations, so check out both if Chromebooks are what
you're dealing with.
