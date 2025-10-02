coreboot 24.12 release
=================================

We are pleased to announce the release of coreboot 24.12, another significant
milestone in our ongoing commitment to delivering open-source firmware
solutions. This release includes 970 commits, contributed by 126 dedicated
individuals from our global community. The updates in 24.12 bring
various enhancements, optimizations, and new features that further improve the
reliability and performance of coreboot across supported platforms.

We extend our sincere thanks to the patch authors, reviewers, and everyone
involved in the coreboot community for their hard work and dedication. Your
contributions continue to advance and refine coreboot with each release. As
always, thank you for your support and collaboration in driving the future of
open-source firmware.

The next coreboot release, 25.03, is planned for mid March.

Significant or interesting changes
----------------------------------

### drivers/option: Add forms in cbtables

Introduce a mechanism so that coreboot can provide a list of options to
post-coreboot code. The options are grouped together into forms and
have a meaning name and optional help text. This can be used to let
payloads know which options should be displayed in a setup menu,
for instance. Although this system was written to be used with edk2,
it has been designed with flexibility in mind so that other payloads
can also make use of this mechanism. The system currently lacks a way
to describe where to find option values.

This information is stored in a set of data structures specifically
created for this purpose. This format is known as CFR, which means
"coreboot forms representation" or "cursed forms representation".
Although the "forms representation" is borrowed from UEFI, CFR can
be used in non-UEFI scenarios as well.

The data structures are implemented as an extension of cbtables records
to support nesting. It should not break backwards compatibility because
the CFR root record (LB_TAG_CFR_ROOT) size includes all of its children
records. The concept of record nesting is borrowed from the records for
CMOS options. It is not possible to reuse the CMOS records because they
are too closely coupled with CMOS options; using these structures would
needlessly restrict more capable backends to what can be done with CMOS
options, which is undesired.

Because CFR supports variable-length components, directly transforming
options into CFR structures is not a trivial process. Furthermore, CFR
structures need to be written in one go. Because of this, abstractions
exist to generate CFR structures from a set of "setup menu" structures
that are coreboot-specific and could be integrated with the devicetree
at some point. Note that `struct sm_object` is a tagged union. This is
used to have lists of options in an array, as building linked lists of
options at runtime is extremely impractical because options would have
to be added at the end of the linked list to maintain option order. To
avoid mistakes defining `struct sm_object` values, helper macros exist
for supported option types. The macros also provide some type checking
as they initialise specific union members.

It should be possible to extend CFR support for more sophisticated
options like fan curve points.


### Makefile.mk: Skip unnecessary recompiles when static.{c,h} are updated

The generated static.c file output by sconfig is currently added as a
prerequisite for all objects to ensure that static.h exists before
compiling anything that might need it. However, this forces every single
object out of date when the compiled devicetree is updated, even though
not every file actually needs static.h.

Only static.h actually needs to exist before compilation of other
objects, since static.c is an independent compilation unit that doesn't
need to exist before other objects can be built. Thus, change the
prerequisite from static.c to static.h, and add a rule for static.h that
depends on static.c. The recipe is a simple `true` since sconfig
generates static.c and static.h at the same time. To prevent unnecessary
recompiles, make static.h an order-only prerequisite [1] using the new
generated files argument for create_cc_template to ensure that the
header exists before any object might need it, but without forcing a
recompile of all objects by default whenever it is updated.

On a clean build, all objects will be compiled since they do not exist,
and these will occur after static.h is generated due to the default
order-only prerequisite. On subsequent incremental compiles, sources
that do need static.h will be appropriately marked out of date due to
the generated .d dependency files from the compiler, which list static.h
as a normal prerequisite for each objects that do include it, which
overrides the default order-only prerequisite. The dependency files
generated for all other objects will not include static.h, and thus the
objects will not be updated since the default order-only dependency does
not force them out of date.

After updating the devicetree of qemu-i440fx after a clean build,
comparing the build log with `make --debug=why` with the generated
dependency files indicates that only objects that actually depend on
static.h were rebuilt, instead of every object. Running a timeless
incremental build after making a change in the devicetree yielded
identical roms when performed with this patch and main, with the only
difference being the number of objects that needed to be rebuilt. Also
tested with the E6430.

