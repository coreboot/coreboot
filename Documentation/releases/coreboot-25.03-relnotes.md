Upcoming release - coreboot 25.03
========================================================================

The coreboot project is pleased to announce the release of coreboot
25.03, marking another milestone in our ongoing work to delivering
open-source firmware. This release brings important improvements to
display handling, USB debugging capabilities, and CPU topology
management, along with various other enhancements that further improve
the reliability and performance of coreboot across supported platforms.

We extend our sincere thanks to all contributors who have made this
release possible. Your expertise and collaborative efforts continue to
propel the coreboot project forward. As always, we value the
participation of everyone in the community, from long-time developers to
those new to the project. The next coreboot release, 25.06, is scheduled
for the end of June 2025.



Significant or interesting changes
----------------------------------

### drivers/intel/fsp2_0: Enable panel-orientation aware bitmap rotation

Implement logo bitmap rotation within fsp_convert_bmp_to_gop_blt() to
support devices with portrait-oriented displays. The rotation is driven
by the panel framebuffer orientation, allowing the logo to be displayed
correctly regardless of physical panel orientation.

This resolves issues where the logo was displayed incorrectly on
portrait-oriented displays.

Additionally, discard the display orientation change if the LID is
closed aka built-in display is not active. This will ensure that display
orientation is proper when extended display is attached w/o any display
rotation.


### util/find_usbdebug: Fix lsusb -t parsing for usbutils v016 and newer

