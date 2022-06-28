* __abuild__ - coreboot autobuild script builds coreboot images for all
available targets. `bash`
* __acpi__ - Walk through all ACPI tables with their addresses. `bash`
* __amdfwtool__ - Create AMD Firmware combination `C`
* __amdtools__ Various tools for AMD processors
	* _update_efs_spi_speed_ - Change SPI speed in binary. `Bash`
	* Tools to compare extended K8 memory settings.
	* _k8-compare-pci-space.pl_ - Shows differences between values
in PCI space and the default value. `Perl`
	* _k8-interpret-extended-memory-settings.pl_ - Shows
differences between memory controller values and the default value.
`Perl`
	* _k8-read-mem-settings.sh_ - Makes data files understood by
the k8-interpret-extended-memory-settings script. `Bash`
	* _parse-bkdg.pl_ - Make bkdg.data file used by above scripts.
`Perl`
	* _example_input_ - Sample input for the above scripts. `Text`
* __apcb__ - AMD PSP Control Block tools
	* _apcb_edit.py_ - This tool allows patching an existing APCB
binary with specific SPDs and GPIO selection pins. `Python3`
	* _apcb_v3_edit.py_ - This tool allows patching an existing
APCB v3 binary with up to 16 specific SPDs. `Python3`
* __archive__ - Concatenate files and create an archive `C`
* __autoport__ - Automated porting coreboot to Sandy Bridge/Ivy Bridge
platforms `Go`
* __bincfg__ - Compiler/Decompiler for data blobs with specs `Lex`
`Yacc`
* __board_status__ - Tools to collect logs and upload them to the board
status repository `Bash` `Go`
* __bucts__ - A tool to manipulate the BUC.TS bit on Intel targets. `C`
* __cavium__ - Devicetree_convert Tool to convert a DTB to a static C
file `Python`
* __cbfstool__
	* _cbfstool_ - For manipulating CBFS file `C`
	* _fmaptool_ - Converts plaintext fmd files into fmap blobs `C`
	* _rmodtool_ - Creates rmodules `C`
	* _ifwitool_ - For manipulating IFWI `C`
* __cbmem__ - CBMEM parser to read e.g. timestamps and console log `C`
* __chromeos__ - These scripts can be used to access ChromeOS
resources, for example to extract System Agent reference code and other
blobs (e.g. mrc.bin, refcode, VGA option roms) from a ChromeOS
recovery image. `C`
* __crossgcc__ - A cross toolchain builder for -elf toolchains (ie. no
libc support) `Bash`
* __docker__ - Dockerfiles for _coreboot-sdk_, _coreboot-jenkins-node_,
_coreboot.org-status_ and _docs.coreboot.org_ `Make`
* __dtd_parser__ - DTD structure parser `Python2`
* __ectool__ - Dumps the RAM of a laptop's Embedded/Environmental
Controller (EC). `C`
* __exynos__ - Computes and fills Exynos ROM checksum (for BL1 or BL2).
`Python3`
* __find_usbdebug__ - Help find USB debug ports `Bash`
* __futility__ - Firmware utility for signing ChromeOS images `Make`
* __fuzz-tests__ - Create test cases that crash the jpeg code. `C`
* __genbuild_h__ - Generate build system definitions `Shell`
* __gitconfig__ - Initialize git repository submodules install git
hooks `Bash`
* __ifdtool__ - Extract and dump Intel Firmware Descriptor information
`C`
* __intelmetool__ - Dump interesting things about Management Engine
even if hidden `C`
* __intelp2m__ - convert the configuration DW0/1 registers value from
an inteltool dump to coreboot macros. `go`
* __inteltool__ - Provides information about the Intel CPU/chipset
hardware configuration (register contents, MSRs, etc). `C`
* __intelvbttool__ - Parse VBT from VGA BIOS `C`
* __ipqheader__
	* _createxbl.py_ - Concatenates XBL segments into one ELF image
`Python`
	* _ipqheader.py_ - Returns a packed MBN header image with the
specified base and size `Python`
	* _mbncat.py_ - Generate ipq8064 uber SBL `Python`
	* *mbn_tools.py* - Contains all MBN Utilities for image
