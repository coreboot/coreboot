Upcoming release - coreboot 24.08
========================================================================

We are pleased to announce the release of coreboot 24.08, another significant
milestone in our ongoing commitment to delivering open-source firmware
solutions. This release includes over 900 commits, contributed by more than 130
dedicated individuals from our global community. The updates in 24.08 bring
various enhancements, optimizations, and new features that further improve the
reliability and performance of coreboot across supported platforms.

We extend our sincere thanks to the patch authors, reviewers, and everyone
involved in the coreboot community for their hard work and dedication. Your
contributions continue to advance and refine coreboot with each release. As
always, thank you for your support and collaboration in driving the future of
open-source firmware. The next coreboot release, 24.11 is planned for mid
November.



Significant or interesting changes
----------------------------------

### Introduce region_create() functions

We introduce two new functions to create region objects. They allow us to check
for integer overflows (`region_create_untrusted()`) or assert their absence
(`region_create()`).

This fixes potential overflows in `region_overlap()` checks in SMI handlers, where
we would wrongfully report MMIO as *not* overlapping SMRAM.

Also, two cases of `strtol()` in `parse_region()` (cbfstool), where the results were
implicitly converted to `size_t`, are replaced with the unsigned `strtoul()`.

FIT payload support is left out, as it doesn't use the region API (only the
struct).

Ticket: <https://ticket.coreboot.org/issues/522> \
Review: <https://review.coreboot.org/79905>


### lib/device_tree: Add some FDT helper functions

This adds some helper functions for FDT (Flattened Device Tree) , since more and
more mainboards seem to need FDT nowadays. For example our QEMU boards need it
in order to know how much RAM is available. Also all RISC-V boards in our tree
need FDT.

This also adds some tests in order to test said functions.

Review: <https://review.coreboot.org/c/coreboot/+/81081>


### device_tree: Add function to get top of memory from a FDT blob

coreboot needs to figure out top of memory to place CBMEM data. On some non-x86
QEMU virtual machines, this is achieved by probing the RAM space to find where
the VM starts discarding data since it's not backed by actual RAM. This behavior
seems to have changed on the QEMU side since then, VMs using the "virt" model
have started raising exceptions/errors instead of silently discarding data
(likely [1] for example) which has previously broken coreboot on these emulation
boards.

The qemu-aarch64 and qemu-riscv mainboards are intended for the "virt" models
and had this issue, which was mostly fixed by using exception handlers in the
RAM detection process [2][3]. But on 32-bit RISC-V we fail to initialize CBMEM
if we have 2048 MiB or more of RAM, and on 64-bit RISC-V we had to limit probing
to 16383 MiB because it can run into MMIO regions otherwise.

The qemu-armv7 mainboard code is intended for the "vexpress-a9" model VM which
doesn't appear to suffer from this issue. Still, the issue can be observed on
the ARMv7 "virt" model via a port based on qemu-aarch64.

QEMU docs for ARM and RISC-V "virt" models [4][5] recommend reading the device
tree blob it provides for device information (incl. RAM size). Implement
functions that parse the device tree blob to find described memory regions and
calculate the top of memory in order to use it in mainboard code as an
alternative to probing RAM space. ARM64 code initializes CBMEM in romstage where
malloc isn't available, so take care to do parsing without unflattening the blob
and make the code available in romstage as well.

[1] <https://lore.kernel.org/qemu-devel/1504626814-23124-1-git-send-email-peter.maydell@linaro.org/T/#u> \
[2] <https://review.coreboot.org/c/coreboot/+/34774> \
[3] <https://review.coreboot.org/c/coreboot/+/36486> \
[4] <https://qemu-project.gitlab.io/qemu/system/arm/virt.html> \
[5] <https://qemu-project.gitlab.io/qemu/system/riscv/virt.html>

Review: <https://review.coreboot.org/c/coreboot/+/80322>


### drivers/wifi: Support Bluetooth Regulator Domain Settings

The 'Bluetooth Increased Power Mode - SAR Limitation' feature provides ability
to utilize increased device Transmit power capability for Bluetooth applications
in coordination with Wi-Fi adhering to product SAR (Specific Absorption Rate)
limit when Bluetooth and Wi-Fi run together.

