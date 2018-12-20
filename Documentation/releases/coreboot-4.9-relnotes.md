coreboot 4.9 release notes
==========================

The 4.9 release covers commit 532b8d5f25 to commit 7f520c8fe6
There is a pgp signed 4.9 tag in the git repository, and a branch will
be created as needed.

In the little more than 7 months since 4.8.1 we had 175 authors commit
2610 changes to master. The changes were, for the most part, all over
the place, touching every part of the repository: chipsets, mainboards,
tools, build system, documentation.

In that time we also had 70 authors made their first commit to coreboot:
Welcome and to many more!

Finally, a big Thank You to all contributors who helped shape the
coreboot project, community and code with their effort, no matter if
through development, review, testing, documentation or by helping people
asking questions on our venues like IRC or our mailing list.

Clean up
--------
If there's any topic to give to this release, "clean up" might be the
most appropriate: There was lots of effort to bring the codebase into
compliance with our coding style, to remove old idioms that we'd like
to retire like the overloaded `device_t` data type, and to let features
percolate through the entire tree to bring more uniformity to its parts.

For example, during the coreboot 4.4 cycle, coreboot gained the notion
of mainboard variants to avoid duplication of code in rather similar
mainboards.

Back then, this feature was developed and used mostly for the benefit
of Chrome OS devices, but more recently the code for various Lenovo
Thinkpads was deduplicated in the same way.

Another part of cleaning up our tree is improving our tools that help
developers follow coding style and avoid mistakes, as well as the
infrastructure we have for automated build tests and we've seen quite
some activity in that space as well.

Documentation
-------------
Since the last release we also moved the documentation into the
repository. No need for a special wiki account to edit the documentation,
and by colocating sources and documentation, it's easier to keep the
latter in sync with the code, too.