generation `Python`
* __kbc1126__ - Tools used to dump the two blobs from the factory
firmware of many HP laptops with 8051-based SMSC KBC1098/KBC1126
embedded controller and insert them to the firmware image. `C`
* __kconfig__ - Build system `Make`
* __lint__ - Source linter and linting rules `Shell`
* __liveiso__ - A script and NixOS configuration files to create an ISO
image for testing purposes and for working on firmware. `Bash`
* __mainboard__ - mainboard specific scripts
	* _google_ - Directory for google mainboard specific scripts
* __marvell__ - Add U-Boot boot loader for Marvell ARMADA38X `C`
* __[me_cleaner](https://github.com/corna/me_cleaner)__ - Tool for
partial deblobbing of Intel ME/TXE firmware images `Python`
* __mma__ - Memory Margin Analysis automation tests `Bash`
* __msrtool__ - Dumps chipset-specific MSR registers. `C`
* __mtkheader__ - Generate MediaTek bootload header. `Python3`
* __nvidia__ - nvidia blob parsers
* __nvramtool__ - Reads and writes coreboot parameters and displaying
information from the coreboot table in CMOS/NVRAM. `C`
* __pmh7tool__ - Dumps, reads and writes PMH7 registers on Lenovo
ThinkPads. PMH7 is used for switching on and off the power of some
devices on the board such as dGPU. `C`
* __post__ - Userspace utility that can be used to test POST cards. `C`
* __qemu__ - Makefile & comprehensive default config for QEMU Q35
emulation `Make`
* __qualcomm__ - CMM script to debug Qualcomm coreboot environments.
`CMM`
* __release__ - Generate coreboot release `Bash`
* __riscv__
	* _make-spike-elf.sh_ - Converts a flat file into an ELF, that
can be passed to SPIKE, the RISC-V reference emulator.`Bash`
	* _sifive-gpt.py_ - Wraps the bootblock in a GPT partition for
SiFive's bootrom. `Python3`
* __rockchip__ - Generate Rockchip idblock bootloader. `Python3`
* __sconfig__ - coreboot device tree compiler `Lex` `Yacc`
* __scripts__
	* _config_ - Manipulate options in a .config file from the
command line `Bash`
	* _cross-repo-cherrypick_ - Pull in patches from another tree
from a gerrit repository. `Shell`
	* _decode_spd.sh_ - Decodes Serial Presence Detect (SPD) files
into various human readable formats. `Bash`
	* _dts-to-fmd.sh_ -Converts a depthcharge fmap.dts into an
fmaptool compatible .fmd format `Bash`
	* _find-unused-kconfig-symbols.sh_ - Points out Kconfig
variables that may be unused. There are some false positives, but it
serves as a starting point `Shell`
	* _gerrit-rebase_ - Applies all commits that from-branch has
over to-branch, based on a common ancestor and gerrit meta-data `Bash`
	* _get_maintainer.pl_ - Print selected MAINTAINERS information
for the files modified in a patch or for a file `Perl`
	* _maintainers.go_ - Build subsystem Maintainers `Go`
	* _no-fsf-addresses.sh_ - Removes various FSF addresses from
license headers `Shell`
	* _parse-maintainers.pl_ - Script to alphabetize MAINTAINERS
file `Perl`
	* _ucode_h_to_bin.sh_ - Microcode conversion tool `Bash`
	* _update_submodules_ - Check all submodules for updates `Bash`
* __showdevicetree__ - Compile and dump the device tree `C`
* __spdtool__ - Dumps SPD ROMs from a given blob to separate files
using known patterns and reserved bits. Useful for analysing firmware
that holds SPDs on boards that have soldered down DRAM. `python`
* __spkmodem_recv__ - Decode spkmodem signals `C`
* __superiotool__ - A user-space utility to detect Super I/O of a
mainboard and provide detailed information about the register contents
of the Super I/O. `C`
* __supermicro__ - Tools for supermicro platforms
	* _smcbiosinfo_ - Generates SMC biosinfo for BMC BIOS updates
`C`
* __testing__ - coreboot test targets `Make`
* __uio_usbdebug__ - Debug coreboot's usbdebug driver inside a running
operating system (only Linux at this time). `C`
* __util_readme__ - Creates README.md of description files in `./util`
subdirectories `Bash`
* __vboot_list__ - Tools to generate a list of vboot enabled devices to
the documentation `Bash`
* __vgabios__ - emulated vga driver for qemu `C`
* __x86__ - Generates 32-bit PAE page tables based on a CSV input file.
`Go`
* __xcompile__ - Cross compile setup `Bash`
