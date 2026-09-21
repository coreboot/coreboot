Upcoming release - coreboot 26.09
========================================================================

The coreboot project is pleased to announce the release of coreboot
26.09. In the three months since the 26.06 release, 112 authors
(including 28 contributors landing their first patches) merged 1251
commits across the tree. We thank everyone who wrote code, reviewed
patches, tested on hardware, or reported bugs during this cycle.

While recent releases have heavily featured new silicon enablement, this
cycle focuses on hardening and refining the existing codebase, alongside
notable feature work such as shared CFR setup menus, Intel MIPI camera
ACPI generation for non-ChromeOS OS stacks, and continued AMD Turin
bring-up. A significant amount of work went into teaching the compiler
to validate LPC decode windows, replacing opaque audio verb tables with
structured alternatives, moving coreboot firmware and most host-side
builds to the C23 standard, and extensively auditing parsers for
external data formats.

These foundational improvements do not always show up in a boot log, but
they are exactly what keeps a long-standing project healthy and
reliable. The next coreboot release, 26.12, is planned for the middle of
December.


Significant or interesting changes
----------------------------------

### treewide: Input validation and general hardening

Several externally supplied formats that coreboot parses received an audit
pass this cycle. The EDID parser was updated to reject short EDID buffers
(0c2df4db29) and prevent reading extension blocks past the buffer end
(e2f1db04e9). It also now checks the extension block length inside parse_cea
(d5644158dc) and ignores failed extension blocks (b1bc8c6e71). In the
commonlib device tree parsing, we validate FDT header self-consistency
(190e6467c8) and header block alignments (61d99ab994). The parser will now
reject FDT versions 16 and below (44138ac188) as well as unflattened trees
with no root (8eedb4334e).

Image rendering and storage also saw improvements. The BMP renderer now
validates BMP dimensions and ImageOffset bounds (8589952afa), and
validates framebuffer geometry bounds before rendering (6ef8ad909f). For
storage, the CBFS walker rejects file headers with a too-small data offset
(e95bdb7eee), and the FMAP code validates area offset and size in
fmap_locate_area() (00d88bed4e). On the SMM side, the bus walk recursion
was modified to prevent cyclic recursion (034f4edca5), and the MP init
code will halt the boot on SMM initialization failure (297a51a071). Other
hardening includes validating the payload size in TPM2 responses
(d7a3dfc1f1) and checking for ramstage heap overflow in crashlog
(899a40fc9b).

### lib,intel: DDR5 SPD cache and TPM-backed verification

Intel platforms gained DDR5 SPD cache support so DIMM serials and
geometry can be reused across boots without re-reading SMBus
(143cab8eb9), with follow-up size-check fixes in the common SMBus path
(0fb58baf02). Separately, `SPD_CACHE_TPM_HASH` stores a SHA-256 of the
SPD cache in TPM NVRAM and invalidates the cache on mismatch, forcing a
clean MRC retrain if RW flash was tampered with (03903a857f). Google
Ocelot variants began opting into the TPM-hash path (19c96135c1,
5384dc480a).

### build: Firmware and host builds move to C23

The coreboot firmware build has officially moved to the -std=gnu23
standard (e9f2a15a05). Mainboard code was subsequently updated to replace
_Static_assert with the standard C23 static_assert keyword (88c1fac546).
Host tooling followed suit, with tools like ifdtool and cbmem also building
with -std=gnu23 (456d48e69e). Note that host builds now require GCC 14 or
newer, or Clang 18 or newer.

### sb,soc/intel: LPC generic decode ranges are now compiler-checked

LPC generic decode ranges are now properly validated. We added an LPC_IO()
helper for GENx_DEC values to the common southbridge (7e16e70c55) and SoC
(819cd9bec3) LPC code. Invalid LPC_IO() values are now rejected at compile
time (4ef247411a), and a new lint check requires using the LPC_IO() macro
for gen*_dec fields in device trees (99a6a8518f).

### arch/x86: i8259 and virtual-wire ExtINT setup rework

The virtual wire EXTINT setup was improved to enable it only after the
i8259 initialisation (afa234c6c8), and ExtINT is now kept masked while
initializing the i8259 (64bbfac246). We initialized the i8259 on the
remaining PC/AT paths (27aae20c29). Additionally, we added a NO_PCAT_8259
policy to gate PC/AT 8259 setup (bce0c6ad5a) for platforms that lack a
PC/AT PIC pair.

### device,mb: Azalia verb tables become readable

Audio verb tables received a major cleanup to make them readable. We added
support for 4-bit verbs (942000193a) and switched to using weak functions
to get azalia codecs (c583b0c4f8). Proper enums were added for the Realtek
ALC1150 (11ef0fac53) and ALC662/892/1200 (85707dc6a0) codecs. Many ASRock
Z87 and Z97 boards were updated to use these human-readable verb
descriptions (1f6fa17e4a).

