Upcoming release - coreboot 4.18
================================

The 4.18 release is planned for August 2022.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Significant changes
-------------------

### Add significant changes here
### edk2 (Tianocore)
coreboot uses TianoCore interchangeably with EDK II, and whilst the
meaning is generally clear, it's not the payload it uses. Consequentially,
Tianocore has been renamed to EDK II (2).

The option to use the already depreciated CorebootPayloadPkg has been
removed.

Recent changes to both coreboot and edk2 means that UefiPayloadPkg
seems to work on all hardware. It has been tested on:
* Intel Core 2nd, 3rd, 4th, 5th, 6th, 7th, 8th, 8th, 9th, 10th,
  11th and 12th generation processors
* Intel Small Core BYT, BSW, APL, GLK and GLK-R processors
* AMD Stoney Ridge and Picasso

CorebootPayloadPkg can still be found [here](https://github.com/MrChromebox/edk2/tree/coreboot_fb).

The recommended option to use is `EDK2_UEFIPAYLOAD_MRCHROMEBOX` as 
`EDK2_UEFIPAYLOAD_OFFICIAL` will no longer work on any SOC.

Plans for Code Deprecation
--------------------------


### Intel Icelake

Intel Icelake code will be removed following the 4.19 release, planned
for November 2022. This consists of the Intel Icelake SOC and Intel
Icelake RVP mainboard

Intel Icelake is unmaintained. Also, the only user of this platform ever
was the CRB board. From the looks of it the code never was ready for
production as only engineering sample CPUIDs are supported. This reduces
the maintanence overhead for the coreboot project.


### Intel Quark

The SoC Intel Quark is unmaintained and different efforts to revive it failed.
Also, the only user of this platform ever was the Galileo board.

Thus, to reduce the maintanence overhead for the community, it is deprecated
from this release on and support for the following components will be dropped
with the release 4.20.

  * Intel Quark SoC
  * Intel Galileo mainboard


### LEGACY_SMP_INIT

Legacy SMP init will be removed from the coreboot master branch
immediately following this release. Anyone looking for the latest
version of the code should find it on the 4.18 branch.

This also includes the codepath for SMM_ASEG. This code is used to start
APs and do some feature programming on each AP, but also set up SMM.
This has largely been superseded by PARALLEL_MP, which should be able to
cover all use cases of LEGACY_SMP_INIT, with little code changes. The
reason for deprecation is that having 2 codepaths to do the virtually
the same increases maintenance burden on the community a lot, while also
being rather confusing.
