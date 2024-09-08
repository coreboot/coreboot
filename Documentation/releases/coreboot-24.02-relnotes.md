coreboot 24.02 release
========================================================================

The coreboot project is happy to announce our release for February
2024. Over the past three months, our contributors have focused on
refining the coreboot codebase, generally prioritizing cleanup and
quality enhancements. We extend our gratitude to all the contributors
who have dedicated their time and expertise. Thank you for your
invaluable contributions to this vital phase of maintenance and
optimization.

The next release is scheduled for mid-May.


## Release number format update

The previous release was the last to use the incrementing 4.xx release
name scheme. For this and future releases, coreboot has switched to a
Year.Month.Sub-version naming scheme. As such, the next release,
scheduled for May of 2024 will be numbered 24.05, with the sub-version
of 00 implied. If we need to do a fix or incremental release, we'll
append the values .01, .02 and so on to the initial release value.


## The master branch is being deleted

The coreboot project changed from master to main roughly 6 months ago,
and has been keeping the two branches in sync since then to ease the
transition. As of this release, we are getting rid of the master branch
completely. Please make sure any scripts you're using that reference the
'master' branch have been switched to 'main'.


Release 24.02.1
---------------

### lib/rtc: Fix off-by-one error in February day count in leap year

The month argument passed to rtc\_month\_days is 0-based, not 1-based.
This results in the RTC being reverted to the build date constantly
on 29th February 2024.


Significant or interesting changes
----------------------------------

### acpi: Add Arm IO Remapping Table structures

Input Output Remapping Table (IORT) represents the IO topology of an Arm
based system.

Document number: ARM DEN 0049E.e, Sep 2022


### acpi: Add PPTT support

This patch adds code to generate Processor Properties Topology Tables
(PPTT) compliant to the ACPI 6.4 specification.

 - The 'acpi_get_pptt_topology' hook is mandatory once ACPI_PPTT is
   selected. Its purpose is to return a pointer to a topology tree,
   which describes the relationship between CPUs and caches. The hook
   can be provided by, for example, mainboard code.

Background: We are currently working on mainboard code for qemu-sbsa and
Neoverse N2. Both require a valid PPTT table. Patch was tested against
the qemu-sbsa board.


### acpi: Add support for WDAT table

This commit lays the groundwork for implementing the ACPI WDAT (Watchdog
Action Table) table specification. The WDAT is a special ACPI table
introduced by Microsoft that describes the watchdog for the OS.

Platforms that need to implement the WDAT table must describe the
hardware watchdog management operations as described in the
specification. See “Links to ACPI-Related Documents”
(http://uefi.org/acpi) under the heading “Watchdog Action Table”.


### lib/jpeg: Replace decoder with Wuffs' implementation

To quote its repo[0]: Wuffs is a memory-safe programming language (and a
standard library written in that language) for Wrangling Untrusted File
Formats Safely. Wrangling includes parsing, decoding and encoding.

It compiles its library, written in its own language, to a C/C++ source
file that can then be used independently without needing support for the
language. That library is now imported to src/vendorcode/wuffs/.

This change modifies our linters to ignore that directory because it's
supposed to contain the wuffs compiler's result verbatim.

Nigel Tao provided an initial wrapper around wuffs' jpeg decoder that
implements our JPEG API. I further changed it a bit regarding data
placement, dropped stuff from our API that wasn't ever used, or isn't
used anymore, and generally made it fit coreboot a bit better. Features
are Nigel's, bugs are mine.

This commit also adapts our jpeg fuzz test to work with the modified
API. After limiting it to deal only with approximately screen sized
inputs, it fuzzed for 25 hours CPU time without a single hang or crash.
This is a notable improvement over running the test with our old decoder
which crashes within a minute.

Finally, I tried the new parser with a pretty-much-random JPEG file I
got from the internet, and it just showed it (once the resolution
matched), which is also a notable improvement over the old decoder which
is very particular about the subset of JPEG it supports.

In terms of code size, a QEmu build's ramstage increases
from 128060 bytes decompressed (64121 bytes after LZMA)
  to 172304 bytes decompressed (82734 bytes after LZMA).

[0] <https://github.com/google/wuffs>



Additional coreboot changes
---------------------------

* Rename Makefiles from .inc to .mk to better identify them
* SPI: Add GD25LQ255E and IS25WP256D chip support
* device: Add support for multiple PCI segment groups
* device: Drop unused multiple downstream link support
* device: Rename bus and link_list to upstream and downstream
* Updated devicetree files for modern Intel platforms to use chipset.cb
* Updated xeon-sp to use the coreboot allocator



Changes to external resources
-----------------------------

### Toolchain updates
* Add buildgcc support for Apple M1/M2 devices
* Upgrade GCC from 11.4.0 to 13.2.0
* Update CMake from 3.26.4 to 3.27.7
* Uprev to Kconfig from Linux 6.7


### Git submodule pointers

* /3rdparty/amd_blobs: Update from commit id e4519efca7 to 64cdd7c8ef
  (5 commits)
* /3rdparty/arm-trusted-firmware: Update from commit id 88b2d81345 to
  17bef2248d (701 commits)
* /3rdparty/fsp: Update from commit id 481ea7cf0b to 507ef01cce (16 commits)
* /3rdparty/intel-microcode: Update from commit id 6788bb07eb to
  ece0d294a2 (1 commit)
* /3rdparty/vboot: Update from commit id 24cb127a5e to 3d37d2aafe
  (121 commits)


### External payloads
* payload/grub2: Update from 2.06 to 2.12
* payload/seabios: Update from 1.16.2 to 1.16.3



Platform Updates
----------------

### Added mainboards:
* Google: Dita
* Google: Xol
* Lenovo: ThinkPad X230 eDP Mod (2K/FHD)


### Removed Mainboards
* Google ->  Primus4ES



Statistics from the 4.22 to the 24.02 release
--------------------------------------------

* Total Commits: 815
* Average Commits per day: 8.63
* Total lines added: 105433
* Average lines added per commit: 129.37
* Number of patches adding more than 100 lines: 47
* Average lines added per small commit: 41.34
* Total lines removed: 16534
* Average lines removed per commit: 20.29
* Total difference between added and removed: 88899
* Total authors: 111
* New authors: 19



Significant Known and Open Issues
---------------------------------

* AMD chromebooks will not work with the signed PSP_verstage images and
  the version of verstage used in coreboot 24.02.


Issues from the coreboot bugtracker: <https://ticket.coreboot.org/>

### coreboot-wide or architecture-wide issues

```{eval-rst}
+-----+-----------------------------------------------------------------+
| #   | Subject                                                         |
+=====+=================================================================+
| 522 | 'region_overlap()' issues due to an integer overflow.           |
+-----+-----------------------------------------------------------------+
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
