Upcoming release - coreboot 25.06
========================================================================

The coreboot project is pleased to announce the release of coreboot
25.06, another milestone in our work promoting the use of open-source
firmware. This release incorporates almost 900 commits from more than
120 contributors, bringing enhanced boot splash screen capabilities,
improved build tooling with GCC 15 support, and expanded wireless power
management features. Along with these headline improvements, we've
delivered numerous stability enhancements and infrastructure updates
that strengthen coreboot's foundation across all supported platforms.

As always, the coreboot project extends our appreciation to everyone who
contributed to this release. From seasoned developers submitting complex
patches to newcomers providing valuable feedback and testing, your
collective efforts make each coreboot release better than the last. The
continued growth of our contributor base demonstrates the strength and
vitality of the open firmware community.

The next coreboot release, 25.09, is scheduled for the end of September
2025.


Significant or interesting changes
----------------------------------

### {lib, drivers/intel}: Enhanced boot splash screen framework

A comprehensive overhaul of the boot splash screen infrastructure
introduces support for multiple logo types and improved rendering
capabilities. The new framework adds `bmp_load_logo_by_type()` function
(be5609bdaf, 4373eea5d8) to enable platform-specific logo selection,
while introducing splash screen footer support for better brand
presentation.

The enhanced system includes horizontal logo alignment controls
(dfeaead9f2) and configurable footer bottom margins (97f92d5c69),
providing OEMs with greater flexibility in customizing the boot
experience. Additionally, the framework now tracks splash screen
rendering completion (4c446751c6) through commonlib, enabling better
coordination between firmware phases and payload handoff.

Low-battery mode detection has been integrated into the splash screen
system, automatically suppressing OEM footers when battery levels are
critical (d309a9dfa8). This ensures essential power information takes
precedence during low-power boot scenarios, improving user experience on
battery-powered devices.


### util/cbfstool: Multi-ELF support for advanced payload configurations

The cbfstool utility has been extended with multi-ELF support
(0dcea61e7c), enabling more sophisticated payload packaging and
deployment scenarios. This enhancement allows firmware developers to
bundle multiple ELF binaries within a single CBFS entry, facilitating
complex boot scenarios where multiple components need to be loaded
simultaneously.

The implementation provides better flexibility for hypervisor
deployments, multi-stage bootloaders, and security-enhanced boot
configurations where isolation between components is critical. This
feature particularly benefits platforms requiring trusted execution
environments or compartmentalized boot processes.


### drivers/wifi/generic: Advanced Power Reduction Features

Implementation of both Wi-Fi and Bluetooth Power Reduction Request (PRR)
DSM functions (d92b6163e7, efa24540b0) significantly improves power
management capabilities for wireless subsystems. These ACPI methods
enable operating systems to request aggressive power savings during
idle periods or when specific wireless functions are not required.

The PRR implementation follows Microsoft's Modern Standby
specifications, allowing Windows and other operating systems to achieve
deeper sleep states by coordinating with wireless hardware. This results
in improved battery life on mobile platforms and better thermal
management on passively cooled systems.


### security/vboot: Enhanced boot phase integrity management

The vboot subsystem now includes CMOS data backup during later boot
phases (bf330f2dd0), providing additional protection against unexpected
power events during firmware execution. This change ensures that
critical configuration data remains consistent even if power is lost
during complex initialization sequences.

A new VBOOT_EC_SYNC_ESOL Kconfig option (ac4503d0dd) has been introduced
to provide finer control over Embedded Controller synchronization
behavior. This allows platforms to optimize EC update procedures based
on their specific hardware requirements and boot time constraints.

Additionally, MRC cache measurements as runtime data (b5581d556b) have
been integrated into the trusted boot chain, ensuring that memory
training data integrity is verified as part of the overall system
attestation process.


### Build system improvements: GCC 15 compatibility and toolchain updates

Comprehensive work has been completed to ensure compatibility with GCC
15, including fixes for the new -Werror=unterminated-string-initialization
warning (73cc8a413a). The build system now properly handles modern
compiler requirements while maintaining backward compatibility with
existing toolchain versions.

The crossgcc build system has been enhanced with RISC-V ISA
specification support (89e4fff2d3), enabling more precise control over
RISC-V target architectures. This improvement allows developers to build
optimized firmware for specific RISC-V implementations while ensuring
compatibility across the diverse RISC-V ecosystem.

Submodule management has been significantly improved with better path
handling (c4eb645a0b) and enhanced logging (f2310ab35e), making it
easier for developers to track third-party component updates and
maintain consistent build environments.


### Enhanced SoC support: Emerald Rapids and Panther Lake

