coreboot 4.18 release
========================================================================

The 4.18 release was quite late, but was completed on October 16, 2022.

In the 4 months since the 4.17 release, the coreboot project has merged
more than 1800 commits from over 200 different authors. Over 50 of those
authors submitted their first patches.

Welcome and thank you to all of our new contributors, and of course the
work of all of the seasoned contributors is greatly appreciated.


Significant or interesting changes
----------------------------------

### sconfig: Allow to specify device operations

Currently we only have runtime mechanisms to assign device operations to
a node in our devicetree (with one exception: the root device). The most
common method is to map PCI IDs to the device operations with a `struct
pci_driver`. Another accustomed way is to let a chip driver assign them.

For very common drivers, e.g. those in soc/intel/common/blocks/, the PCI
ID lists grew very large and are incredibly error-prone. Often, IDs are
missing and sometimes IDs are added almost mechanically without checking
the code for compatibility. Maintaining these lists in a central place
also reduces flexibility.

Now, for onboard devices it is actually unnecessary to assign the device
operations at runtime. We already know exactly what operations should be
assigned. And since we are using chipset devicetrees, we have a perfect
place to put that information.

This patch adds a simple mechanism to `sconfig`. It allows us to speci-
fy operations per device, e.g.

device pci 00.0 alias system_agent on
	ops system_agent_ops
end

The operations are given as a C identifier. In this example, we simply
assume that a global `struct device_operations system_agent_ops` exists.


### Set touchpads to use detect (vs probed) flag

Historically, ChromeOS devices have worked around the problem of OEMs
using several different parts for touchpads/touchscreens by using a
ChromeOS kernel-specific 'probed' flag (rejected by the upstream kernel)
to indicate that the device may or may not be present, and that the
driver should probe to confirm device presence.

Since release 4.18, coreboot  supports detection for i2c devices at
runtime when creating the device entries for the ACPI/SSDT tables,
rendering the 'probed' flag obsolete for touchpads. Switch all touchpads
in the tree from using the 'probed' flag to the 'detect' flag.

Touchscreens require more involved power sequencing, which will be done
at some future time, after which they will switch over as well.


### Add SBOM (Software Bill of Materials) Generation

Firmware is typically delivered as one large binary image that gets
flashed. Since this final image consists of binaries and data from a
vast number of different people and companies, it's hard to determine
what all the small parts included in it are. The goal of the software
bill of materials (SBOM) is to take a firmware image and make it easy to
find out what it consists of and where those pieces came from.

Basically, this answers the question, who supplied the code that's
running on my system right now? For example, buyers of a system can use
an SBOM to perform an automated vulnerability check or license analysis,
both of which can be used to evaluate risk in a product. Furthermore,
one can quickly check to see if the firmware is subject to a new
vulnerability included in one of the software parts (with the specified
version) of the firmware.

Further reference:
https://web.archive.org/web/20220310104905/https://blogs.gnome.org/hughsie/2022/03/10/firmware-software-bill-of-materials/

- Add Makefile.inc to generate and build coswid tags
- Add templates for most payloads, coreboot, intel-microcode,
  amd-microcode. intel FSP-S/M/T, EC, BIOS_ACM, SINIT_ACM,
  intel ME and compiler (gcc,clang,other)
- Add Kconfig entries to optionally supply a path to CoSWID tags
  instead of using the default CoSWID tags
- Add CBFS entry called SBOM to each build via Makefile.inc
- Add goswid utility tool to generate SBOM data


