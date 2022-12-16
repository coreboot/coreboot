# Distributions

coreboot doesn't provide binaries but provides a toolbox that others can use
to build boot firmware for all kinds of purposes. These third-parties can be
broadly separated in two groups: Those shipping coreboot on their hardware,
and those providing after-market firmware to extend the usefulness of devices.


## Hardware shipping with coreboot

### NovaCustom laptops

[NovaCustom](https://configurelaptop.eu/) sells configurable laptops with
[Dasharo](https://dasharo.com/) coreboot based firmware on board, maintained by
[3mdeb](https://3mdeb.com/). NovaCustom offers full GNU/Linux and Microsoft
Windows compatibility. NovaCustom ensures security updates via fwupd for 5 years
and the firmware is equipped with important security features such as measured
boot, verified boot, TPM integration and UEFI Secure Boot.

### ChromeOS Devices

All ChromeOS devices ([Chromebooks](https://chromebookdb.com/), Chromeboxes,
Chromebit, etc) released from 2012 onward use coreboot for their main system
firmware. Additionally, starting with the 2013 Chromebook Pixel, the firmware
running on the Embedded Controller (EC) – a small microcontroller which provides
functions like battery management, keyboard support, and sensor interfacing –
is open source as well.

### PC Engines APUs

[PC Engines](https://pcengines.ch) designs and sells embedded PC hardware that
ships with coreboot and support upstream maintenance for the devices through a
third party, [3mdeb](https://3mdeb.com). They provide current and tested
firmware binaries on [GitHub](https://pcengines.github.io).

### Star Labs

[Star Labs](https://starlabs.systems/) offers a range of laptops designed and
built specifically for Linux that are available with coreboot firmware. They
use edk2 as the payload and include an NVRAM option to disable the Intel
Management Engine.

### System76

[System76](https://system76.com/) manufactures Linux laptops, desktops, and
servers. Some models are sold with [System76 Open
Firmware](https://github.com/system76/firmware-open), an open source
distribution of coreboot, edk2, and System76 firmware applications.

### Purism

[Purism](https://www.puri.sm) sells laptops with a focus on user privacy and
security; part of that effort is to minimize the amount of proprietary and/or
binary code. Their laptops ship with a blob-free OS and coreboot firmware
with a neutralized Intel Management Engine (ME) and SeaBIOS as the payload.

## After-market firmware

### Libreboot

[Libreboot](https://libreboot.org) is a downstream coreboot distribution that
provides ready-made firmware images for supported devices: those which can be
built entirely from source code. Their copy of the coreboot repository is
therefore stripped of all devices that require binary components to boot.


### Dasharo

[Dasharo](https://dasharo.com/) is an open-source based firmware distribution
focusing on clean and simple code, long-term maintenance, transparent
validation, privacy-respecting implementation, liberty for the owners, and
trustworthiness for all.

Contributions are welcome,
[this document](https://docs.dasharo.com/ways-you-can-help-us/).

### MrChromebox

[MrChromebox](https://mrchromebox.tech/) provides upstream coreboot firmware
images for the vast majority of x86-based Chromebooks and Chromeboxes, using
edk2 as the payload to provide a modern UEFI bootloader. Why replace
coreboot with coreboot? Mr Chromebox's images are built using upstream
coreboot (vs Google's older, static tree/branch), include many features and
fixes not found in the stock firmware, and offer much broader OS compatibility
(i.e., they run Windows as well as Linux). They also offer updated CPU
microcode, as well as firmware updates for the device's embedded controller
(EC). This firmware "takes the training wheels off" your ChromeOS device :)

### Heads

[Heads](http://osresearch.net) is an open source custom firmware and OS
configuration for laptops and servers that aims to provide slightly better
physical security and protection for data on the system. Unlike
[Tails](https://tails.boum.org/), which aims to be a stateless OS that leaves
no trace on the computer of its presence, Heads is intended for the case where
you need to store data and state on the computer.

Heads is not just another Linux distribution – it combines physical hardening
of specific hardware platforms and flash security features with custom coreboot
firmware and a Linux boot loader in ROM.

### Skulls

[Skulls](https://github.com/merge/skulls) provides firmware images for
laptops like the Lenovo Thinkpad X230. It uses upstream coreboot, an easy
to use payload like SeaBIOS and Intel's latest microcode update.

It simplifies installation and includes compact documentation. Skulls also
enables easy switching to [Heads](#heads) and back.
