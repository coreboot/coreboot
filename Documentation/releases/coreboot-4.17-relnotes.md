coreboot 4.17
========================================================================


The coreboot 4.17 release is being done on June 1, 2022.

Since the 4.16 release, we've had over 1260 new commits by around 150
contributors.  Of those people, roughly 15 were first-time contributors.

As always, we appreciate everyone who has contributed and done the hard
work to make the coreboot project successful.


Major Bugfixes in this release
------------------------------
* [CVE-2022-29264](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2022-29264)


New Mainboards
--------------

* Clevo L140MU / L141MU / L142MU
* Dell Precision T1650
* Google Craask
* Google Gelarshie
* Google Kuldax
* Google Mithrax
* Google Osiris
* HP Z220 CMT Workstation
* Star Labs LabTop Mk III (i7-8550u)
* Star Labs LabTop Mk IV (i3-10110U and i7-10710U)
* Star Labs Lite Mk III (N5000)
* Star Labs Lite Mk IV (N5030)


Removed Mainboards
------------------

* Google Deltan
* Google Deltaur

Significant or interesting changes
----------------------------------

These changes are a few that were selected as a sampling of particularly
interesting commits.


### CBMEM init hooks changed

Instead of having per stage x_CBMEM_INIT_HOOK, we now have only 2 hooks:
* CBMEM_CREATION_HOOK: Used only in the first stage that creates cbmem,
  typically romstage. For instance code that migrates data from cache
  as ram to dram would use this hook.
* CBMEM_READY_HOOK: Used in every stage that has cbmem. An example would
  be initializing the cbmem console by appending to what previous stages
  logged.
The reason for this change is improved flexibility with regards to which
stage initializes cbmem.


### Payloads

* SeaBIOS: Update stable release from 1.14.0 to 1.16.0
* iPXE: Update stable release from 2019.3 to 2022.1
* Add "GRUB2 atop SeaBIOS" aka "SeaGRUB" option, which builds GRUB2 as a
  secondary payload for SeaBIOS with GRUB2 set as the default boot
  entry.  This allows GRUB2 to use BIOS callbacks provided by SeaBIOS as
  a fallback method to access hardware that the native GRUB2 payload
  cannot access.
* Add option to build SeaBIOS and GRUB2 as secondary payloads
* Add new coreDOOM payload.  See commit message below.


### payloads/external: Add support for coreDOOM payload

coreDOOM is a port of DOOM to libpayload, based on the doomgeneric
source port. It renders the game to the coreboot linear framebuffer,
and loads WAD files from CBFS.


### cpu/x86/smm_module_load: Rewrite setup_stub

This code was hard to read as it did too much and had a lot of state
to keep track of.

It also looks like the staggered entry points were first copied and
only later the parameters of the first stub were filled in. This
means that only the BSP stub is actually jumping to the permanent
smihandler. On the APs the stub would jump to wherever c_handler
happens to point to, which is likely 0. This effectively means that on
APs it's likely easy to have arbitrary code execution in SMM which is a
security problem.

Note: This patch fixes CVE-2022-29264 for the 4.17 release.


### cpu/x86/smm_module_loader.c: Rewrite setup

This code is much easier to read if one does not have to keep track of
mutable variables.

This also fixes the alignment code on the TSEG smihandler setup code.
It was aligning the code upwards instead of downwards which would cause
it to encroach a part of the save state.


### cpu/x86/smm: Add sinkhole mitigation to relocatable smmstub

The sinkhole exploit exists in placing the lapic base such that it
messes with GDT. This can be mitigated by checking the lapic MSR
against the current program counter.


### cpu/x86/64bit: Generate static page tables from an assembly file

This removes the need for a tool to generate simple identity pages.
Future patches will link this page table directly into the stages on
some platforms so having an assembly file makes a lot of sense.

This also optimizes the size of the page of each 4K page by placing
the PDPE_table below the PDE.


### cpu/x86/smm,lib/cbmem_console: Enable CBMEMC when using DEBUG_SMI