Additional coreboot changes
---------------------------

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* Allocator v4 is not yet ready, but received significant work.
* Console: create an [smbus console driver](https://doc.coreboot.org/technotes/console.html)
* pciexp_device: Numerous updates and fixes
* Update checkpatch to match Linux v5.19
* Continue updating ACPI to ASL 2.0 syntax
* arch/x86: Add a common romstage entry point
* Documentation: Add a list of [acronyms](https://doc.coreboot.org/acronyms.html)
* Start hooking up ops in devicetree
* Large amounts of general code cleanup and improvement, as always
* Work to make sure all files have licenses


Payloads
--------

### EDK II (TianoCore)

coreboot uses TianoCore interchangeably with EDK II, and whilst the
meaning is generally clear, it's not the payload it uses.
Consequentially, TianoCore has been renamed to EDK II (2).

The option to use the already deprecated CorebootPayloadPkg has been
removed.

Recent changes to both coreboot and EDK means that UefiPayloadPkg seems
to work on all hardware. It has been tested on:

* Intel Core 2nd, 3rd, 4th, 5th, 6th, 7th, 8th, 8th, 9th, 10th,
  11th and 12th generation processors
* Intel Small Core BYT, BSW, APL, GLK and GLK-R processors
* AMD Stoney Ridge and Picasso

CorebootPayloadPkg can still be found [here](https://github.com/MrChromebox/edk2/tree/coreboot_fb).

The recommended option to use is `EDK2_UEFIPAYLOAD_MRCHROMEBOX` as
`EDK2_UEFIPAYLOAD_OFFICIAL` will no longer work on any SoC.


New Mainboards
--------------

* AMD Birman
* AMD Pademelon renamed from Padmelon
* Google Evoker
* Google Frostflow
* Google Gaelin4ADL
* Google Geralt
* Google Joxer
* Google Lisbon
* Google Magikarp
* Google Morthal
* Google Pujjo
* Google Rex 0
* Google Shotzo
* Google Skolas
* Google Tentacruel
* Google Winterhold
* Google Xivu
* Google Yaviks
* Google Zoglin
* Google Zombie
* Google Zydron
* MSI PRO Z690-A WIFI DDR4
* Siemens MC APL7


Removed Mainboards
------------------

* Google Brya4ES


Updated SoCs
------------

* Added Intel Meteor Lake
* Added Mediatek Mt8188
* Renamed AMD Sabrina to Mendocino
* Added AMD Morgana


Plans for Code Deprecation
--------------------------

### LEGACY_SMP_INIT

Legacy SMP init will be removed from the coreboot master branch
immediately following this release. Anyone looking for the latest
version of the code should find it on the 4.18 branch or tag.

This also includes the codepath for SMM_ASEG. This code is used to start
APs and do some feature programming on each AP, but also set up SMM.
This has largely been superseded by PARALLEL_MP, which should be able to
cover all use cases of LEGACY_SMP_INIT, with little code changes. The
reason for deprecation is that having 2 codepaths to do the virtually
the same increases maintenance burden on the community a lot, while also
being rather confusing.


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


### Intel Icelake SoC & Icelake RVP mainboard

Intel Icelake is unmaintained. Also, the only user of this platform ever
was the Intel CRB (Customer Reference Board). From the looks of it the
code was never ready for production as only engineering sample CPUIDs
are supported. This reduces the maintanence overhead for the coreboot
project.

Intel Icelake code will be removed with release 4.19 and any maintenence
will be done on the 4.19 branch. This consists of the Intel Icelake SoC
and Intel Icelake RVP mainboard.


### Intel Quark SoC & Galileo mainboard

The SoC Intel Quark is unmaintained and different efforts to revive it
failed.  Also, the only user of this platform ever was the Galileo
board.

Thus, to reduce the maintanence overhead for the community, support for
the following components will be removed from the master branch and will
be maintained on the release 4.20 branch.

  * Intel Quark SoC
  * Intel Galileo mainboard


Statistics from commit d2d9021543 to f4c97ea131
-----------------------------------------------

- Total Commits: 1822
- Average Commits per day: 13.38
- Total lines added: 150578
- Average lines added per commit: 82.64
- Number of patches adding more than 100 lines: 128
- Average lines added per small commit: 38.44
- Total lines removed: 33849
- Average lines removed per commit: 18.58
- Total difference between added and removed: 116729
- Total authors: 202
- New authors: 52


Known Issues
------------

A couple of issues were discovered immediately following the release
that will be fixed in a follow-on point release in the upcoming weeks.

A pair of changes ([CB:67754](https://review.coreboot.org/67754) and
[CB:67662](https://review.coreboot.org/67662)) which merged shortly
before the 4.18 release have created an issue on Intel Apollo Lake
platform boards which prevents SMM/SMI from functioning; this affects
only Apollo Lake (but not Gemini Lake) devices.
See [CB:68599](https://review.coreboot.org/68599) for the fix.

Another issue applies to all Intel-based boards with onboard I2C TPMs
when verified boot is not enabled. The I2C buses don’t get initialized
until after the TPM, causing timeouts, TPM initialization failures, and
long boot times.  See [CB:68550](https://review.coreboot.org/68550) for
the fix.
