Upcoming release - coreboot 4.19
========================================================================

The 4.19 release is planned for the 15th of January 2023.

Since the last release, the coreboot project has merged almost 1500
commits from over 150 authors.  Of those authors, more than 20 were
first-time committers to the coreboot project.

As always, we are very grateful to all of the contributors for helping
to keep the project going.  The coreboot project is different from many
open source projects in that we need to keep constantly updating the
codebase to stay relevant with the latest processors and technologies.
It takes constant effort to just stay afloat, let alone improve the
codebase.  Thank you very much to everyone who has contributed, both
in this release and in previous times.


Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.
* Note that all changes before the release are done are marked upcoming.
  A final version of the notes are done after the release.

Significant or interesting changes
----------------------------------

### Show all Kconfig options in saved config file, compress it

The coreboot build system automatically adds a 'config' file to CBFS
that lists the exact Kconfig configuration that the image was built
with. This is useful to reproduce a build after the fact or to check
whether support for a specific feature is enabled in the image.

This file has been generated using the 'savedefconfig' Kconfig command,
which generates the minimal .config file that is needed to produce the
required config in a coreboot build. This is fine for reproduction, but
bad when you want to check if a certain config was enabled, since many
options get enabled by default or pulled in through another option's
'select' statement and thus don't show up in the defconfig.

Instead coreboot now includes a larger .config instead. In order to save
some space, all of the comments disabling options are removed from the
file, except for those included in the defconfig.

We can also LZMA compress the file since it is never read by firmware
itself and only intended for later re-extraction via cbfstool, which
always has LZMA support included.


### Toolchain updates

* Upgrade LLVM from 15.0.0 to 15.0.6
* Upgrade CMake from 3.24.2 to 3.25.0
* Upgrade IASL from 20220331 to 20221020
* Upgrade MPFR from 4.1.0 to 4.1.1



Additional coreboot changes
---------------------------

* Significant work to enable and build-test clang builds



New Mainboards
--------------

* AMD: Mayan for Morgana SoC
* GIGABYTE: GA-H61M-DS2
* Google: Crystaldrift
* Google: Gladios
* Google: Marasov
* Google: Voltorb
* Intel: Meteorlake-P RVP
* Siemens: MC_EHL3
* Star Labs: StarBook Mk VI (i3-1220P and i7-1260P)
* System76: darp8
* System76: galp6

Removed Mainboards
-------------------

* AMD: Inagua
* AMD: Olive Hill
* AMD: Parmer
* AMD: Persimmon
* AMD: Southstation
* AMD: Thatcher
* AMD: Unionstation
* ASROCK: E350M1
* ASROCK: IMB-A180
* ASUS: A88XM-E
* ASUS: AM1I-A
* ASUS: F2A85-M
* ASUS: F2A85-M LE
* ASUS: F2A85-M PRO
* BAP: ODE_e20xx
* Biostar: A68N-5200
* Biostar: AM1ML
* ELMEX: pcm205400
* ELMEX: pcm205401
* GizmoSphere: Gizmo
* GizmoSphere: Gizmo2
* HP: ABM
* HP: Pavilion m6 1035dx
* Jetway: NF81_T56N_LF
* Lenovo: AMD G505s
* LiPPERT: FrontRunner-AF aka ADLINK CoreModule2-GF
* LiPPERT: Toucan-AF aka cExpress-GFR (+W83627DHG SIO)
* MSI: MS-7721 (FM2-A75MA-E35)
* PC Engines: APU1


New SoCs
-------------------

* soc/amd/glinda


Removed processors
-------------------

* cpu/amd/agesa/family14
* cpu/amd/agesa/family15tn
* cpu/amd/agesa/family16kb




Payloads
--------

### Payload changes go here



Plans to move platform support to a branch:
-------------------------------------------

### Intel Icelake SoC & Icelake RVP mainboard

Intel Icelake is unmaintained and the only user of this platform ever
was the Intel CRB (Customer Reference Board). From the looks of the
code, it was never ready for production as only engineering sample
CPUIDs are supported.

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


Statistics from the 4.18 to the 4.19 release
--------------------------------------------

- Total Commits: 1376
- Average Commits per day: 18.24
- Total lines added: 83954
- Average lines added per commit: 61.01
- Number of patches adding more than 100 lines: 69
- Average lines added per small commit: 38.96
- Total lines removed: 765520
- Average lines removed per commit: 556.34
- Total difference between added and removed: -681566


Known Issues
------------
