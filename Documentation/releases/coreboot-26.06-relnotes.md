coreboot 26.06
========================================================================

The coreboot project is pleased to announce the release of coreboot
26.06. In the roughly three months since the 26.03 release, 101 authors
(including 22 new authors) have landed well over eleven hundred commits
of new features, platform enablement, cleanup, and bug fixes across the
tree.

This release exists because of the people who wrote, reviewed, tested,
and reported against it. Thank you. Whether you landed a new SoC port or
fixed a single typo, your work is in here, and the project is better for
it.

The next coreboot release, 26.09, is scheduled for the end of September
2026.



Significant or interesting changes
----------------------------------

### soc/intel/novalake: Initial Nova Lake SoC support

This release lays down the foundation for Intel's Nova Lake (NVL)
platform. The SoC was brought up in stages, with separate commits taking
the code through bootblock (5fd6ad6644), romstage (a8c9ecfaa9), and
ramstage (cfa90f7a03), followed by FSP-M (0beffbd0be) and FSP-S
(a6798f2eb5) programming. GPIO definitions (0abe0a6f0b) and the SoC ACPI
directory (9b6d29c2ec) round out the basic enablement.

Several supporting pieces landed alongside the core code. The chipset.cb
mechanism is used to link PCIe root port operations (1945d07700), the
Intel Flash Descriptor chipset is configured (0f171a217f), and IOC 1MB
LGMR support is enabled (2a2ab9e0cc). The heap size was increased to
make room for high-quality firmware splash rendering (fabe57c3f7).
Tooling keeps pace as well: the NVL FSP headers were imported from
v3150.22 (e6bc275fdb), ifdtool gained NVL support under IFD2
(2f9ab09057), and spd_tools learned about the Nova Lake SoC
(61dc9a4916).

Note that this is early enablement work. DDR5 is not yet supported, and
some FSP workarounds remain in place pending upstream FSP fixes. The
Google Atria baseboard and its variants were also migrated from the
Panther Lake SoC to Nova Lake during this cycle as the bring-up matured
(cff044285b).


### soc/amd/strix_halo: Initial Strix Halo SoC support

AMD's Strix Halo SoC received its first code in this cycle. The initial
commit (ee148cc689) introduces the SoC based on a rename of glinda, and
subsequent patches adjust it for the new part: the maximum CPU count was
tuned (c108ccb930), the fw.cfg was adapted (29f93567c5), the APOB region
was grown from 256K to 576K (eedd292f32), and the CPUID match was
updated (7982c6e172). Dummy FSP headers were added to keep builds going
until binaries and real headers are available (bddc56d7fe), and the
VBIOS path washooked up (d381d33a39). The new SoC also picked up
maintainers (847b42fa69).

The first board to use Strix Halo is the AMD Maple reference mainboard,
introduced in this release (47f124abba) with its EC, GPIO, and blob
plumbing (7f8e7842fc, d5d8446831, 995eb78c64). As with Nova Lake, this
is work-in-progress enablement and is not yet intended for use on real
hardware.


### soc/qualcomm/calypso: Initial Calypso SoC support

The Qualcomm Calypso SoC was bootstrapped from the X1P42100 codebase
with SoC-specific pieces removed, providing a foundation for incremental
development (38e8eadfa7). From that skeleton the platform gained ARM
Trusted Firmware support (230f210d2d), basic PCIe (30b8524ff5), QUPv3
clock initialization (e4cae1a62b) and Serial Engine entries
(8dbf88a300), the SPMI driver in romstage and ramstage (49e3595e1f), and
QSPI/QUPv3 bring-up in the bootblock (421c21c6cf).

Firmware loading and memory layout were fleshed out with CPUCP and PDP
firmware loading (192fdf75e0), a split of the CPUCP binary into RO and
RW regions (1c79360b44), CBFS preloading for BL31 and BL32 (03aaebef7e),
and memory layout alignment for the new SoC (22a67b77d6, 6995ef7c91).
Calypso is consumed by the new Google Calypso, Mensa, and C1nv
mainboards; the directory itself was renamed from the earlier "mensa"
directory as the work matured (201392d363, e187893fa9).