This commit introduces a `bluetooth_companion' field to the generic Wi-Fi
drivers chip data. This field can be set in the board design device tree to
supply the bluetooth device for which the BRDS function must be created.

The implementation follows document 559910 Intel Connectivity Platforms BIOS
Guideline revision 8.3 specification.

Review: <https://review.coreboot.org/c/coreboot/+/83200>


### acpigen_ps2_keybd: Support Do Not Disturb & Accessibility Keys

These commits add support for a Do Not Disturb key and an Accessibility key.

HUTRR94 added support for a new usage titled "System Do Not Disturb" which
toggles a system-wide Do Not Disturb setting.

HUTRR116 added support for a new usage titled "System Accessibility Binding"
which toggles a system-wide bound accessibility UI or command.

HUTRR94: <https://www.usb.org/sites/default/files/hutrr94_-_system_do_not_disturb.pdf> \
HUTRR116: <https://www.usb.org/sites/default/files/hutrr116-systemaccessbilitybinding_2.pdf>

Review: <https://review.coreboot.org/c/coreboot/+/82997> \
Review: <https://review.coreboot.org/c/coreboot/+/82996>


### superio/ite/common: Add common driver for GPIO and LED configuration

Add a generic driver to configure GPIOs and LEDs on common ITE SuperIOs. The
driver supports most ITE SuperIOs, except Embedded Controllers. The driver
allows configuring every GPIO property with pin granularity.

Verified against datasheets of all ITE SIOs currently supported by coreboot,
except IT8721F (assumed to be the same as IT8720F), IT8623E and IT8629E.

Review: <https://review.coreboot.org/c/coreboot/+/83355>


### util/cbfstool: Fix linux_trampoline.c generation

linux_trampoline.c generation is broken with latest crossgcc-i386 toolchain. Fix
the issue to enable the building.

```
../cbfstool/linux_trampoline.S: Assembler messages:
../cbfstool/linux_trampoline.S:100: Error: no instruction mnemonic
	suffix given and no register operands; can't size instruction