This change will allow the SMI handler to write to the cbmem console
buffer. Normally SMIs can only be debugged using some kind of serial
port (UART). By storing the SMI logs into cbmem we can debug SMIs using
'cbmem -1'. Now that these logs are available to the OS we could also
verify there were no errors in the SMI handler.

Since SMM can write to all of DRAM, we can't trust any pointers
provided by cbmem after the OS has booted. For this reason we store the
cbmem console pointer as part of the SMM runtime parameters. The cbmem
console is implemented as a circular buffer so it will never write
outside of this area.


### security/tpm/crtm: Add a function to measure the bootblock on SoC level

On platforms where the bootblock is not included in CBFS anymore
because it is part of another firmware section (IFWI or a different
CBFS), the CRTM measurement fails.

This patch adds a new function to provide a way at SoC level to measure
the bootblock. Following patches will add functionality to retrieve the
bootblock from the SoC related location and measure it from there.
In this way the really executed code will be measured.


### soc/amd/common/block/psp: Add platform secure boot support

Add Platform Secure Boot (PSB) enablement via the PSP if it is not
already enabled. Upon receiving psb command, PSP will program PSB fuses
as long as BIOS signing key token is valid.
Refer to the AMD PSB user guide doc# 56654, Revision# 1.00.
Unfortunately this document is only available with NDA customers.


### drivers/intel/fsp2_0: Add native implementation for FSP Debug Handler

This patch implements coreboot native debug handler to manage the FSP
event messages.

'FSP Event Handlers' feature introduced in FSP to generate event
messages to aid in the debugging of firmware issues. This eliminates
the need for FSP to directly write debug messages to the UART and FSP
might not need to know the board related UART port configuration.
Instead FSP signals the bootloader to inform it of a new debug message.
This allows the coreboot to provide board specific methods of reporting
debug messages, example: legacy UART or LPSS UART etc.

This implementation has several advantages as:
1. FSP relies on XIP 'DebugLib' driver even while printing FSP-S debug
  messages, hence, without ROM being cached, post 'romstage' would
  results into sluggish boot with FSP debug enabled.
  This patch utilities coreboot native debug implementation which is
  XIP during FSP-M and relocatable to DRAM based resource for FSP-S.

2. This patch simplifies the FSP DebugLib implementation and remove the
   need to have serial port library. Instead coreboot 'printk' can be
   used for display FSP serial messages. Additionally, unifies the debug
   library between coreboot and FSP.

3. This patch is also useful to get debug prints even with FSP
   non-serial image (refer to 'Note' below) as FSP PEIMs are now
   leveraging coreboot debug library instead FSP 'NULL' DebugLib
   reference for release build.

4. Can optimize the FSP binary size by removing the DebugLib dependency
   from most of FSP PEIMs, for example: on Alder Lake FSP-M debug binary
   size is reduced by ~100KB+ and FSP-S debug library size is also
   reduced by ~300KB+ (FSP-S debug and release binary size is exactly
   same with this code changes). The total savings is ~400KB for each
   FSP copy, and in case of Chrome AP firmware with 3 copies, the total
   savings would be 400KB * 3 = ~1.2MB.

Note: Need to modify FSP source code to remove 'MDEPKG_NDEBUG' as
compilation flag for release build and generate FSP binary with non-NULL
FSP debug wrapper module injected (to allow FSP event handler to execute
even with FSP non-serial image) in the final FSP.fd.


### security/tpm: Add vendor-specific tis functions to read/write TPM regs

In order to abstract bus-specific logic from TPM logic, the prototype
for two vendor-specific tis functions are added in this
patch. tis_vendor_read() can be used to read directly from TPM
registers, and tis_vendor_write() can be used to write directly to TPM
registers.


### arch/x86: Add support for catching null dereferences through debug regs

This commit adds support for catching null dereferences and execution
through x86's debug registers. This is particularly useful when running
32-bit coreboot as paging is not enabled to catch these through page
faults. This commit adds three new configs to support this feature:
DEBUG_HW_BREAKPOINTS, DEBUG_NULL_DEREF_BREAKPOINTS and
DEBUG_NULL_DEREF_HALT.


