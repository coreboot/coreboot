coreboot 25.12 release
========================================================================

The coreboot project is pleased to announce the release of coreboot
25.12, continuing our commitment to advancing open-source firmware
development. This release incorporates over 750 commits from more than
110 contributors, including numerous first-time authors.

Key improvements include:
- Addition of runtime setup options, also known as "coreboot Forms Representation" (CFR), for
  many platforms.
- Increased capacity of SMMSTORE.
- Improved support for MIPI cameras on Intel platforms (IPU).
- Qualcomm X1P42100 platform enablement.

As always, the coreboot project extends our gratitude to all
contributors who made this release possible. From experienced
developers implementing complex features to community members
providing testing and feedback, your collective contributions drive
the continued evolution of open-source firmware. The sustained engagement
from our growing community demonstrates the importance of transparent
system initialization.

The next coreboot release, 26.03, is scheduled for the end of March 2026.


Significant or interesting changes
----------------------------------

### drivers/option: Comprehensive setup option menu support

Extensive implementation of setup option menu support has been added
across numerous Google ChromeOS mainboards, enabling user-accessible
runtime firmware configuration. This enhancement allows platforms to
expose firmware options like iGPU enablement, touchpad/touchscreen
selection, fan control, and various hardware features through a
consistent API that can be exposed by payloads.

The option menu framework enables mainboards to define configuration
options providing user-visible names, help text, default values, and
runtime flags. Over 40 mainboards (and their variants) now include setup
option menu support, significantly expanding the reach of user-
configurable firmware options. Intel SoC code has also been enhanced
with additional menu item support (cde42807966), and SoC-level support
for iGPU enable/disable has been added (7f93e2fe29f).

This implementation enables OEMs and end- users to modify firmware
behavior without requiring re-compilation or reflashing, improving
accessibility and flexibility of open-source firmware solutions. The
framework maintains backward compatibility while providing a modern,
structured approach to firmware option management that can integrate
with UEFI variable storage and other persistent storage backends.


### drivers/smmstore: Increased default storage capacity

The SMMSTORE driver default size has been increased from 256KB to
512KB (d32a3728465), with numerous Google ChromeOS mainboards updated
to take advantage of the expanded storage capacity. This change
provides more space for UEFI variables and other persistent data
stored in SMM-protected regions.

Multiple platforms including Sarien, Reef, Octopus, Drallion,
Skyrim, Zork, and Guybrush have been updated to use the larger
512KB store size, ensuring adequate space for modern firmware variable
requirements. Additionally, alignment fixes for the SMMSTORE v2
structure (2a9deabc35f) improve compatibility across different
architectures and payload expectations.


### drivers/intel/mipi_camera: Enhanced SSDB (Sensor Static Data Block) support

Comprehensive improvements to the MIPI camera driver include expanded
SSDB field support with proper enums and bitfield handling
(423fbcd06b5, aa18a6fe8d5, f8d12a0bdb4, c91ea7c5824). The driver now
always generates PLD (Physical Location Descriptor) information for
camera sensors, ensuring proper ACPI representation of camera physical
locations.

Additional SSDB defaults are now automatically set
(c75236d436c), and the driver properly handles platform field
configuration (30b43839448) and VCM type and address settings
(e9ebcb2918b). These enhancements improve camera sensor enumeration and
configuration under modern operating systems, particularly Windows,
as proper SSDB and PLD configuration is required for full functionality.

DSM (Device Specific Method) generation has been refactored into
per-UUID functions (ea099e8b8c9, c8f89e00e4c), with new support for
CVF (Computer Vision Framework) DSM and I2C V2 DSM functions
(a64b93562d0, 1532eb60ee7). The driver now supports ACPI device type
selection (ae0d2324021) and ROM type/address configuration for camera
sensors (6459a2007a5), improving compatibility with various camera
implementations.


### soc/qualcomm/x1p42100: Expanded platform support and debug capabilities

Significant enhancements to the Qualcomm X1P42100 platform include
download mode detection and ramdump packing capabilities
(36edc2e371c), enabling better debugging workflows for development and
field diagnostics. Support for loading ramdump images
(445961c604a) and APDP (Application Processor Debug Policy) image
packing in CBFS (3c563669b57, 1d70286d4eb) provides comprehensive
debug infrastructure.

Display subsystem improvements include MDSS (Mobile Display
Subsystem) domain register definitions for clock enablement
(6957f84aa77), Lucidole PLL API support (9a95aef482d), and proper
DRAM reservation for display requirements (36632a08a88). Memory
layout refinements (c3afc13a0a5) optimize BL31 region placement and
TZ Application memory alignment, improving overall platform memory
utilization and security boundaries.