Support for Intel's 5th Generation Xeon Scalable Processors (Emerald
Rapids) has been added (9d878fc6c0), bringing coreboot to Intel's latest
server and workstation platforms. This implementation includes support
for the expanded feature set and improved power management capabilities
of the Emerald Rapids architecture.

Panther Lake support continues to evolve with new SKU additions
(b879342fe6) and VR Fast Vmode threshold optimizations (e58883aace).
The refactored SoC configuration (cf5696834b) provides a cleaner
foundation for supporting the full range of Panther Lake variants as
they become available.


### Documentation: Clarification of contributor responsibilities and AI tool usage

A significant policy clarification has been added to the project
documentation (9ddc9cbfc9) addressing contributor responsibilities,
particularly in the context of modern development practices including
the use of AI-assisted tools. This addition reinforces that submitters
bear full responsibility for ensuring their contributions meet coreboot's
quality standards and comply with all applicable licensing requirements,
regardless of how the code was generated or authored.

The new documentation explicitly states that contributors must have all
necessary rights to submit their work under coreboot's licenses, and
that violations of third-party rights will result in code removal or
replacement. This policy applies universally, encompassing contributions
created through various means including generative AI tooling, ensuring
the project maintains clear legal standing and quality expectations.

This clarification provides important guidance to the growing coreboot
community as development practices evolve, establishing clear
expectations while allowing contributors the flexibility to use whatever
tools help them create high-quality firmware code.


Additional coreboot changes
---------------------------

* Exposed ALWAYS_ALLOW_ABOVE_4G_ALLOCATION in Kconfig for x86 to fix
  Resizable BAR support on modern GPUs.
* Enhanced PCI support with SR-IOV Virtual Function BAR resource
  assignment capabilities
* Improved SPI driver support for forced 4-byte address mode operation
  via 0xB7 command sequences
* Numerous MediaTek MT8189 and MT8196 platform enhancements including
  eDP, DDP, and AUXADC driver improvements
* Enhanced memory initialization support across multiple Intel platforms
  with improved timing and reliability
* Expanded superio support with addition of Fintek f81966d controller
* Improved ELOG handling for later boot phase operations
* Enhanced SMMSTORE driver with 64-bit MMIO address support
* Comprehensive timing infrastructure improvements with early chip
  initialization timestamps
* Libpayload updates focused on ARM64 DMA and MMU fixes, build system
  improvements, and support for large USB mass storage devices.
* Add documentation on devicetree, sconfig, cbmem and device operations.
* Contributor guides updated with new commit message policy, Gerrit
  instructions, and collaboration details.



Changes to external resources
-----------------------------

### Toolchain updates
* Fix GMP build on GCC 15
* Add a `RISCV_ISA_SPEC` variable to the toolchain build script to allow
  for specifying the RISC-V ISA version.


### Git submodule pointers
* arm-trusted-firmware: Update from commit id e5a1f4abee to 9109143417
  (523 commits)
* fsp: Update from commit id 86c9111639 to cc36ae2b57 (9 commits)
* intel-microcode: Update from commit id 8a62de41c0 to eeb93b7a81 (1
  commit)


### External payloads

* edk2: Update default branch for MrChromebox repo to 2025-02


Platform Updates
----------------

### Added mainboards:
* ASUS H61M-A/USB3
* CWWK CW-ADL-4L-V1.0
* CWWK CW-ADLNTB-1C2L-V3.0
* Google Anakin
* Google Baze
* Google Bluey
* Google Brox RTK EC
* Google Epic
* Google Fatcat4ES
* Google Fatcatite4ES
* Google Fatcatnuvo4ES
* Google Felino4ES
* Google Kaladin
* Google Kinmen
* Google Obiwan
* Google Ocelot4ES
* Google Ocelotite
* Google Ocelotite4ES
* Google Ocelotmchp
* Google Ocelotmchp4ES
* Google Pujjocento
* Google Pujjolo
* Google Quenbi
* Google Yoda
* Intel Google Chrome EC
* MiTAC Computing R520G6SB
* MiTAC Computing SC513G6
* NovaCustom V540TNx (14")
* NovaCustom V560TNx (16")
* Siemens MC RPL1
* Star Labs Byte Mk III (N355)
* System76 darp11
* System76 lemp13

### Updated SoCs

* Added qualcomm/x1p42100


Statistics from the 25.03 to the 25.06 release
--------------------------------------------
* Total Commits: 879
* Average Commits per day: 9.95
* Total lines added: 73596
* Average lines added per commit: 83.73
* Number of patches adding more than 100 lines: 84
* Average lines added per small commit: 40.40
* Total lines removed: 15717
* Average lines removed per commit: 17.88
* Total difference between added and removed: 57879
* Total authors: 128
* New authors: 34


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>

