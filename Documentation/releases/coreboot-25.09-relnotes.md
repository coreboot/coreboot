Upcoming release - coreboot 25.09
========================================================================

The coreboot project is pleased to announce the release of coreboot
25.09, continuing our commitment to advancing open-source firmware
development. This release incorporates over 680 commits from more than
110 contributors, including more than 20 first time authors. This brings
performance enhancements, improved boot functionality, and enhanced
developer tooling. Key improvements include (up to) a 30% speed increase
in LZMA decompression, new boot mode detection capabilities, and
comprehensive utility tool enhancements that strengthen the development
experience across all supported platforms.

As always, the coreboot project extends our gratitude to all
contributors who made this release possible. From experienced
developers implementing complex optimizations to community members
providing testing and feedback, your collective contributions drive
the continued evolution of open firmware. The sustained engagement
from our growing community demonstrates the vital importance of
hardware freedom and transparent system initialization.

The next coreboot release, 25.12, is scheduled for the end of December.


Significant or interesting changes
----------------------------------

### SPI flash and payload loading performance improvements

Multiple optimizations across the storage and decompression pipeline
deliver significant boot time improvements. The most impactful change
optimizes LZMA decompression (159afbc5d5) by using SSE prefetch
instructions when CONFIG_SSE is enabled, allowing the SPI controller to
preload data into CPU cache while decompression continues. This
technique exploits the timing characteristics of SPI interfaces running
at 100Mbit/s on Sandy Bridge mobile CPUs, achieving 46ms faster boot
times, representing a 30% performance improvement during payload
decompression. Testing on Lenovo X220 demonstrated the payload loading
bandwidth increased to 53 Mbit/s.

Intel Panther Lake platforms receive dedicated asynchronous file loading
capabilities (2de0158eec) that utilize SPI DMA to preload the fsps.bin
file while the CPU executes other boot tasks. This cooperative
multithreading approach maintains boot predictability while improving
performance by approximately 17-18 ms on tested Fatcat devices. The
implementation strategically coordinates with chipset lockdown settings
to ensure preloading completes before SPI DMA interfaces are locked,
requiring special FSP binaries when CHIPSET_LOCKDOWN_COREBOOT is used.

Supporting infrastructure improvements ensure robust and efficient DMA
operations across platforms. A new synchronization function
cbfs_preload_wait_for_all() (3b069d320c) provides safe shutdown points
for asynchronous operations before storage backends are deactivated. The
Fast SPI DMA subsystem gains a token-based transfer queue (eb1b5ee116)
that minimizes gaps between consecutive DMA operations, creating more
consistent and predictable boot performance. Additionally, CBFS cache
buffer alignment was corrected (454079c3bc) to properly honor
CONFIG_CBFS_CACHE_ALIGN requirements, ensuring optimal memory access
patterns throughout the boot process.


### lib: Enhanced boot mode information framework for payload coordination

Implementation of comprehensive boot mode detection capabilities
(a45c8441af, 893a2b008a) introduces the new `LB_TAG_BOOT_MODE` in
coreboot tables, enabling platforms to communicate critical boot state
information to payloads. The framework supports detection of normal
mode, low-battery mode, and off-mode charging states through a
standardized `enum boot_mode_t` interface.

This enhancement is particularly valuable for platforms where the
Application Processor manages charging solutions, as it eliminates the
need for payloads to reimplement battery detection logic. By passing
boot mode context through coreboot tables, the system ensures
consistent battery and charging information availability across both
firmware and payload phases.

The implementation includes a new weak function `lb_add_boot_mode()`
that platforms can override to provide specific boot mode detection
logic. This approach maintains backward compatibility while enabling
platforms to implement sophisticated power management strategies based
on hardware capabilities and charging state requirements.


### drivers/intel/fsp2_0: Optimized graphics memory initialization sequence

A comprehensive refactoring of graphics memory MTRR management
(5f0225a7b5) improves FSP-S initialization performance by moving Write-
Combine MTRR setup for graphics memory from logo rendering functions to
the silicon initialization phase. The new `soc_mark_gfx_memory()`
helper function establishes temporary WC MTRRs earlier in the boot
process, enabling better memory access patterns during graphics
initialization.

This optimization addresses performance issues specifically affecting
platforms using `USE_COREBOOT_FOR_BMP_RENDERING` where logo rendering
occurs through coreboot drivers rather than FSP logic. Testing
demonstrated FSP Multi-Phase Silicon Init improvements from 123ms to
115ms, representing a measurable reduction in graphics subsystem
initialization time.

The refactoring also improves code maintainability by decoupling MTRR
management from logo rendering functions, creating a cleaner separation
of concerns between memory configuration and graphics operations. This
change enables more consistent performance across different graphics
initialization scenarios while maintaining compatibility with existing
platform configurations.


Additional coreboot changes
---------------------------

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* AMD firmware tool (amdfwtool) enhancements for Turin platform support
  with improved address mode handling and EFW structure parsing
  capabilities (46b03e682c, 73dd7bb046, 97cf4a1919)