### security/amd: ROM Armor 2 and A/B recovery infrastructure

AMD platforms gained a substantial expansion of firmware-integrity and
recovery tooling. ROM Armor 2 support was implemented in the PSP common
block (16a69e9619), backed by a PSP mailbox interface (8121a3dd72),
HSTI-based state reporting (cd8072191d), and enforcement checks in the
SPI path (3bc8a9fec1, 2ff5b9c0ff, d72d7d1ba0). The PSP now talks to the
SPI controller through the ROM Armor mailbox interface (c4bd58b02d), and
the AMD fTPM driver learned to operate under ROM Armor with backup SPI
flash support (63fff936cd, 3a87b84f7c).

In parallel, A/B recovery support was added at the SoC level
(4ca21336b4), introducing a new FMAP layout with EFS, RECOVERY_A, and
RECOVERY_B partitions. The PSP gained commands for A/B recovery
(492b7c7c09), the ability to boot from a second flash device
(b6385bcf0d), and logic to preserve the recovery flag across boots
(2059d08bbf). Cezanne was the first SoC wired up for the new flow with
A/B recovery and PSP_AB_NVRAM support (f78ba958e5, 302ea069ef). On the
tooling side, amdfwtool learned to split the A/B level 2 directory from
PSP level 1 (ac8bd296ee) and fix backup PSP L1 generation (dee0bfdbc0),
while amdfwread gained A/B recovery parsing (0aaeff81e3).


### mb/google: Qualcomm mobile charging and battery management

The Qualcomm-based Google Bluey and Calypso boards received an extensive
set of charging, battery, and low-power-boot improvements. Calypso
gained a charging framework (07ce3b92a0) and a low-power charging boot
sequence (ae5b0d713c), while Bluey implemented slow-to-fast charging
transition logic (eda62af9dd), falling back to slow charging when the
battery is below 2% (8e57010d88) and monitoring thermal sensors
throughout the charging process (86b3901ba5).

Robustness in degraded power states was a recurring theme. Bluey now
handles a dedicated "no-battery" boot mode (9dc937c982), carries dead
battery boot-flag configuration (2115ec3b4a), and includes battery
ship-mode and cutoff recovery handling in romstage (d300ef280c,
17b825ae1c). Boot mode and battery status are cached early and reused to
streamline mainboard init (99b53e5a84, 7c75b0655f), and NVMe power
sequencing was optimized in romstage to bring the rail up earlier
(69f0093d54, 38addfb24f). Several of these behaviors were iterated on
through the cycle, including reverts that backed out boot-count and
reboot changes pending further work (a2f401da5b, 0b2d5a6e7a).


### nb/intel/haswell: Broadwell enablement and CPU power tuning

Two goals drove this work: unifying the Haswell and Broadwell codebases,
and getting Broadwell "trad" (non-ULT) parts working in coreboot. The
latter is still a work in progress as of this release and will only work
with native raminit (NRI), since neither the Haswell nor the Broadwell
MRC supports those parts. As part of this effort, Broadwell's native
graphics init was backported to Haswell: the driver gained Broadwell
device IDs (6e95ade0cb), GT power-management init (aa9ff8895f), and
CDCLK programming (f7412bf209), along with a series of cleanups that
bring the two graphics paths closer together (b75d086f86, 892d68a8c8,
f2e24e5230).

On the CPU side, a new OC mailbox undervolt driver was introduced
(fa68b66686) and wired up for Haswell (aaa396d571), Skylake
(e9239d2308), and Cannonlake (1654e0a1de). Haswell also gained
option-backed PL1/PL2 power-limit overrides with package-limit locking
(1dc346e61e) and now finalizes the CPU in ramstage (f841e98bca). This
work pairs with a wave of newly added ASUS Haswell-era boards, including
the Maximus VI and VII series, the Z87-K, and the H81M-K.


### soc/intel/pantherlake: Initial Panther Lake SoC and baseboard support

Intel's Panther Lake (PTL) SoC enablement saw significant development
this cycle, alongside extensive work on the Google Fatcat and Ocelot
reference platforms. The core SoC gained UFS inline encryption support
(dfe5b08978), CNVi WWAN coexistence (d9956b0bcf), VCCSA shutdown
mitigation (eeca9d6f77), and transitioned to a common CBFS preload
implementation (084a7ea69c).