### drivers,ec,console: Shared CFR menus and runtime serial console

Setup-option coverage via CFR expanded substantially this cycle. Lenovo
H8/PMH7 boards now get a shared EC firmware menu automatically
(2f96e1d90e), and ChromeEC gained the same pattern so every CFR-enabled
ChromeEC board picks up a consolidated EC form without per-board copies
(9916da47e9). On the chipset side, bd82x6x grew a shared Intel ME status
and control menu (6f581f4808), with common north/southbridge options for
Arrandale-era parts (ce0b89b178) and shared Hyper-Threading controls
(b363cc3237, 4a930e7cf3).

The console subsystem can now enable the serial console at runtime
(605cba2fd1) and auto-publishes a shared CFR Console form
(5d76acd9aa), dropping per-board debug-level wiring (8da7079e50).
Asset-tag CFR support was added as a generic driver (ac094eb2b7), and
CFR was wired up or enabled by default across LattePanda Mu, Framework,
cyan, x201/x230, and several edk2-default configs.

### drivers/intel/mipi_camera: ACPI generation for IPU3 camera stacks

The MIPI camera driver grew a PMIC SSDT generator and richer ACPI binding
support aimed at the Windows/Linux driver stack for IPU3 cameras: `_DEP`
(c45d969667), external `_STA` objects (be4ad536b2), reset-once PMIC
PowerResources (9160dfa147), graph `_DSD` and GPIO `_DSM` (c74e3c9718),
SSDB lane derivation (3598a525ec), and local PRIC alongside INT3472
(cc36de97cc). CIO2 port/`_DSD` emission was scoped into the existing
device (3321f6013c), with Skylake CIO2 naming and static-child scanning
(2f8349943b, d019cf7fcd).

Google Poppy variants (atlas, nautilus, soraka) moved from static ASL to
the MIPI devicetree path (603c189906, b993b3026f, a3a4d23b2c), and an
ACPI programming guide was added under Documentation/drivers
(e3ec4f1fc0).

### drivers/efi: Capsule-on-disk support and an SMMSTORE cleanup

The EFI payload infrastructure gained support for capsule-on-disk
(56535e2783), and edk2 payloads are now configured to support it
(dc67faf538). The capsule driver rejects invalid memory ranges (bd50f39597)
and allows the driver to be called multiple times (db6aeea755). SMMSTORE
dropped its v1 protocol support (52fd778a78) and made its block size
configurable (01a26951a6).

### soc/amd/turin: Turin PoC matures; first EPYC board lands

AMD Turin continued past the skeleton stage. The PoC SoC picked up CPPC
support (c21b6f255d), IOAPIC interrupt-routing hooks (9a3ca0c141), FADT
and amdfwtool configuration (bfddb4ad78, b53955450c), and ACPI
descriptions for CXL and MPDMA devices (d0a055ae78, 858049a7ae). SEV
NVRAM was hooked into the PSP directories (a8dba0849e), with matching
amdfwtool and EFS work for Genoa/Turin (721453b7ff, 0fd91dc227,
f0194c62ba, c14cf399e3).

The first board on this path is the GIGABYTE MZ33-AR1, added with PCIe
slot/MCIO topology, on-board device config, and BMC BIOS-update packing
(0754b3ea38, 32b4cddd27, 737480a0da, a53c63b814). Common AMD FCH paths
(AOAC, UART, SMI sleep) were unified alongside this work (80fade0ec4,
c8d0d2591a, fb11c51fb7).

### Documentation: AI review policy and AGENTS.md

coreboot adopted an explicit policy for AI-assisted code review
(b34a744391). The Gerrit Guidelines now describe when and how AI may be
used on patches, and a top-level `AGENTS.md` encodes the same rules for
coding agents: comments must be marked `[AI-generated]`, may only raise
new top-level issues (never replies to humans), must not set review
flags or drive Gerrit actions, and must follow the project coding style.


Additional coreboot changes
---------------------------

* util/cbfstool: FMD gained an `EXT` flag so CBFS regions can be extended
  onto alternate storage as `ECBFS_<name>` siblings (a80a718fd9), and
  fmaptool enumerates those extended regions for cbfstool (db23ba0439).
* soc/qualcomm/calypso: Add Calypso RPMh RSC driver (f9617dba37), display
  subsystem initialization (74410e2a00), and TSENS controllers and thermal
  zones (e00170ceca).
* soc/mediatek/mt8196: Enable EOSC hardware calibration for RTC accuracy
  (beb40bd594) and break unclocked CBUSY deadlock during MT6685 RTC init
  (f7c6229c61).