Commit e24294ff9ade ("lsusb -t: print ports and buses and devices with
same width") [1] in the usbutils repository changed the format of the
lsusb -t output, breaking the find_usbdebug.sh script. This commit is
present in usbutils version 016 and later.

Use the output of lsusb -V to set the parsing patterns based on the
version in order to maintain compatibility with older versions of
usbutils. A simple integer comparison of the version number is used for
this, which will not work with versions older than v001 as those use a
0.nn version number format. However, since v001 was released in late
2010, it is probably safe to assume that no one will be using a version
of usbutils older than that. Usbutils v016 was released in late 2023 so
there could still conceivably be systems using older versions, such as
Ubuntu 22.04 LTS which is on v014.

[1] https://github.com/gregkh/usbutils/commit/e24294ff9ade6dafcd1909763e888d97b377c841


### cpu/x86/topology: Fix FSP-S crash caused by shared core ID

This resolves a crash issue observed on Meteor Lake and introduced by
commit 70bdd2e1fad9fe89835aab240ed4b41a02f15078 ("cpu/x86/topology:
Simplify CPU topology initialization"). This commit simplifies the code
and provides more detailed CPU topology information by generalizing the
use of the Extended Topology Enumeration Leaves 0x1f. As a result, the
coreboot APIC core_id field does not provide the fully detailed path
information.

It turns out that the topology core identifier is used by the coreboot
MP service mp_get_processor_info() implementation. But the MP Service
EFI_CPU_PHYSICAL_LOCATION data structure only captures information about
the package, core, and thread. The core identifier returned to the MP
service caller must incorporate the full hierarchical path (die group,
die, module, tile, module and core).

This commit adds a new field to the cpu topology structure to represent
the core ID within the package.

For reference, here is that signature of the crash:

   LAPIC 0x40 in X2APIC mode.
   CPU Index 2 - APIC 64 Unexpected Exception:13 @ 10:69f3d1e4 - Halting
   Code: 0 eflags: 00010046 cr2: 00000000
   eax: 00000001 ebx: 69f313e8 ecx: 0000004e edx: 00000000
   edi: 69f38018 esi: 00000029 ebp: 69aeee0c esp: 69aeedc0
   [...]

The crash occurred when FSP attempted to lock the Protected
Processor Inventory Number Enable Control MSR (IA32_PPIN_CTL
0x4e).

   69f3d1d3:	8b 43 f4            	mov	-0xc(%ebx),%eax
   69f3d1d6:	89 4d c4            	mov	%ecx,-0x3c(%ebp)
   69f3d1d9:	89 45 dc            	mov	%eax,-0x24(%ebp)
   69f3d1dc:	8b 55 c4            	mov	-0x3c(%ebp),%edx
   69f3d1df:	8b 45 c0            	mov	-0x40(%ebp),%eax
   69f3d1e2:	8b 4d dc            	mov	-0x24(%ebp),%ecx
   69f3d1e5:	0f 30               	wrmsr
   69f3d1e7:	e9 ee fd ff ff      	jmp	0xfffffe39

FSP experiences issues due to attempting to lock the same register
multiple times for a single core. This is caused by an inconsistency in
the processor information data structure, where multiple cores share the
same identifier. This is not permitted and triggers a General Protection
Fault Exception.


### {drivers, lib}: Move low-battery user notification logic outside FSP

This patch refactors low-battery user notification logic (Kconfig, APIs
to check if low-battery rendering is required, low-battery shutdown is
required) outside FSP driver code to ensure in future non-FSP platforms
might still be able to leverage this feature/logics to render the
low-battery indicator icon during boot.

Specifically, it:

- Moves Kconfig options related to low-battery notifications from
  drivers/intel/fsp to lib/
- Relocates the low-battery check and shutdown APIs drivers/intel/fsp
  to bootsplash.h
* Adjusts the vendor driver to utilize the new APIs for low-battery
  rendering decisions.
* Drop the unwanted header file "fsp/api.h" from bmp_logo.c

This change avoids tight coupling of low-battery functionality to FSP,
promoting code reusability across platforms.


### soc/intel/cannonlake: Use common ACPI code for SRAM and HECI

Use the newly-created ACPI devices in common/acpi, and adjust the SoC
ACPI name for the CSE/HECI device to match.


### lib: Introduce early power off support Kconfig option

This commit introduces the `HAVE_EARLY_POWEROFF_SUPPORT` Kconfig option
and the `platform_do_early_poweroff()` API.

The Kconfig option enables platform-specific early power off support,
which is often required on Intel platforms. The corresponding API allows
platforms to implement the necessary hardware operations for early power
off, typically before memory initialization.



Additional coreboot changes
---------------------------

* Numerous changes to Haswell open source ram init
* Numerous additions to intelp2m tool
* Enhanced power management and thermal control across multiple platforms
* Improved USB Type-C and Thunderbolt support
* Added support for early power off and low battery detection
* Enhanced display and graphics support across multiple platforms
* Improved memory initialization and training
* Added support for various new memory parts and configurations
* Enhanced ACPI support and device handling
* Improved security features and TPM support
* Enhanced EC (Embedded Controller) support across platforms
* Added support for various new touch panels and input devices
* Refactored and improved code organization across multiple subsystems
* Enhanced build system and toolchain support
* Improved documentation and testing infrastructure
* Added support for RISC-V architecture improvements
* Enhanced debugging and logging capabilities
* Improved error handling and recovery mechanisms
* Added 7500 MT/s support for DDR5



Changes to external resources
-----------------------------

### Toolchain updates

* Update CMake from 3.30.2 to 3.31.3
* Update ACPICA from 20230628 to 20241212


### Git submodule pointers

* arm-trusted-firmware: Update from commit id 15e5c6c91d to e5a1f4abee (608 commits)
* blobs: Update from commit id 14f8fcc1b4 to a0726508b8 (10 commits)
* fsp: Update from commit id 851f7105d8 to 86c9111639 (30 commits)
* intel-microcode: Update from commit id 8ac9378a84 to 8a62de41c0 (1 commits)
* vboot: Update from commit id f1f70f46dc to 3f94e2c7ed (49 commits)



Platform Updates
----------------

### Added mainboards:
* AMD Crater for Renoir SoC
* ASROCK Z87 Extreme3
* ASROCK Z87 Extreme4
* ASROCK Z87M Extreme4
* ASROCK Z87 Pro4
* ASUS P8H67-I DELUXE
* Google Dirks
* Google Guren
* Google Meliks
* Google Moxie
* Google Ocelot
* Google Pujjoniru
* Google Quandiso2
* Google Wyrdeer
* HP Pro 3400 Series
* Intel Ptlrvp
* Lenovo ThinkCentre M900
* NovaCustom V540TU (14")
* NovaCustom V560TU (16")
* StarLabs StarLite Mk V Smart Battery (N200)
* StarLabs StarBook Mk VII (165H)
* StarLabs StarBook Mk VII (N200)

### Updated SoCs
* Added src/soc/xilinx/zynq7000



Statistics from the 24.12 to the 25.03 release
----------------------------------------------

* Total Commits: 992
* Average Commits per day: 10.09
* Total lines added: 87471
* Average lines added per commit: 88.18
* Number of patches adding more than 100 lines: 95
* Average lines added per small commit: 40.50
* Total lines removed: 22845
* Average lines removed per commit: 23.03
* Total difference between added and removed: 64626
* Total authors: 129
* New authors: 28



Significant Known and Open Issues
---------------------------------

### coreboot-wide or architecture-wide issues

+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 519 | make gconfig - could not find glade file                        |
+-----+-----------------------------------------------------------------+
| 518 | make xconfig - g++: fatal error: no input files                 |
+-----+-----------------------------------------------------------------+


### Payload-specific issues

+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 577 |  SeaBIOS/EDK2 Windows 10 BSOD "UNSUPPORTED PROCESSOR"           |
+-----+-----------------------------------------------------------------+
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


### Platform-specific issues

+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 579 | MAC address set by coreboot to RTL8111F does not persist        |
+-----+-----------------------------------------------------------------+
| 565 | Wifi card not recognized on Lenovo M700 tiny                    |
+-----+-----------------------------------------------------------------+
| 563 | tty doesn't show on external display using edk2 on W530         |
+-----+-----------------------------------------------------------------+
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



coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>