At the board level, the Google Fatcat and Ocelot baseboards and their
variants (ruby, lapis, moonstone, etc.) received major feature
additions. Highlights include lid-closed power-off options (a5d2a225a1),
tablet-mode support, active display limiting for portrait panels
(a5fb73a737), and dedicated UFS inline encryption enablement across
multiple board variants.



Additional coreboot changes
---------------------------

* The Lynx Point and Wildcat Point southbridges were heavily unified,
  with Wildcat Point now reusing Lynx Point's ME, PCIe, HDA, PMC, and
  SMI code rather than maintaining its own copies (1f376aebde,
  ffce7b6ae7, 73b325e337, 82b7a33abf, 23de5b5288). The shared ME path
  also picked up Wildcat Point finalisation and a use-after-free fix
  along the way (f64c1fb9c3, 574c7e116d).
* Continued progress on C23 compatibility, replacing `__typeof__` with
  `typeof` treewide (b084eef118) and applying the `nonstring` attribute
  to unterminated string buffers (e57478e238).
* `WARNINGS_ARE_ERRORS` was removed treewide (e8a3bb81db), simplifying
  the build configuration.
* SPI flash handling gained JEDEC SFDP parsing (67845716da) and now uses
  larger block erases where possible (95edd07bca).
* Boot performance work continued with cooperative-multitasking support
  on arm64 (2227096f55), optimized cached LZMA decompression
  (cd7f369416), ROM3 caching on AMD (72da5e8f41), and microcode caching
  in DRAM on Intel (bc61158f64).
* Default FMAP generation was reworked, and the x86 FMAP region size was
  increased from 0x200 to 0x1000 to accommodate larger layouts
  (5b71cd5f15, d55ed29d85).
* The Intel FSP 2.0 driver gained Zstd decompression support
  (bc7976277b).
* Support was added for secondary-resolution boot splash logos, letting
  boards pick a logo based on panel resolution (5b811635e7).
* The Realtek r8168 driver had its ERI programming reworked and a stale
  Kconfig option dropped (0c5d76977c), and a shared Nuvoton HWM
  fan-control helper was introduced for Super I/O boards (a46373fcfa).
* Lenovo H8-based ThinkPads gained FnLock and logo LED control
  (b01c7d6642, cf541343a9), alongside improved ThinkLight, keyboard
  backlight and primary Fn-key handling (07981ca1c0, 6078f59a7c).
  Additionally, ACPI battery power units were fixed and basic charge
  behaviour was added to supported models (a06042792d, b247bf183d).
* Star Labs boards picked up CFR-based power-profile and per-port PCIe
  power-management controls (046a35f84f, 6f3c6558f3, 21c424ae9a).
* In-Band ECC (IBECC) can now be controlled at runtime: the FSP enable
  bit is driven from an option backend rather than only the devicetree
  on Alder Lake, Elkhart Lake, Meteor Lake, and Tiger Lake
  (c2d06ca3be), and Star Labs boards expose it through CFR (0c555f171d).
* Apollo Lake and Gemini Lake gained a native IFWI stitcher that
  assembles the final flash image from Kconfig-selected descriptor, CSE
  image, and signing inputs using OpenSSL, without invoking Intel FIT or
  MEU (00793280d1).
* AMD platforms can keep the ACP XTAL running in S0i3 via a new Kconfig,
  allowing wake from sources such as a microphone (241287f596,
  41f67ad2a7, 7d474cda77), with Jaguar opting in (8df29f19d7).
* MediaTek common code added a shared resource mutex for DMA safety
  (5b49e6d976), and the RISC-V QEMU target now initializes its PCI root
  bus (f1e95c5536).
* Qualcomm common code received major updates, adding support for PCIe
  Gen5 (687780fca9), delta DCB loading (589ff5b60b), and asynchronous
  PCIe initialization.