The platform now includes CMD-DB (Command Database) driver support
(01bc527afa2), enabling resource address and configuration data
lookup for hardware accelerators. The CMD-DB region is properly mapped
as non-cacheable in MMU (a4cc1784860, 2277edff88b), ensuring correct
access patterns for shared resource management. ARM64 architecture
improvements include distinct PRERAM and POSTRAM stack regions
(641f7ac677b, 1b599a88449), with PRERAM stack relocated to BSRAM
memory (4d53aa77042) for improved memory utilization.


### soc/intel: LPCAMM (Low Power Compression Attached Memory Module) support

Support for LPCAMM has been added to Intel Panther Lake platforms
(58cdf9e6685), providing infrastructure for this emerging memory
standard. LPCAMM topology support (67777b76719) enables platforms to
properly enumerate and configure these new memory modules, with initial
board support added for Panther Lake RVP T3 (b31e62ae5cc).

This addition positions coreboot to support future Intel platforms
utilizing LPCAMM, ensuring compatibility with next-generation laptop and
mobile form factors adopting this memory standard.


### soc/amd: Glinda/Faegan enhancements and ACPI improvements

AMD Glinda platform support has been expanded with Faegan SoC variant
support (244e8edf182), USB4 FSP UPD configuration from devicetree
(f4aeac42767), and 10GbE device additions (9e5c7eb3f87). The platform
now properly fills DIMM voltage information in FSP configuration
(dcd4f071887).

ACPI improvements include fixed base address reporting for LPC
(8929659d936, 3053cd2dad1), I3C controller ACPI code
(c1958597487), and HEST (Hardware Error Source Table) support
(2aadfc2b5e1, cc542c15f42) for enhanced error reporting capabilities.
ECAM MMCONF support has been extended to 64-bit addressing
(8b97968e53d), and bootblock CRTM (Core Root of Trust for
Measurement) initialization support has been added
(33fc33c132c), improving security and system reliability.


### include/acpi: Comprehensive APEI (Advanced Platform Error Interface) infrastructure

Extensive APEI struct definitions have been added to support advanced
error reporting capabilities (679ea61d4de, b689671e79d, 7a41dc416be,
5251284e392). The implementation includes structs for Machine Check
Exception (MCE), Non-Maskable Interrupt (NMI), and PCIe AER (Advanced
Error Reporting) error sources, providing a foundation for BERT (Boot
Error Record Table), HEST (Hardware Error Source Table), and EINJ
(Error Injection Table) implementations.

These additions enable platforms to properly report hardware errors
to operating systems, supporting firmware-first error handling models
and improving system reliability diagnostics. The struct definitions
follow ACPI specification standards and include proper validation
(847d91b82e5, b70309350f8).


### commonlib: Code consolidation and endian handling improvements

The `<endian.h>` header implementations from both coreboot and
libpayload have been merged into commonlib (02a2fe7907d), eliminating
code duplication and ensuring consistent endian conversion behavior
across all coreboot components. The deprecated `<swab.h>` header and
swabXX() functions have been removed (8f34fdfab3a), completing the
transition to standard endian conversion APIs.

Memory information structures have been enhanced with new fields
(273a41c4d93), improving SMBIOS memory type 17 compatibility and
supporting DDR3 voltage field reporting (25c45012232). Device tree
handling improvements fix NOP token skipping issues (db01aa6cb29),
and boot mode information is now stored in CBMEM for payload
coordination (27fcb8617dc).


Additional coreboot changes
---------------------------

* SMMSTORE structure alignment fixes improving cross-architecture
  compatibility (2a9deabc35f)
* MediaTek display subsystem refactoring supporting dual DSI and DSC
  (Display Stream Compression) for MIPI displays (7e1aa974bfd, 3aaeca83780)
* MediaTek DSI API improvements with proper register structure passing
  (6b5a872ce85, 74c13eead40, cf0b91d7749)
* Intel Skylake CSE reset status utilization improving reset reliability
  (5eaf85d19b0, 84a4cdc6a53, 4f52ca6ba66)
* AMD MTRR calculation simplifications for improved boot performance
  (04f83ff7dca)
* Enhanced Intel touch driver support for new I2C _DSD entries
  (e6a8143d8b4)
* Improved ACPI FACS table initialization with proper clearing
  (26a18c674d0)
* TPM driver improvements removing duplicate operations and fixing
  generated ACPI (67a7e06c386, ac5c57d24a3)
* Intel GMA brightness handling improvements with caching and valid-cache
  logic (23b00a06da9, 2d78478345e, 2e96a71e6f9)
