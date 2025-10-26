# Lenovo ThinkPad T470s/T480/T480s/T580

This page describes how to run coreboot on the Lenovo [Thinkpad T470s], [ThinkPad T480],
[Thinkpad T480s], and [Thinkpad T580].

## Important Notes

### EC UART
The T480 (but not T480s) supports the use of the EC UART for debugging. If you want to use this
feature, you need to downgrade the vendor firmware to a version with a compatible EC firmware
*before flashing* for the EC UART to work properly. The EC firmware version needs to be 1.22
(N24HT37W), which means any BIOS from 1.39 (N24ET64W) to 1.54 (N24ET79W) is acceptable.

The mapping can be seen here [on Lenovo's site].

### Boot Guard
Most Thinkpads newer than the xx30 models except machines with swappable CPUs (T440p, W541) and
some models in the E series (E460, E470) have Intel Boot Guard enabled. This prevents these
laptops from running custom firmware which is not cryptographically signed with the manufacturer's
key.

The [deguard utility](../../soc/intel/deguard) will need to be used in order to bypass Boot Guard
on these devices.

### Thunderbolt

Older versions of Thunderbolt 3 controller firmware are buggy on these laptops.

The issue is that the TB3 firmware writes its logging data to the TB3's own 4M flash chip, and
when the flash chip fills up, the device fails to boot.

Lenovo has addressed this in a TB3 firmware update, which is recommended be be applied before
installing coreboot to ensure long-term reliability of the device. That said, most laptops that
still work likely have this fix already applied, and you can always externally flash the TB3
firmware update at the same time as flashing coreboot.

If your device becomes bricked due to this issue, the only resolution is to externally flash the
updated/fixed TB3 firmware flash (which is located on a separate flash IC from the main firmware).

The Lenovo TB3 FW update can be obtained [from Lenovo's site].

The Libreboot project has some more information about the issue and [how to externally flash the
TB3 firmware].

## Initial Setup

Follow the coreboot [tutorial](../../tutorial/part1) to:
* install dependencies/prerequisites
* clone the coreboot repo
* build the coreboot toolchain

You will also need to clone the [deguard](https://review.coreboot.org/deguard) repository in
order to bypass Boot Guard.

## Required proprietary blobs

When flashing the laptop for the first time, the following blobs are required for a full
flash image:
 * Flash Descriptor (IFD)
 * Management Engine (ME)
 * Gigabit Ethernet (GBE)

These will either be extracted from the vendor firmware on the device or downloaded as needed.

Additionally, all Skylake/Kabylake machines require Intel's Firmware Support Package (FSP) for
hardware initialization. FSP is provided by default as part of the coreboot build, no user
intervention is required.

## Reading the vendor firmware

The Skylake Thinkpads have a single BIOS flash chip (16 MiB serial flash in a SOIC-8 package).
The flash chip uses the following layout:

    00000000:00000fff flash descriptor (fd)
    00001000:00002fff gbe
    00003000:006fffff me
    00700000:00ffffff bios

To read the vendor firmware, disconnect external power and remove the back of the laptop
as described in the [hardware maintenance manual](thinkpad_hmm).
Disconnect/remove all batteries (and CMOS battery if equipped). Attach a chip clip to the flash.
Use an external SPI programmer (ch341a, raspberry Pi, etc) to read the chip `bios.bin`.

## Preparing the binaries

The IFD and GBE need to be extracted from the vendor firmware backup read from the flash chip.
See [Extract IFD binaries](../../util/ifdtool/binary_extraction).

    ifdtool -x -p sklkbl bios.bin

Rename the extracted files to `ifd.bin` and `gbe.bin` and move them to the `binaries` folder
you created per the instructions.

### Preparing the ME with deguard

Please review the documentation on the use of the [deguard utility](../../soc/intel/deguard).

All Skylake/Kabylake Thinkpads supported by coreboot are also supported in deguard. Hence, we
simply need to download/extract the donor image, then generate the "deguarded" ME firmware
image.

The donor ME binary required for use with the deguard utility can be downloaded as part of the
[Dell firmware updater]. After downloading, use [Dell_PFS_Extract.py] to extract the required ME
firmware image from the updater:

    python Dell_PFS_Extract.py Inspiron_5468_1.3.0.exe

The resulting binary should be renamed `me_donor.bin` and moved to the `binaries` folder with
the IFD and GBE binaries.

Then, generate the deguarded ME firmware image adjusting the `--delta` argument accordingly:

    ./finalimage.py --delta data/delta/thinkpad_t480 --version 11.6.0.1126 --pch LP --sku 2M --fake-fpfs data/fpfs/zero --input ../coreboot/binaries/me_donor.bin --output ../coreboot/binaries/me_deguarded.bin

The command above assumes you are running deguard from the root of the deguard repo, that the
deguard and coreboot repos are checked out under the same parent directory, and that your ME
donor firmware is in the `binaries` subdirectory of coreboot (along with the IFD/GBE binaries).
Adjust the paths as necessary if that is not the case.

Please be careful with [me_cleaner](../../northbridge/intel/sandybridge/me_cleaner). While
me_cleaner is generally expected to work, truncating the ME binary can have unintended side
effects like disabling PCIe devices on Thinkpad T470s (NVMe, WLAN, WWAN, etc.).

### Preparing the IFD

In order to use the modified ME firmware binary generated by deguard above, we need to set the
HAP bit in the IFD using `ifdtool`:

    util/ifdtool/ifdtool -p sklkbl -M 1 binaries/ifd.bin

The modified IFD will be saved as `ifd.bin.new`. Rename the original file to `ifd.bin.orig` and
the HAP-enabled one to `ifd.bin`

## Building coreboot

You can use `make menuconfig` or `make nconfig` to select the mainboard matching your machine,
enable the inclusion of the IFD/ME/GBE binaries, and select your payload and options.
For example, you can also just use the following defconfig for a Thinkpad T480 with EDK2/UEFI
as a payload:

    CONFIG_VENDOR_LENOVO=y
    CONFIG_BOARD_LENOVO_T480=y
    CONFIG_IFD_BIN_PATH="binaries/ifd.bin"
    CONFIG_ME_BIN_PATH="binaries/me_deguarded.bin"
    CONFIG_GBE_BIN_PATH="binaries/gbe.bin"
    CONFIG_HAVE_IFD_BIN=y
    CONFIG_HAVE_ME_BIN=y
    CONFIG_HAVE_GBE_BIN=y
    CONFIG_PAYLOAD_EDK2=y

Save the above to a file called `defconfig` in your coreboot directory, then run:

    make defconfig
    make -j"$(nproc)"

Upon successful compilation, you will have a file `coreboot.rom` in the `build` directory ready
to flash.

## Flashing instructions

The initial flash of coreboot with the modified IFD and deguarded ME must be done using an
external programmer:

    sudo flashrom -p <programmer> -w coreboot.rom

Subsequent flashes can be done internally and need only modify the `bios` region of the flash:

    sudo flashrom -p internal -w coreboot.rom --ifd -i bios -N

The `coreboot.rom` flashed on subsequent flashes does not need to contain the IFD/ME/GBE
binaries if only flashing the `bios` region.

## Known Issues

- Alpine Ridge Thunderbolt 3 controller does not work
  - Lower (right) USB-C port only works for charging/DP alt mode, not USB/PCIe data
- Some Fn+F{1-12} keys aren't handled correctly
- Nvidia dGPU is finicky
  - Needs option ROM
  - Power enable code is buggy
  - Proprietary driver does not work at all
  - Nouveau only works on linux 6.8-6.9
- Headphone jack detection
  - Both headphone jack and speakers work when manually selected via pulseaudio

## Verified Working

- Integrated graphics init with libgfxinit
- video output: internal (eDP), miniDP
- ACPI support
- keyboard and trackpoint
- SATA
- M.2 SATA SSD
- NVMe
- USB
- Ethernet
- WLAN
- WWAN
- Bluetooth
- Virtualization: VT-x and VT-d
- Internal flashing (after initial flash with unlocked IFD)

[Thinkpad T470s]: https://pcsupport.lenovo.com/us/en/products/laptops-and-netbooks/thinkpad-t-series-laptops/thinkpad-t470s/
[ThinkPad T480]: https://pcsupport.lenovo.com/us/en/products/laptops-and-netbooks/thinkpad-t-series-laptops/thinkpad-t480-type-20l5-20l6/
[ThinkPad T480s]: https://pcsupport.lenovo.com/us/en/products/laptops-and-netbooks/thinkpad-t-series-laptops/thinkpad-t480s-type-20l7-20l8/
[Thinkpad T580]: https://pcsupport.lenovo.com/us/en/products/laptops-and-netbooks/thinkpad-t-series-laptops/thinkpad-t580-type-20l9-20la/
[on Lenovo's site]: https://support.lenovo.com/us/en/downloads/ds502355-bios-update-utility-bootable-cd-for-windows-10-64-bit-linux-thinkpad-t480
[from Lenovo's site]: https://pcsupport.lenovo.com/gb/en/products/laptops-and-netbooks/thinkpad-t-series-laptops/thinkpad-t480s-type-20l7-20l8/solutions/ht508988-critical-intel-thunderbolt-software-and-firmware-updates-thinkpad
[how to externally flash the TB3 firmware]: https://libreboot.org/docs/install/t480.html#thunderbolt-issue-read-this-before-flashing
[Dell firmware updater]: https://web.archive.org/web/20241110222323/https://dl.dell.com/FOLDER04573471M/1/Inspiron_5468_1.3.0.exe
[Dell_PFS_Extract.py]: https://github.com/vuquangtrong/Dell-PFS-BIOS-Assembler/blob/master/Dell_PFS_Extract.py