<builtin>: recipe for target '../cbfstool/linux_trampoline.o' failed
```

Review: <https://review.coreboot.org/c/coreboot/+/82704>


### Add LeanEFI payload

This adds another external payload to coreboot. The payload has been heavily
based on u-boots UEFI implementation.

The LeanEFI payload is basically a translator from coreboot to UEFI. It takes
the coreboot tables and transforms them into UEFI interfaces. Although it can
potentially load any efi application that can handle the minimized interface
that LeanEFI provides, it has only been tested with LinuxBoot (v6.3.5) as a
payload. It has been optimized to support only those interfaces that Linux
requires to start.

Among other LeanEFI does not support:
- efi capsule update (also efi system resource table)
- efi variables
- efi text input protocol (it can only output)
- most boot services. mostly memory services are left (e.g. alloc/free)
- all runtime services (although there is still a very small runtime
  footprint that is planned to be removed in the near future)
- TCG2/TPM (although that is mostly because of laziness)
The README.md currently provides more details on why.

The payload currently only supports arm64 and has only been tested on
emulation/simulator targets. The original motivation was to get ACPI on arm64
published to the OS without using EDK2. It is however also possible to supply
the LeanEFI with a FDT that is published to the OS. At that point one would
however probably use coreboot only instead of this shim layer on top. It would
be way nicer to have Linux support something other than UEFI to propagate the
ACPI tables, but it requires getting the Linux maintainer/community on board. So
for now this shim layer circumvents that.

LBBR Test:
1. dump FDT from QEMU like mentioned in aarch64 coreboot doc
2. compile u-root however you like (aarch64)
3. compile Linux (embed u-root initramfs via Kconfig)
4. copy Linux kernel to payloads/leanefi/Image
5. copy following coreboot defconfig to configs/defconfig:
```
CONFIG_BOARD_EMULATION_QEMU_AARCH64=y
CONFIG_PAYLOAD_NONE=n
CONFIG_PAYLOAD_LEANEFI=y
CONFIG_LEANEFI_PAYLOAD=y
CONFIG_LEANEFI_PAYLOAD_PATH="[path-to-linux]/arch/arm64/boot/Image"
CONFIG_LEANEFI_FDT=y
CONFIG_LEANEFI_FDT_PATH="[path-to-dumped-DTB]"
```
6. compile coreboot \
`make defconfig` \
`make -j$(nproc)`
7. run qemu like mentioned in coreboot doc (no FIT)
8. say hello to u-root and optionally kexec into the next kernel

Review: <https://review.coreboot.org/c/coreboot/+/78913>



Additional coreboot changes
---------------------------

* Dropped ChromeEC as a submodule.
* Numerous updates to autoport tool, including Haswell support.
* Upgrade to Wuffs 0.4.0-alpha.8
* Add x86_64 (64-bit) support to LibPayload
* Add hda-decoder utility that dumps decoded HDA default configuration registers
* Add SBMIOS tables for arm64 platforms
* cpu/x86/lapic: Always have LAPIC enabled
* arch/arm64: Support calling a trusted monitor
* drivers/wifi: Support Wi-Fi 7 11be Enablement
* drivers/wifi: Support Radio Frequency Interference Mitigation



Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade CMake from 3.28.3 to 3.29.3
* Upgrade nasm from 2.16.01 to 2.16.03
* Upgrade LLVM from 17.0.6 to 18.1.6
* Upgrade GCC from 13.2 to 14.1.0


### Git submodule pointers

* /3rdparty/amd_blobs: Update from commit ae5fc7d277 to 26c572974b (2 commits)
* /3rdparty/arm-trusted-firmware: Update from commit 48f1bc9f52 to c5b8de86c8 (430 commits)
* /3rdparty/fsp: Update from commit cc6399e8c7 to 800c85770b (23 commits)
* /3rdparty/intel-microcode: Update from commit 41af345005 to 5278dfcf98 (2 commits)
* /3rdparty/libgfxinit: Update from commit a4be8a21b0 to 17cfc92f40 (5 commits)
* /3rdparty/vboot: Update from commit 09fcd2184f to f1f70f46dc (69 commits)


Platform Updates
----------------

### New mainboards:

* Acer Q45T-AM
* AOOSTAR WTR R1
* ASROCK Fatal1ty Z87 Professional
* ASROCK Z87E-ITX
* ASROCK Z87M OC Formula
* ASROCK Z97E-ITX/ac
* CWWK CW-ADL-4L-V1.0
* Dell Inc. Latitude E6430
* Dell Inc. Latitude E7240
* Dell Inc. XPS 8300
* Emulation QEMU sbsa
* GIGABYTE GA-H61M-S2P-R3
* Google Awasuki
* Google Brox TI PDC
* Google Domika
* Google Fatcat
* Google Jubilant
* Google Orisa
* Google Rauru
* Google Rex 64
* Google Teliks
* Google Tereid
* HP EliteBook 8560w
* Intel Avenue City CRB
* Intel Beechnut City CRB
* Protectli VP6630/VP6650/VP6670
* Star Labs Star Labs Lite Mk V (N200)
* System76 addw4
* System76 darp10
* System76 darp10-b
* System76 oryp12


### Updated SoCs & socket definitions

* Added src/cpu/intel/socket_LGA1700
* Added src/cpu/intel/socket_LGA3647_1
* Added src/cpu/intel/socket_LGA4189
* Added src/cpu/intel/socket_LGA4677
* Added src/soc/intel/pantherlake
* Added src/soc/mediatek/mt8196



Statistics from the 24.05 to the 24.08 release
----------------------------------------------

* Total Commits: 883
* Average Commits per day: 8.78
* Total lines added: 105457
* Average lines added per commit: 119.43
* Number of patches adding more than 100 lines: 98
* Average lines added per small commit: 37.67
* Total lines removed: 18689
* Average lines removed per commit: 21.17
* Total difference between added and removed: 86768
* Total authors: 136
* New authors: 33



Significant Known and Open Issues
---------------------------------

### coreboot-wide or architecture-wide issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 519 | make gconfig - could not find glade file                        |
+-----+-----------------------------------------------------------------+
| 518 | make xconfig - g++: fatal error: no input files                 |
+-----+-----------------------------------------------------------------+
```