### drivers/i2c/generic: Add support for i2c device detection

Add 'detect' flag which can be attached to devices which may or may not
be present at runtime, and for which coreboot should probe the i2c bus
to confirm device presence prior to adding an entry for it in the SSDT.

This is useful for boards which may utilize touchpads/touchscreens from
multiple vendors, so that only the device(s) present are added to the
SSDT. This relieves the burden from the OS to detect/probe if a device
is actually present and allows the OS to trust the ACPI _STA value.


### util/cbmem: Add FlameGraph-compatible timestamps output

Flame graphs are used to visualize hierarchical data, like call stacks.
Timestamps collected by coreboot can be processed to resemble
profiler-like output, and thus can be feed to flame graph generation
tools.

Generating flame graph using https://github.com/brendangregg/FlameGraph:
```
   cbmem -S > trace.txt
   FlameGraph/flamegraph.pl --flamechart trace.txt > output.svg
```


### src/console/Kconfig: Add option to disable loglevel prefix

This patch adds an option to disable loglevel prefixes. This patch helps
to achieve clear messages when low loglevel is used and very few
messages are displayed on a terminal. This option also allows to
maintain compatibility with log readers and continuous integration
systems that depend on fixed log content.

If the code contains:
  printk(BIOS_DEBUG, "This is a debug message!\n")
it will show as:
  [DEBUG]  This is a debug message!
but if the Kconfig contains:
  CONFIG_CONSOLE_USE_LOGLEVEL_PREFIX=n
the same message will show up as
  This is a debug message!


### util/cbmem: add an option to append timestamp

Add an option to the cbmem utility that can be used to append an entry
to the cbmem timestamp table from userspace. This is useful for
bookkeeping of post-coreboot timing information while still being able
to use cbmem-based tooling for processing the generated data.


`-a | --add-timestamp ID:          append timestamp with ID\n`


Additional changes
------------------

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* As always, general documentation, code cleanup, and refactoring
* Remove doxygen config files and targets
* Get clang compile working for all x86 platforms
* Work on updating checkpatch to match the current Linux version
* Timestamps: Rename timestamps to make names more consistent
* Continue updating ACPI code to ASL 2.0
* Remove redundant or unnecessary headers from C files
* arch/x86/acpi_bert_storage.c: Use a common implementation
* Postcar stage improvements
* arch/x86/acpi: Consolidate POST code handling
* intel/common: Enable ROM caching in ramstage
* vendorcode/amd/agesa: Fix improper use of .data (const is important)
* sandybridge & gm45: Support setting PCI bars above 4G


Plans for Code Deprecation
--------------------------


### Intel Icelake

Intel Icelake is unmaintained. Also, the only user of this platform ever was
the CRB board. From the looks of it the code never was ready for production as
only engineering sample CPUIDs are supported.

Thus, to reduce the maintanence overhead for the community, it is deprecated
from this release on and support for the following components will be dropped
with the release 4.19.

  * Intel Icelake SoC
  * Intel Icelake RVP mainboard


### LEGACY_SMP_INIT

As of release 4.18 (August 2022) we plan to deprecate LEGACY_SMP_INIT.
This also includes the codepath for SMM_ASEG. This code is used to start
APs and do some feature programming on each AP, but also set up SMM.
This has largely been superseded by PARALLEL_MP, which should be able to
cover all use cases of LEGACY_SMP_INIT, with little code changes. The
reason for deprecation is that having 2 codepaths to do the virtually
the same increases maintenance burden on the community a lot, while also
being rather confusing.

No platforms in the tree have any hardware limitations that would block
migrating to PARALLEL_MP / a simple !CONFIG_SMP codebase.


Statistics
----------

- Total Commits: 1261
- Average Commits per day: 13.26
- Total lines added: 42535
- Average lines added per commit: 33.73
- Number of patches adding more than 100 lines: 51
- Average lines added per small commit: 21.00
- Total lines removed: 65961
- Average lines removed per commit: 52.31
- Total difference between added and removed: -23426
- Total authors: 146
- New authors: 17