This effort is still under way, which is why we still host the old wiki (now
read-only) in parallel to the [new documentation
site](https://doc.coreboot.org) that is rendered from coreboot.git's
Documentation/ directory.

Blobs handling
--------------
Another big change is in our blobs handling: Given that Intel now
provides a reasonably licensed repository with FSP binaries, we were
able to mirror it to coreboot.org and integrate it in the build system.
This makes it easier to have working images out of the box for devices
that depend on Intel's proprietary init code.

As usual the blobs aren't part of the coreboot tree and only downloaded
with the `USE_BLOBS` options.

Deprecations
------------
One of the first changes to coreboot after the 4.8 release was to remove
boards that didn't support certain new features and were apparently
unmaintained, as discussed in the release notes of coreboot 4.6.

We didn't follow up on all plans made back then to deprecate boards more
aggressively: The board status reporting mechanism is still rather raw
and therefore places quite a burden on otherwise sympathetic contributors
of build results.

Also, there will be no deprecations after 4.10: Due to its slipping
schedule, coreboot 4.9 is released rather late, and as a result 4.10
will only see about 4 months of development. We considered that a rather
short timeframe in which to bring old boards up to new standards, and
so the next deprecation cycle may be announced with 4.10 to occur after
4.11 is released, in late 2019.

General changes
---------------
* Various code cleanups
  * Removed `device_t` in favor of `struct device*` in ramstage code
  * Removed unnecessary include directives
  * Improved adherence to coding style
  * Deduplicated boards by using the variants mechanism
* Expand use of the postcar stage
* Add bootblock compression capability: on systems that copy the bootblock
  from very slow flash to SRAM, allow adding a stub that decompresses the
  bootblock into SRAM to minimize the amount of flash reads
* Rename the POWER8 architecture port to PPC64 to reflect that it isn't limited
  to POWER8
* Added support for booting FIT (uImage) payloads on arm64
* Added SPI flash write protection API
  * Implemented on Winbond
* Implemented TCPA log for measured boot
* Implemented GDB support for arm64 architecture in libpayload
* Dropped support for unmaintained code paths
* Measured boot support

Added 56 mainboards
-------------------
* ASROCK G41C-GS
* ASROCK G41M-GS
* ASROCK G41M-S3
* ASROCK G41M-VS3 R2.0
* ASROCK H81M-HDS
* ASUS P5QC
* ASUS P5QL-PRO
* ASUS P5Q-PRO
* ASUS P8H61-M-LX
* ASUS P8H61-M-PRO
* CAVIUM CN8100-SFF-EVB
* FACEBOOK WATSON
* FOXCONN D41S
* GIGABYTE GA-H61M-S2PV
* GOOGLE ALEENA
* GOOGLE AMPTON
* GOOGLE ARCADA
* GOOGLE ASUKA
* GOOGLE BOBBA
* GOOGLE BUDDY
* GOOGLE CAREENA
* GOOGLE CAROLINE
* GOOGLE CASTA
* GOOGLE CAVE
* GOOGLE DELAN
* GOOGLE DRAGONEGG
* GOOGLE FLEEX
* GOOGLE HATCH
* GOOGLE KARMA
* GOOGLE KUKUI
* GOOGLE LIARA
* GOOGLE MEEP
* GOOGLE RAMMUS
* GOOGLE SARIEN
* GOOGLE SENTRY
* HEWLETT PACKARD HP COMPAQ 8200 ELITE SFF PC
* INTEL COFFEELAKE RVP11
* INTEL COFFEELAKE RVP8
* INTEL COFFEELAKE RVPU
* INTEL DG41WV
* INTEL ICELAKE RVPU
* INTEL ICELAKE RVPY
* INTEL WHISKEYLAKE RVP
* LENOVO T431S
* LENOVO THINKCENTRE A58
* LENOVO W500
* LENOVO W530
* OPENCELLULAR ELGON
* OPENCELLULAR ROTUNDU
* OPENCELLULAR SUPABRCKV1
* SIEMENS MC-APL2
* SIEMENS MC-APL3
* SIEMENS MC-APL4
* SIEMENS MC-APL5

Dropped 71 mainboards
---------------------
* AAEON PFM-540I REVB
* AMD DB800
* AMD DBM690T
* AMD F2950
* AMD MAHOGANY
* AMD NORWICH
* AMD PISTACHIO
* AMD SERENGETI-CHEETAH
* ARTECGROUP DBE61
* ASROCK 939A785GMH
* ASUS A8N-E
* ASUS A8N-SLI
* ASUS A8V-E DELUXE
* ASUS A8V-E SE
* ASUS K8V-X
* ASUS KFSN4-DRE K8
* ASUS M2N-E
* ASUS M2V
* ASUS M2V MX-SE
* BACHMANN OT200
* BCOM WINNETP680
* BROADCOM BLAST
* DIGITALLOGIC MSM800SEV
* GIGABYTE GA-2761GXDK
* GIGABYTE M57SLI
* GOOGLE KAHLEE
* GOOGLE MEOWTH
* GOOGLE PURIN
* GOOGLE ROTOR
* GOOGLE ZOOMBINI
* HP DL145-G1
* HP DL145-G3
* IEI PCISA LX-800 R10
* IEI PM LX2-800 R10
* IEI PM LX-800 R11
* INTEL COUGAR-CANYON2
* INTEL STARGO2
* IWILL DK8 HTX
* JETWAY J7F2
* JETWAY J7F4K1G2E
* JETWAY J7F4K1G5D
* KONTRON KT690
* LINUTOP LINUTOP1
* LIPPERT HURRICANE LX
* LIPPERT LITERUNNER LX
* LIPPERT ROADRUNNER LX
* LIPPERT SPACERUNNER LX
* LOWRISC NEXYS4DDR
* MSI MS7135
* MSI MS7260
* MSI MS9185
* MSI MS9282
* NVIDIA L1-2PVV
* SIEMENS SITEMP-G1P1
* SUNW ULTRA40
* SUNW ULTRA40M2
* SUPERMICRO H8DME
* SUPERMICRO H8DMR
* TECHNEXION TIM5690
* TECHNEXION TIM8690
* TRAVERSE GEOS
* TYAN S2912
* VIA EPIA-CN
* VIA EPIA-M700
* VIA PC2500E
* VIA VT8454C
* WINENT MB6047
* WINENT PL6064
* WINNET G170

CPU changes
-----------
* cpu/intel/model\_2065x,206ax,haswell: Switch to `POSTCAR_STAGE`
* cpu/intel/slot\_1: Switch to different CAR setup
* Dropped support for the FSP1.0 sandy-/ivy-bridge bootpath

SoC changes
-----------
* Added Cavium CN81xx, Intel Ice Lake and Mediatek MT8183
* Dropped Broadcom Cygnus, Lowrisc and Marvell mvmap2315

Northbridge changes
-------------------
* Dropped AMD K8, VIA CN700, VIA CX700, VIA VX800 because they lack `EARLY_CBMEM` support
* intel/e7505: Moved to `EARLY_CBMEM`
* nb/intel/i945,e7505,pineview,x4x,gm45,i440bx: Moved to `POSTCAR_STAGE`
* nb/intel/i440bx, e7505: Moved to `RELOCATABLE_RAMSTAGE`
* intel/x4x: Add DDR3 support
* nb/intel/pineview: Speed up fetching SPD
* nb/intel/i945,gm45,x4x,pineview: Use TSEG in SMI

Southbridge changes
-------------------
* sb/intel/i82801{g,i,j}x, lynxpoint: Use the common ACPI pirq generator
* sb/intel/i82801{g,i,j}x: Use common code to set up SMM and for the smihandler
* Use common functions for PMBASE configuration

Payload changes
---------------
* Support initrd in uImage/FIT to be placed above 4GiB
* Added documentation for uImage/FIT payloads

Toolchain
---------
* Update to gcc 8.1.0, binutils 2.30, IASL 20180810, clang 6
