coreboot 4.16
========================================================================

The 4.16 release was done on February 25th, 2022.

Since 4.15 there have been more than 1770 new commits by more than 170
developers.  Of these, more than 35 contributed to coreboot for the
first time.

Welcome to the project!

Thank you to all the developers who continue to make coreboot the
great open source firmware project that it is.

New mainboards:
---------------
* Acer Aspire VN7-572G
* AMD Chausie
* ASROCK H77 Pro4-M
* ASUS P8Z77-M
* Emulation QEMU power9
* Google Agah
* Google Anahera4ES
* Google Banshee
* Google Beadrix
* Google Brya4ES
* Google Crota
* Google Dojo
* Google Gimble4ES
* Google Herobrine_Rev0
* Google Kingler
* Google Kinox
* Google Krabby
* Google Moli
* Google Nereid
* Google Nivviks
* Google Primus4ES
* Google Redrix4ES
* Google Skyrim
* Google Taeko4ES
* Google Taniks
* Google Vell
* Google Volmar
* Intel Alderlake-N RVP
* Prodrive Atlas
* Star Labs Star Labs StarBook Mk V (i3-1115G4 and i7-1165G7)
* System76 gaze16 3050
* System76 gaze16 3060
* System76 gaze16 3060-b

Removed mainboards:
-------------------
* Google ->  Corsola
* Google ->  Nasher
* Google ->  Stryke

Added processors:
-----------------
* src/cpu/power9
* src/soc/amd/sabrina

Submodule Updates
-----------------
* /3rdparty/amd_blobs (6 commits)
* /3rdparty/arm-trusted-firmware (965 commits)
* /3rdparty/blobs (30 commits)
* /3rdparty/chromeec (2212 commits)
* /3rdparty/intel-microcode (1 commits)
* /3rdparty/qc_blobs (13 commits)
* /3rdparty/vboot (44 commits)

Plans to move platform support to a branch:
-------------------------------------------
After the 4.18 release in November 2022, we plan to move support for any
boards still requiring RESOURCE_ALLOCATOR_V3 to the 4.18 branch.  V4 was
introduced more than a year ago and with minor changes most platforms
were able to work just fine with it. A major difference is that V3 uses
just one continuous region below 4G to allocate all PCI memory BAR's. V4
uses all available space below 4G and if asked to, also above 4G too.
This makes it important that SoC code properly reports all fixed
resources.