[1] <https://www.gnu.org/software/make/manual/html_node/Prerequisite-Types.html>


### sconfig: Move config_of_soc from device.h to static.h

Many sources include device.h and thus static.h, but many only need the
function declarations and type definitions, not the compiled devicetree
from sconfig. This causes many unnecessary recompiles whenever the
devicetree is updated due to the dependency. Address this by moving the
config_of_soc macro directly into the generated static.h header, as it
seems to be the only line in device.h that actually requires static.h.
For now, static.h remains included in device.h so that the build is not
affected. Subsequent commits will include static.h directly into sources
that actually need it, after which it can be dropped from device.h.

Some statistics for C objects:

Dell Latitude E6400 (GM45/ICH9):
669 total objects
181 depend on static.h
2 require static.h

Dell Latitude E6430 (Ivy Bridge/Panther Point):
693 total objects
199 depend on static.h
3 require static.h

Lenovo ThinkCentre M700 / M900 Tiny (Kaby Lake):
794 total objects
298 depend on static.h
23 objects require static.h

MSI PRO Z690-A (WIFI) DDR4 (Alder Lake):
959 total objects
319 depend on static.h
25 require static.h

The number of objects was determined by grepping the build log for
calls to CC, the number of objects that depend on static.h was
determined by grepping for calls to CC after touching static.h, and the
number of objects that actually require the static.h related lines from
device.h was determined by grepping for objects that failed to build
after removing the static.h lines from device.h and running make with
the --keep-going flag.


### Makefile: Fix no-op incremental build

If make is ran a second time after an initial clean compile, the entire
rom will be rebuilt. Subsequent calls to make will not rebuild the rom.
This initial rebuild was due to build/util/kconfig/conf being newer than
config.h, and the subsequent rebuild of the header marked everything
else as out of date. The reason conf was newer than config.h is because
it was being treated as an intermediate file [1]. In the rule for
config.h, conf is a prerequisite, but since it is treated as an
intermediate, its rule will not be run if config.h exists and all the
prerequisites of conf (i.e. its source files) are also up to date.

On a clean build after a make menuconfig, config.h exists, satisfying
these conditions. In this case, config.h is treated as being up to date
even though conf does not exist. However, if another target does not
exist and also has conf as a prerequisite, conf will then be built so
that the target can be built. This caused conf to be newer than
config.h, but by default GNU Make deletes intermediate files after a
build which would prevent conf from affecting config.h on subsequent
rebuilds.

However, commit dd6efce934fb ("Makefile: Add .SECONDARY") adds the
.SECONDARY special target, which prevents intermediate files from being
deleted after the build [2]. Thus, conf persists to the first no-op
build, making config.h out of date. Since config.h is updated during
this first rebuild, conf is no longer newer than it, and thus subsequent
no-op builds behave as expected.

Fix this by preventing conf from being treated as an intermediate file
by adding it as a prerequisite of the .NOTINTERMEDIATE special target,
which causes conf to always be rebuilt if it does not exist. Thus, on
the initial clean compile, config.h will be updated after building conf
as a prerequisite, preventing config.h from being marked out of date on
a subsequent rebuild.

[1] <https://www.gnu.org/software/make/manual/html_node/Chained-Rules.html>
[2] <https://www.gnu.org/software/make/manual/html_node/Special-Targets.html>


### drivers/efi/uefi_capsules.c: coalesce and store UEFI capsules

How it approximately works:

(During a normal system run):
1. OS puts a capsule into RAM and calls UpdateCapsule() function of EFI
runtime
2. If applying the update requires a reboot, EFI implementation creates
a new CapsuleUpdateData* EFI variable pointing at the beginning of
capsules description (not data, but description of the data) and does
a warm reboot leaving capsule data and its description in RAM to be
picked by firmware on the next boot process