* soc/amd: Add BOCO (Bus Off Chip Off) support (90157e8977).
* soc/amd/common/block/pci: Add PCIe hotplug support (44f4416b63).
* soc/amd/common/block/psp: Add SMM A/B recovery support (0c62b9d21e).
* util/amdfwtool: Add multi-PSP support (56fee2ba34) and support for
  SEV NVRAM (c14cf399e3).
* soc/intel/pantherlake: Add configurable TDP support (f57c27b496) and
  dynamically configure DDR5 memory controller topology (ade6bd7e87).
* soc/intel/novalake: Enable coreboot native logo rendering (1f8931aceb)
  and add TDC, power limits, and power state thresholds (538e724405).
* mb/starlabs: Share identical mainboard sources (8dcd09d131) and move
  shared settings to common files (c2d000ba36).
* ec/starlabs/merlin: Generate battery ACPI with acpigen (9810f4a5ef) and
  generate sleep hooks with acpigen (3c77e34b52).
* ec/lenovo/h8: Add keyboard backlight hardware detection (30b8105b0b) and
  ACPI method _BIX (493593a1b8).
* nb/intel/pineview: Add DDR2 UDIMM Vref margining (c96952c3e4) and add
  libgfxinit support (9c6cb93982).
* drivers/wifi: Support Selectively Wi-Fi Bands (3e81074243) and add DSM
  function 8 'Device for Indoor Use Only' support (61394c0a43).
* mb/framework: Enable vboot on all boards (26450cca94), unify EC code
  across mainboards (4c9e99d07d), and add thermal zones (3449b5e01a).
* mb/lattepanda/mu: Split into baseboard (SOM) and lite carrier variant
  (6bb3ad5902).
* mb/sapphire/rave2: Add Rave2 mainboard (41f44c3a35) and add PCIe/DDI
  topology and enable NVMe/WiFi/OcuLink (8e3ba74abe).


Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade binutils from version 2.45.1 to 2.46.1 (6372edd871)
* Upgrade LLVM from version 21.1.8 to 22.1.8 (30870ce128)
* Upgrade NASM from version 3.01 to 3.02 (c165edb4ce)
* Upgrade CMake from version 4.0.3 to 4.3.4 (560771f617)
* Upgrade MPC from version 1.3.1 to 1.4.1 (56fc96dcac)
* Add Cortex-R and Cortex-M multilib support to crossgcc (54bcc595c0)

### Git submodule pointers

* arm-trusted-firmware: Update from commit id 9109143417 to b5eaba47ef
* blobs: Update from commit id 4a8de0324e to 6f42f0f39b
* fsp: Update from commit id ca4f8b702d to 98426bfd95
* intel-microcode: Update from commit id 98f8d817ca to 927e65c8d5
* libgfxinit: Update from commit id 7e08e5d280 to 6e74c70313
* libhwbase: Update from commit id 5bd7c1101c to 61f7287f18

### External payloads

* payloads/external/CrabEFI: Add CrabEFI payload support (ff34dafa38).
  CrabEFI is a UEFI implementation written in Rust.
* payload/grub2: Update GRUB2 to 2.14 (bcb8f01241).
* payloads/edk2: Configure capsule-on-disk support (dc67faf538) and bump
  MrChromebox default revision to 26.09.1 (db23298e06).
* libpayload: Parse soc_id from coreboot tables (6b028355f7) and recreate
  MSC disk if geometry changes (0e4ed28002).


Platform Updates
----------------

### Added mainboards:

* Acer TravelMate P633-M
* Acer TravelMate P633-V
* Apple iMac4,1
* ASRock Fatal1ty Z97X Killer
* ASRock Z97 Anniversary
* ASUS B85M-E
* ASUS Maximus VI GENE
* ASUS Maximus VII GENE
* ASUS Maximus VII HERO
* Framework Laptop 12 (13th Gen Intel Core)
* GIGABYTE MZ33-AR1
* Google Aneto
* Google Annite
* Google Hekla
* Google Meru
* Google Pic
* Intel NUC D33217GKE
* LattePanda Mu with DFR1141 full eval carrier
* LattePanda Mu with DFR1142 lite carrier
* Lenovo ThinkPad X380 Yoga
* Lenovo ThinkStation P320
* MSI PRO B760-P WIFI DDR4 (MS-7D98)
* NVIDIA Porg (Jetson Nano)
* Qotom Q535G6
* Sapphire Rave2 (Faegan)
* Siemens FA ASL


Statistics from the 26.06 to the 26.09 release
--------------------------------------------

* Total Commits: 1310
* Average Commits per day: 13.38
* Total lines added: 112657
* Average lines added per commit: 86.00
* Number of patches adding more than 100 lines: 145
* Average lines added per small commit: 40.55
* Total lines removed: 27467
* Average lines removed per commit: 20.97
* Total difference between added and removed: 85190
* Total authors: 112
* New authors: 28


coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