Currently only AGESA platforms have issues with it. On Gerrit both
attempts to fix AMD AGESA codebases to use V4 and compatibility modes
inside the V4 allocator have been proposed, but both efforts seem
stalled. See the (not yet merged) documentation
[CR:43603](https://review.coreboot.org/c/coreboot/+/43603) on it's
details. It looks like properly reporting all fixed resources is the
issue.

At this point, we are not specifying which platforms this will include
as there are a number of patches to fix these issues in flight.
Hopefully, all platforms will end up being migrated to the v4 resource
allocator so that none of the platforms need to be supported on the
branch.

Additionally, even if the support for the platform is moved to a branch,
it can be brought back to ToT if they're fixed to support the v4
allocator.

Plans for Code Deprecation
--------------------------
As of release 4.18 (November 2022) we plan to deprecate LEGACY_SMP_INIT.
This also includes the codepath for SMM_ASEG. This code is used to start
APs and do some feature programming on each AP, but also set up SMM.
This has largely been superseded by PARALLEL_MP, which should be able to
cover all use cases of LEGACY_SMP_INIT, with little code changes. The
reason for deprecation is that having 2 codepaths to do the virtually
the same increases maintenance burden on the community a lot, while also
being rather confusing.

A few things are lacking in PARALLEL_MP init:
- Support for !CONFIG_SMP on single core systems. It's likely easy to
  extend PARALLEL_MP or write some code that just does CPU detection on
  the BSP CPU.
- Support SMM in the legacy ASEG (0xa0000 - 0xb0000) region. A POC
  showed that it's not that hard to do with PARALLEL_MP
  https://review.coreboot.org/c/coreboot/+/58700

No platforms in the tree have any hardware limitations that would block
migrating to PARALLEL_MP / a simple !CONFIG_SMP codebase.

Significant changes
-------------------
This is, of course, not a complete list of all changes in the 4.16
coreboot release, but a sampling of some of the more interesting and
significant changes.

### Option to disable Intel Management Engine
Disable the Intel (Converged Security) Management Engine ((CS)ME) via
HECI based on Intel Core processors from Skylake to Alder Lake. State is
set based on a CMOS value of `me_state`. A value of `0` will result in a
(CS)ME state of `0` (working) and value of `1` will result in a (CS)ME
state of `3` (disabled). For an example CMOS layout and more info, see
[cse.c](https://review.coreboot.org/plugins/gitiles/coreboot/+/refs/heads/master/src/soc/intel/common/block/cse/cse.c).


### Add [AMD] apcb_v3_edit tool
apcb_v3_edit.py tool edits APCB V3 binaries. Specifically it will inject
up to 16 SPDs into an existing APCB. The APCB must have a magic number
at the top of each SPD slot.


### Allow enable/disable ME via CMOS
Add .enable method that will set the CSME state. The state is based on
the new CMOS option me_state, with values of 0 and 1. The method is very
stable when switching between different firmware platforms.

This method should not be used in combination with USE_ME_CLEANER.

State 1 will result in:
ME: Current Working State   : 4
ME: Current Operation State : 1
ME: Current Operation Mode  : 3
ME: Error Code              : 2

State 0 will result in:
ME: Current Working State   : 5
ME: Current Operation State : 1
ME: Current Operation Mode  : 0
ME: Error Code              : 0


### Move LAPIC configuration to MP init
Implementation for setup_lapic() did two things -- call enable_lapic()
and virtual_wire_mode_init().

In PARALLEL_MP case enable_lapic() was redundant as it was already
executed prior to initialize_cpu() call.  For the !PARALLEL_MP case
enable_lapic() is added to AP CPUs.


### Add ANSI escape sequences for highlighting
Add ANSI escape sequences to highlight a log line based on its loglevel
to the output of "interactive" consoles that are meant to be displayed
on a terminal (e.g. UART). This should help make errors and warnings
stand out better among the usual spew of debug messages. For users whose
terminal or use case doesn't support these sequences for some reason (or
who simply don't like them), they can be disabled with a Kconfig.

While ANSI escape sequences can be used to add color, minicom (the
presumably most common terminal emulator for UART endpoints?) doesn't
support color output unless explicitly enabled (via -c command line
flag), and other terminal emulators may have similar restrictions, so in
an effort to make this as widely useful by default as possible I have
chosen not to use color codes and implement this highlighting via
bolding, underlining and inverting alone (which seem to go through in
all cases). If desired, support for separate color highlighting could be
added via Kconfig later.


### Add cbmem_dump_console
This function is similar to cbmem_dump_console_to_uart except it uses
the normally configured consoles. A console_paused flag was added to
prevent the cbmem console from writing to itself.


### Add coreboot-configurator
A simple GUI to change CMOS settings in coreboot's CBFS, via the
nvramtool utility.  Testing on Debian, Ubuntu and Manjaro with coreboot
4.14+, but should work with any distribution or coreboot release that
has an option table. For more info, please check the
[README](https://web.archive.org/web/20220225194308/https://review.coreboot.org/plugins/gitiles/coreboot/+/refs/heads/master/util/coreboot-configurator/README.md).


### Update live ISO configs to NixOS 21.11
Update configs so that they work with NixOS 21.11. Drop `iasl` package
since it was replaced with `acpica-tools`.


### Move to U-Boot v2021.10
Move to building the latest U-Boot.


### Support systems with >128 cores
Each time the spinlock is acquired a byte is decreased and then the
sign of the byte is checked. If there are more than 128 cores the sign
check will overflow. An easy fix is to increase the word size of the
spinlock acquiring and releasing.


### Add [samsung] sx9360 [proximity sensor] driver
Add driver for setting up Semtech sx9360 SAR sensor.
The driver is based on sx9310.c. The core of the driver is the same, but
the bindings are slightly different.

Registers are documented [in the kernel tree:](https://web.archive.org/web/20220225182803/https://patchwork.kernel.org/project/linux-iio/patch/20211213024057.3824985-4-gwendal@chromium.org/)
Documentation/devicetree/bindings/iio/proximity/semtech,sx9360.yaml


### Add driver for Genesys Logic [SD Controller] GL9750
The device is a PCIe Gen1 to SD 3.0 card reader controller to be
used in the Chromebook. The datasheet name is GL9750S and the revision
is 01.

The patch disables ASPM L0s.


### Add support for Realtek RT8125
The Realtek RT8168 and RT8125 have a similar programming interface,
therefore add the PCI device ID for the RT8125 into driver for support.


### Add Fibocom 5G WWAN ACPI support
Support PXSX._RST and PXSX.MRST._RST for warm and cold reset.
PXSX._RST is invoked on driver removal.

build dependency:
  soc/intel/common/block/pcie/rtd3

This driver will use the rtd3 methods for the same parent in the device
tree. The rtd3 chip needs to be added on the same root port in the
devicetree separately.


### Fix bug in vr_config
The `cpu_get_power_max()` function returns the TDP in milliwatts, but
the vr_config code interprets the value in watts. Divide the value by
1000 to fix this.

This also fixes an integer overflow when `cpu_get_power_max()` returns
a value greater than 65535 (UINT16_MAX).


### Make mixed topology work
When using a mixed memory topology with DDR4, it's not possible to boot
when no DIMMs are installed, even though memory-down is available. This
happens because the DIMM SPD length defaults to 256 when no DIMM SPD is
available. Relax the length check when no DIMMs are present to overcome
this problem.


### Add FSP 2.3 support
FSP 2.3 specification introduces following changes:

1. FSP_INFO_HEADER changes
   Updated SpecVersion from 0x22 to 0x23
   Updated HeaderRevision from 5 to 6
   Added ExtendedImageRevision
   FSP_INFO_HEADER length changed to 0x50

2. Added FSP_NON_VOLATILE_STORAGE_HOB2

Following changes are implemented in the patch to support FSP 2.3:

- Add Kconfig option
- Update FSP build binary version info based on ExtendedImageRevision
  field in header
- New NV HOB related changes will be pushed as part of another patch


### Join hash calculation for verification and measurement
This patch moves the CBFS file measurement when CONFIG_TPM_MEASURED_BOOT
is enabled from the lookup step into the code where a file is actually
loaded or mapped from flash. This has the advantage that CBFS routines
which just look up a file to inspect its metadata (e.g. cbfs_get_size())
do not cause the file to be measured twice. It also removes the existing
inefficiency that files are loaded twice when measurement is enabled
(once to measure and then again when they are used). When CBFS
verification is enabled and uses the same hash algorithm as the TPM, we
are even able to only hash the file a single time and use the result for
both purposes.


### Skip FSP Notify APIs
Alder Lake SoC deselects Kconfigs as below:
- USE_FSP_NOTIFY_PHASE_READY_TO_BOOT
- USE_FSP_NOTIFY_PHASE_END_OF_FIRMWARE
to skip FSP notify APIs (Ready to boot and End of Firmware) and make
use of native coreboot driver to perform SoC recommended operations
prior booting to payload/OS.

Additionally, created a helper function `heci_finalize()` to keep HECI
related operations separated for easy guarding again config.

TODO: coreboot native implementation to skip FSP notify phase API (post
pci enumeration) is still WIP.


### Add support for PCIe Resizable BARs
Section 7.8.6 of the PCIe spec (rev 4) indicates that some devices can
indicates support for "Resizable BARs" via a PCIe extended capability.

When support this capability is indicated by the device, the size of
each BAR is determined in a different way than the normal "moving
bits" method. Instead, a pair of capability and control registers is
allocated in config space for each BAR, which can be used to both
indicate the different sizes the device is capable of supporting for
the BAR (powers-of-2 number of bits from 20 [1 MiB] to 63 [8 EiB]), and
to also inform the device of the size that the allocator actually
reserved for the MMIO range.

This patch adds a Kconfig for a mainboard to select if it knows that it
will have a device that requires this support during PCI enumeration.
If so, there is a corresponding Kconfig to indicate the maximum number
of bits of address space to hand out to devices this way (again, limited
by what devices can support and each individual system may want to
support, but just like above, this number can range from 20 to 63) If
the device can support more bits than this Kconfig, the resource request
is truncated to the number indicated by this Kconfig.