(After DEV_INIT:)
3. Capsules are discovered by checking for CapsuleUpdateData* variables
4. Capsule description in memory and capsule data is validated for
Sanity
5. Capsule data is coalesced into a continuous piece of memory

(On BS_WRITE_TABLES via dasharo_add_capsules_to_bootmem() hook:)
6. Buffer with coalesced capsules is marked as reserved

(On BS_WRITE_TABLES via lb_uefi_capsules() hook:)

7. coreboot table entry is added for each of the discovered capsules

(In UEFI payload:)
8. CapsuleUpdateData* get removed
9. coreboot table is checked for any update capsules which are then applied



Additional coreboot changes
---------------------------

* Add Serial Flash Discoverable Parameters (SFDP) Support
* Add Replay Protected Monotonic Counter (RPMC) Support
* arch/arm64: Add Clang as supported compiler
* Add initial experimental Link-time Optimization (LTO) support
* util/scripts: Add script to capture commands from build
* Transform more devicetree options into booleans
* Make more mainboards using the AZALIA_PIN_DESC macro
* More effort for reworking the audio jacks setup
* libgfxinit: Add option to configure screen rotation
* Additional functions supposed in ACPI interface for Intel WiFi and BT devices



Changes to external resources
-----------------------------

### Toolchain updates

* binutils from 2.42 to 2.43.1
* GCC from 14.1.0 to 14.2.0
* CMake from 3.29.3 to 3.30.2
* LLVM from 18.1.6 to 18.1.8


### Git submodule pointers

* 3rdparty/arm-trusted-firmware: Update from commit id c5b8de86c8 to 15e5c6c91d (693 commits)
* 3rdparty/blobs: Update from commit id a8db7dfe82 to 14f8fcc1b4 (15 commits)
* 3rdparty/fsp: Update from commit id 800c85770b to 851f7105d8 (24 commits)
* 3rdparty/intel-microcode: Update from commit id 5278dfcf98 to 8ac9378a84 (4 commits)
* 3rdparty/open-power-signing-utils: add SecureBoot utility for OpenPOWER


### External payloads

* U-Boot: Upgrade from 2024.04 to 2024.07
* payloads/edk2: Update default branch for MrChromebox repo to 2024-08



Platform Updates
----------------

### Added 39 mainboards:

* Arm Neoverse N2
* ASROCK IMB-1222
* Dell Inc. Latitude E5420
* Dell Inc. Latitude E5520
* Dell Inc. Latitude E5530
* Dell Inc. Latitude E6220
* Dell Inc. Latitude E6230
* Dell Inc. Latitude E6320
* Dell Inc. Latitude E6330
* Dell Inc. Latitude E6420
* Dell Inc. Latitude E6520
* Dell Inc. Latitude E6530
* Dell Inc. OptiPlex 3050 Micro
* Erying Polestar G613 Pro (TGL-H)
* GIGABYTE GA-H77M-D3H
* Google -> Fatcat_ish
* Google -> Fatcatite
* Google -> Fatcatnuvo
* Google ->  Felino
* Google -> Francka
* Google ->  Hylia
* Google ->  Kanix
* Google ->  Navi
* Google ->  Pujjogatwin
* Google ->  Rull
* Google ->  Skywalker
* Google ->  Telith
* Google ->  Uldrenite
* Hardkernel ODROID-H4 / H4+ / H4 Ultra
* HP Compaq 8200 Elite USDT
* HP Compaq 8300 Elite SFF
* Intel Frost Creek
* LattePanda LattePanda Mu
* Lenovo ThinkCentre M920 Tiny
* MiTAC Computing Whitestone 2
* Star Labs Star Labs Byte Mk II (N200)
* Star Labs Star Labs StarFighter Mk I (i3-1315U, i7-13700H and i9-13900H)
* Topton X2F_N100
* VIA EPIA-EX (work in progress)


### Updated CPUs/SoCs

* Via C7
* Intel Pantherlake
* Intel Snowridge
* Intel Xeon SP
* MediaTek MT8196



Statistics from the 24.08 to the 24.12 release
----------------------------------------------

* Total commits: 970
* Total authors: 126
* New authors: 20



coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