### Payload-specific issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 552 |  X201 not booting with edk2 payload                             |
+-----+-----------------------------------------------------------------+
| 549 | SeaBIOS Windows 10/11 BSOD "ACPI BIOS ERROR" (Thinkpad W530)    |
+-----+-----------------------------------------------------------------+
| 499 | edk2 boot fails with RESOURCE_ALLOCATION_TOP_DOWN enabled       |
+-----+-----------------------------------------------------------------+
| 496 | Missing malloc check in libpayload                              |
+-----+-----------------------------------------------------------------+
| 484 | No USB keyboard support with secondary payloads                 |
+-----+-----------------------------------------------------------------+
| 414 | X9SAE-V: No USB keyboard init on SeaBIOS using Radeon RX 6800XT |
+-----+-----------------------------------------------------------------+
```


### Platform-specific issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 548 | Lenovo X201 Fails To Recognize Upgraded WiFi Card               |
+-----+-----------------------------------------------------------------+
| 538 | x230: Dock Causes Internal Display to "Permanently" Malfunction |
+-----+-----------------------------------------------------------------+
| 535 | T420: Power light stays off after reboot                        |
+-----+-----------------------------------------------------------------+
| 528 | Building qemu-i440fx with CONFIG_CBFS_VERIFICATION fails        |
+-----+-----------------------------------------------------------------+
| 524 | X2APIC Options cause Linux to crash on emulation/qemu-i440fx    |
+-----+-----------------------------------------------------------------+
| 517 | lenovo x230 boot stuck with connected external monitor          |
+-----+-----------------------------------------------------------------+
| 509 | SD Card hotplug not working on Apollo Lake                      |
+-----+-----------------------------------------------------------------+
| 507 | Windows GPU driver fails on Google guybrush & skyrim boards     |
+-----+-----------------------------------------------------------------+
| 506 | APL/GML don't boot OS when CPU microcode included "from tree"   |
+-----+-----------------------------------------------------------------+
| 505 | Harcuvar CRB - 15 of 16 cores present in the operating system   |
+-----+-----------------------------------------------------------------+
| 499 | T440p - EDK2 fails with RESOURCE_ALLOCATION_TOP_DOWN enabled    |
+-----+-----------------------------------------------------------------+
| 495 | Stoney Chromebooks not booting PSPSecureOS                      |
+-----+-----------------------------------------------------------------+
| 478 | X200 booting Linux takes a long time with TSC                   |
+-----+-----------------------------------------------------------------+
| 474 | X200s crashes after graphic init with 8GB RAM                   |
+-----+-----------------------------------------------------------------+
| 457 | Haswell (t440p): CAR mem region conflicts with CBFS_SIZE > 8mb  |
+-----+-----------------------------------------------------------------+
| 453 | Intel HDMI / DP Audio not present in Windows after libgfxinit   |
+-----+-----------------------------------------------------------------+
| 449 | ThinkPad T440p fail to start, continuous beeping & LED blinking |
+-----+-----------------------------------------------------------------+
| 448 | Thinkpad T440P ACPI Battery Value Issues                        |
+-----+-----------------------------------------------------------------+
| 446 | Optiplex 9010 No Post                                           |
+-----+-----------------------------------------------------------------+
| 439 | Lenovo X201 Turbo Boost not working (stuck on 2,4GHz)           |
+-----+-----------------------------------------------------------------+
| 427 | x200: Two battery charging issues                               |
+-----+-----------------------------------------------------------------+
| 412 | x230 reboots on suspend                                         |
+-----+-----------------------------------------------------------------+
| 393 | T500 restarts rather than waking up from suspend                |
+-----+-----------------------------------------------------------------+
| 350 | I225 PCIe device not detected on Harcuvar                       |
+-----+-----------------------------------------------------------------+
```


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