* LAPIC spurious interrupt vector standardization (8795680828f)
* Intel Fast SPI DMA improvements ensuring proper completion before
  lockdown
* Enhanced device tree overlay detection API (ee59936e833)
* Libpayload API additions for physical memory size querying
  (c109fc92ff2)
* Libpayload ARM64 asynchronous exception routing fixes (7fb0f14ebea)
* Libpayload LZ4 legacy compression format support (d62653749cf)
* Enhanced EC reboot flag usage for immediate cold reboots
  (c77d256886b, 1a0d123ec13)
* Improved SPD support with additional DDR4 parts and dual die package
  width corrections (e4a809d4415, 4eb524ee9d1)
* SMBIOS socket type additions for BGA1744 packages (d09ea1c3512)
* EC RGB keyboard boot color configuration options (d97cb61b508, e695731399d)
* Panel ID reporting improvements for Google platforms
* Intel FSP UPD header typedef additions (2ce4e094690)
* Azalia verb table implementation rework for improved maintainability
  (31fc5b06a6b) across multiple platforms
* Azalia driver timing fixes including proper 521us delay after RESET#
  de-assertion (ecf202b8e4a) and link-reset improvements
  (6e074550a5c, 152914272c1)
* Intel touch driver enhancements for new devices with improved I2C
  speed handling (f1708cf21a2, fce489e9e5c, 1af54d9784b)
* Generic graphics driver support extended to non-VGA devices
  (0f1ae4ae5f1), expanding compatibility beyond traditional VGA
  displays
* MediaTek ARMv9 MTE (Memory Tagging Extension) tag memory support
  added to bootmem (3d5135fdd07, 9203cc827f9), enabling proper memory
  tagging for security-enhanced platforms
* Parallel charging infrastructure and support for Google Bluey platforms
  (de87ea0efad, 896984e800a), enabling faster charging capabilities
* Qualcomm USB Type-C support with PHY configuration and repeater support
  (8ffa58723a2, 45cedbb9922, 155041ad4cf, b18dfde22a7)
* SoundWire drivers for Cirrus Logic CS35L56 and CS42L43 codecs
  (a5252bd5b97, f1c973bbff5)
* RISC-V ACPI support additions (5daf497df49)
* Numerous mainboard-specific enhancements across Intel, AMD, MediaTek,
  and Qualcomm platforms


Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade binutils from version 2.44 to 2.45 (4a3cc37cbd6)
* Upgrade ACPICA from 20250404 to 20250807 (402ac7cd812)
* Drop nds32le-elf toolchain from default builds (14fc6c34690)


### Vendorcode updates

* Updates to FSP headers for Panther Lake (PTL) to FSP 3373_03 and Wildcat
  Lake (WCL) to 3344_03 (0eafe672e32, 1699d455e70)
* AMD FSP updates for Glinda platform (f68450e39b1)


### Git submodule pointers

* 3rdparty/blobs: Update from commit id a0726508b8 to 4a8de0324e (39 commits)
* 3rdparty/intel-microcode: Update from commit id 4ded52b4b0 to f910b0a225 (1 commit)

### External payloads

* Libpayload enhancements including:
  - physical memory size API (c109fc92ff2)
  - endian.h consolidation with coreboot (02a2fe7907d)
  - ARM64 exception routing fixes (7fb0f14ebea)
  - legacy LZ4 compression support (d62653749cf)


Platform Updates
----------------

### Added mainboards/variants:

* ASRock Z77 Extreme4
* ASUS PRIME H610I-PLUS D4
* Google Fatcat: ruby
* Google Ocelot: kodkod, ocicat
* Google Rauru: Sapphire
* Google Skywalker: Dooku, Grogu
* Lenovo ThinkPad (sklkbl_thinkpad): T470s, T580
* Siemens MC EHL: MC EHL6
* Star Labs Starfighter: Arrow Lake (285H)
* Topton ADL: TWL (X2E_N150)


### Added SoCs:

* AMD Turin POC (proof of concept)


### Updated SoCs

* Enhanced qualcomm/x1p42100 with ramdump, APDP, and display support
* Enhanced amd/glinda with Faegan variant, USB4, and XGBE support
* Enhanced intel/pantherlake with LPCAMM memory support


Statistics from the 25.09 to the 25.12 release
--------------------------------------------
* Total Commits: 757
* Average Commits per day: 8.94
* Total lines added: 62219
* Average lines added per commit: 82.19
* Number of patches adding more than 100 lines: 77
* Average lines added per small commit: 42.02
* Total lines removed: 9669
* Average lines removed per commit: 12.77
* Total difference between added and removed: 52550
* Total authors: 106
* New authors: 21


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