* smmstoretool expansion supporting variable block sizes and secure
  boot GUID aliasing for improved UEFI variable management
  (00d954977c, 008f0ec078)
* CBMEM utility robustness improvements replacing abort() with exit(1) for
  better error handling in debugging scenarios (4fd3cb35c2)
* Build system enhancements with improved sequential board building
  support in abuild, and MediaTek platform build optimizations
  (9c5557f982)
* Memory subsystem improvements including SMBIOS memory type and form
  factor updates (d4da533473, 183589dcbd), graphics memory compression
  enablement for Panther Lake (ebab858d92, ad10d4a977), and enhanced
  memory training capabilities (e31fbc493d, d5854e4139)
* SPI flash handling improvements including addition of a 4-byte address
  mode flag in `lb_spi_flash` and refactored 4-byte address mode entry/exit
  handling (8dec5fcaf8, a01c368a8a, 8f09629fb1)
* Expanded TPM support across multiple platforms with CRB TPM
  enablement and improved fTPM integration for AMD platforms
  (984c64295b, 4b58ec5ac2, 8df079c609)
* Graphics and display improvements including enhanced logo rendering
  with proper bottom margin handling (51a8e238b0) and QEMU bochs driver
  architectural compatibility enhancements (5171098814)
* cbfstool robustness improvements including overflow prevention when
  sorting FIT table entries, bail-out on invalid ELF in amdcompress,
  and fallback copying of segments when compression fails
  (480ac15044, 69888bc7fc, f3ca3aa16b)
* AMD PSP SPI flash semaphore implementation preventing SMM corruption
  of SPI controller registers during concurrent access, with improved
  flash busy checking in PSP SMI handler (a17a41559a, 038262155e)
* Intel Fast SPI DMA transfer cleanup ensuring proper completion before
  lockdown to prevent low power state over-consumption from hanging
  transfers (ef0c650edf)
* Numerous platform-specific enhancements for Intel, AMD, MediaTek, and
  Qualcomm SoCs with improved power management, memory initialization,
  and hardware interface support


Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade binutils from version 2.43 to 2.44 (0a94fcd2db)
* Upgrade ACPICA from 20241212 to 20250404 (f2fed71533)
* Upgrade MPFR from 4.2.1 to 4.2.2 (c3f5d7c1ee)
* Enhanced compiler-rt build system using runtimes configuration (f1aa0a175b)
* Improved build reliability with HOSTCFLAGS handling for GMP headers (ad9bfd4243)
* Fixed IASL build initialization for OPT_LDFLAGS variable (07a8737cbd)


### Git submodule pointers

* fsp: Update from commit id cc36ae2b57 to 9623d52450 (21 commits)
* intel-microcode: Update from commit id eeb93b7a81 to 4ded52b4b0 (1 commit)
* qc_blobs: Update from commit id a252198ec6 to 6379308814 (3 commits)


### Payloads

* SeaBIOS: Update from 1.16.3 to 1.17.0, bringing improved hardware
  compatibility with preferential PCI I/O allocation above 4GB on
  64-bit capable machines, multiple simultaneous USB keyboard and mouse
  support, SeaVGABIOS VBE palette data support, and removal of legacy
  internally generated ACPI table support (d315f26217)
* U-Boot: Upgrade from 2024.07 to 2025.07 (0e682859e7)
* edk2: Configure capsule update support for FMP (Firmware Management
  Protocol) capsules, with enhanced build system compatibility and
  improved menu sizing for non-full-screen configurations
* LinuxBoot: Build system improvements with corrected prerequisites
  and target renaming for better maintainability (d233b6c903,
  502d19be89, cba0f0b8b9)
* Libpayload enhancements include coreboot boot mode table support for
  improved payload coordination (893a2b008a), strsep() edge case fixes
  for more robust string handling (8097809c8a), Qualcomm PCI driver ATU
  configuration fix (53dd93ff14), cbmem console memory leak resolution
  in error conditions (05396238da), and propagation of SPI flash address
  mode flag to libpayload (61d74dc8f7)


Platform Updates
----------------

### Added mainboards:

* ASROCK SPC741D8-2L2T/BCM
* GIGABYTE GA_H81M_D2W
* Google BlueyH
* Google Caboc
* Google Kinmen4ES
* Google Lapis
* Google Matsu
* Google Moonstone
* Google Ojal
* Google Padme
* Google Quartz
* Google QuenbiH
* Google Tarkin
* HP 260 G1 DM
* HP Compaq Pro 6300 SFF/MT
* HP ProDesk 600 G1 SFF
* Intel Ptlrvp4es
* Lenovo ThinkPad T480
* Lenovo ThinkPad T480s


Statistics from the 25.06 to 25.09 release
--------------------------------------------

* Total Commits: 684
* Average Commits per day: 7.55
* Total lines added: 62999
* Average lines added per commit: 92.10
* Number of patches adding more than 100 lines: 73
* Average lines added per small commit: 38.99
* Total lines removed: 10251
* Average lines removed per commit: 14.99
* Total difference between added and removed: 52748
* Total authors: 112
* New authors: 21


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