* The Qualcomm X1P42100 SoC matured substantially, gaining eDP display
  support (c6e0f28814), runtime SoC identification (574166edf9), support
  for both Hamoa and X1P42100 firmware blobs in CBFS (2ac9b3d715), and
  PCIe endpoint power-off (2a6b546ca2).
* amdfwtool saw a large cleanup and modernization pass: combo support
  was dropped (c3a82354fa), the config file and `--location` argument
  became mandatory (8c7c9ad484, 02579e43be), platform-specific quirks
  were moved into a dedicated file (5ffe9ff224), AIFv1 BIOS directory
  tables are now supported (c3e0cb75fa), and the Mullins platform was
  added (e9487cafa0). amdfwread additionally gained UEFI image parsing
  (2c43cd5be0).
* Retro-enablement continued with the addition of support for the Intel
  GM965 northbridge (Core 2 Duo era), including MRC cache for fast boot
  and SMBIOS memory information setup (6457201fe6, c094305a92).
* Numerous platform-specific enhancements across Intel, AMD, MediaTek,
  and Qualcomm SoCs, including additional setup options,
  power-management tuning, and memory-initialization fixes.
* The libpayload library saw arm64 fixes, adding the pre-allocated
  framebuffer to its used-memory ranges (5c77449b8b) and correcting an
  unrecognized linker option in its Makefile (82de37d171).



Changes to external resources
-----------------------------

### Toolchain updates

* Update GCC from 14.2.0 to 15.2.0 (e518885dce)
* Update NASM from 2.16.03 to 3.01 (5267cae13a), with a fix for a
  parallel-build race in the nasm crossgcc build (ced4028bff)
* Update binutils from 2.45 to 2.45.1 (b6ca7755f3)
* Update ACPICA from 20250807 to 20251212 (3ef459a968)


### Git submodule pointers

* amd_blobs: Update from commit id 0a6d270fb3 to 588da8d6de (2 commits)
* fsp: Update from commit id 81399b3b61 to ca4f8b702d (13 commits)
* intel-microcode: Update from commit 250941fb67 to 98f8d817ca (1
  commit)


### External payloads

* edk2 builds a local FMP certificate PCD in support of capsule updates
  (4064b5de37), with the serial driver disabled when serial output is
  off (f71e7624c4), and the default MrChromebox branch was updated from
  2511 to 2603 (25d3809ea3).



Platform Updates
----------------

### Added mainboards

* AMD Crater for V2000A SoC
* AMD Jaguar for Faegan SoC
* AMD Maple for Strix Halo SoC
* ASRock H370M-ITX/ac
* ASRock Z87 Extreme6
* ASUS H81M-K
* ASUS Maximus VI EXTREME
* ASUS Maximus VI FORMULA
* ASUS Maximus VI HERO
* ASUS Maximus VII IMPACT
* ASUS Maximus VI IMPACT
* ASUS Maximus VII RANGER
* ASUS P8H61-I R2.0
* ASUS P8H61-M LX2
* ASUS PRIME H610M-K D4
* ASUS Z87-K
* Framework Laptop 13 (Intel Core Ultra Series 1)
* Framework Laptop 13 Pro (Intel Core Ultra Series 3)
* Google Atria
* Google C1nv
* Google Calypso
* Google Dirkson
* Google Mensa
* Google Penghu
* Google R2d2
* Google Sheev
* Lenovo ThinkPad X61 / X61s
* Star Labs Byte Mk I (Ryzen 7 5800U)
* Star Labs StarBook Mk VI (Ryzen 7 5800U)
* System76 bonw15-b
* System76 gaze20


### Removed mainboards

* AMD Crater (Renoir) — replaced by AMD Crater for V2000A SoC
* Google Myst


## SoC Changes

* added amd/strix_halo
* added intel/novalake
* added intel/pantherlake
* added qualcomm/calypso



Statistics from the 26.03 to the 26.06 release
--------------------------------------------

* Total Commits: 1163
* Average Commits per day: 12.69
* Total lines added: 139996
* Average lines added per commit: 120.37
* Number of patches adding more than 100 lines: 118
* Average lines added per small commit: 37.83
* Total lines removed: 20972
* Average lines removed per commit: 18.03
* Total difference between added and removed: 119024
* Total authors: 101
* New authors: 22


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
