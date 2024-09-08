coreboot 4.2 release notes
==========================

Halloween 2015 release - just as scary as that sounds

Dear coreboot community,
today marks the release of coreboot 4.2, the second release on our time
based release schedule. Since 4.1 there were 936 commits by 90 authors,
increasing the code base by approximately 17000 lines of code. We saw 35
new contributors - welcome to coreboot! More than 34 developers were
active as reviewers in that period. Thanks go to all contributors who
helped shape this release.

As with 4.1, the release tarballs are available at
<http://www.coreboot.org/releases/>. There's also a 4.2 tag and branch in
the git repository.

This marks the first release that features a changelog comparing it to
the previous release. There was some limited testing to make sure that
the code is usable, and it boots on some devices. A structured test plan
will only become part of the release procedure of future versions. I'm
grateful to Martin for assembling this release's changelog.

This is also the first release that will be followed by the removal of
old, unused code. There will be a policy on how to announce deprecation
and removal of mainboard and chipset code for future releases.

Regards,
Patrick

Log of commit d5e6618a4f076610e683b174c4dd5108d960c785 to
commit 439a527014fa0cb3e4ef60ba59e5c57c737b4444

Changes between 4.1 and 4.2
---------------------------

### Build system:
* Store a minimized coreboot config file in cbfs instead of the full
  config
* Store the payload config and revision in CBFS when that info is
  available
* Add -compression option for cbfs-files-y. Valid entries are now -file,
  -type, -align, and -compression
* Change Microcode inclusion method from building .h files to pre-built
  binaries
* Update Builder tests for each commit to test utilities and run lint
  tools
* Many other small makefile and build changes and fixes
* Remove expert mode as a Kconfig option

### Utilities:
* Many fixes and updates to many utilities (158 total commits)
* ifdtool: Update for skylake, handle region masks correctly
* crossgcc: Update to gcc 5.2.0
* kconfig: Add strict mode to fail on kconfig errors and warnings
* vgabios: Significant fixes to remove issues in linking into coreboot
  code
* Add script to parse MAINTAINERS file
* Add Kconfig lint tool
* Create a common library to share coreboot routines with utilities

#### Significant changes and cleanup to cbfstool (81 commits)
* Update cbfstool to change the internal location of FSP binaries when
  adding them
* Decompress stage files on extraction and turn them into ELF binaries
* Header sizes are now variable, containing extended attributes
* Add compression tags to all cbfs headers so all cbfs files can be
  compressed
* Add and align CBFS components in one pass instead of two
* Add XIP support for X86 to relocate the romstage when it'™s added
* Removed locate command as it'™s no longer needed
* Add bootblock and cbfs_header file types so the master header knows
  about them
* Prefer FMAP data to CBFS master header if FMAP data exists
* Add hashes to cbfs file metadata for verification of images

### Payloads:
* SeaBIOS: update stable release from 1.7.5 to 1.8.2
* Libpayload had some significant changes (61 commits). Major changes:
* Add support for fmap tables
* Add support for SuperSpeed (3.0) USB hubs
* Updates and bugfixes for DesignWare OTG controller (DWC2)
* Add video_printf to print text with specified foreground & background
  colors
* Updates to match changes to cbfs/cbfstool
* Add cbgfx, a library to show graphics and text on a display
* Read cbfs offset and size from sysinfo when available

### Vendorcode:
* fsp_baytrail: Support Baytrail FSP Gold 4 release
* AMD binary PI: add support for fan control
* Work to get AMD AGESA to compile correctly as 64-bit code
* Add standalone (XIP) verstage support for x86 to verify romstage

### Mainboards:
* New Mainboards:
* apple/macbookair4_2 * Sandy/Ivy Bridge with Panther / Cougar point
chipset
* asus/kgpe-d16 - AMD Family 10, SB700/SR5650 platform
* emulation/spike-riscv - RISCV virtualized platform
* google/chell - Intel Skylake chrome platform
* google/cyan - Intel Braswell chrome platform
* google/glados - Intel Skylake chrome platform
* google/lars - Intel Skylake chrome platform
* intel/kunimitsu - Intel Skylake chrome platform
* intel/sklrvp - Intel Skylake reference platform
* intel/strago - Intel Braswell chrome platform
* Cleanups of many mainboards - several patches each for:
* amd/bettong
* getac/p470
* google/auron, google/smaug and google/veyron_rialto
* pcengines/apu1
* siemens/mc_tcu3
* Combine the google/veyron_(jerry, mighty, minnie, pinkie, shark &
  speedy) mainboards into the single google/veyron mainboard directory

### Console:
* Add EM100 ˜hyper term" spi console support in ramstage & smm
* Add console support for verstage

### ARM:
* armv7: use asm coded memory operations for 32/16 bit read/write
* Many cleanups to the nvidia tegra chips (40 patches)

### RISC-V:
* Add trap handling
* Add virtual Memory setup

### X86:
* Remove and re-add Rangeley and Ivy Bridge / panther point FSP
  platforms
* Update microcode update parser to use stock AMD microcode blobs from
  CBFS
* ACPI: Align FACS to 64 byte boundary. Fixes FWTS error
* AMD/SB700: Init devices in early boot, restore power state after power
  failure. Add IDE/SATA asl code
* Add initial support for AMD Socket G34 processors
* Add tick frequency to timestamp table to calculate boot times more
  accurately
* Unify X86 romstage / ramstage linking to match other platforms
* Start preparing X86 bootblock for non-memory-mapped BIOS media
* cpu/amd/car: Add Suspend to RAM (S3) support
* Native VGA init fixes on several platforms
* Significant updates to FSP 1.1 code for cleanup and cbfstool changes
* SMMhandler: on i945..nehalem, crash if LAPIC overlaps with ASEG to
  prevent the memory sinkhole smm hack

### Drivers:
* Add native text mode support for the Aspeed AST2050
* w83795: Add support for for fan control and voltage monitoring
* Intel GMA ACPI consolidation and improvements
* Set up the 8254 timer before running option ROMs
* Resource allocator: Page align memory mapped PCI resources

### Lib:
* Derive fmap name from offset/size
* Several edid fixes
* Updates to cbfs matching changes in cbfstool

Submodules:
----------
### 3rdparty/blobs:
Total commits: 16
Log of commit 61d663e3 to commit aab093f0
* AMD Merlin Falcon: Update to CarrizoPI 1.1.0.0 (Binary PI 1.4)
* AMD Steppe Eagle: Update to MullinsPI 1.0.0.A (Binary PI 1.1)
* Update microcode to binary blobs. Remove old .h microcode files

### 3rdparty/arm-trusted-firmware:
* No Changes

### 3rdparty/vboot:
Total commits: 41
Log of commit fbf631c8 to commit d6723ed1
* Update the code to determine the write protect line gpio value
* Several updates to futility and image_signing scripts
* Update crossystem to accommodate Android mosys location
* Support reboot requested by secdata
* Add NV flag to default boot legacy OS

### util/nvidia/cbootimage:
* No Changes
