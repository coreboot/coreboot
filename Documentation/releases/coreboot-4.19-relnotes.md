coreboot 4.19 release
========================================================================

The 4.19 release was completed on the 16th of January 2023.

Since the last release, the coreboot project has merged over 1600
commits from over 150 authors. Of those authors, around 25 were
first-time committers to the coreboot project.

As always, we are very grateful to all of the contributors for helping
to keep the project going. The coreboot project is different from many
open source projects in that we need to keep constantly updating the
codebase to stay relevant with the latest processors and technologies.
It takes constant effort to just stay afloat, let alone improve the
codebase. Thank you very much to everyone who has contributed, both in
this release and in previous times.

The 4.20 release is planned for the 20th of April, 2023.


Significant or interesting changes
----------------------------------


### Show all Kconfig options in saved config file; compress same

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


### Finished the conversion to ASL 2.0 syntax

Until recently, coreboot still contained lots of code using the legacy
ASL syntax. However, all ASL code was ported over to make use of the ASL
2.0 syntax and from this point on new ASL code should make use of it.


Additional coreboot changes
---------------------------

* Significant work was done to enable and build-test clang builds.
* Added touchscreen power sequencing and runtime detection.
* A number of patches were added to clean up and improve SMBIOS.
* Work is in progress to unify and extend coreboot post codes.
* Clean up for header includes is in progress with help from IWYU.
* IOAPIC code has been reworked.
* Support was added to superiotool for the NCT6687D-W chip.
* Work is progressing to switch return values to enum cb_err instead of
  bool or other pass/fail indicators.
* Clang builds are now working for most boards and are being
  build-tested.
* 64-bit coreboot support is in progress and is working on a number of
  platforms.
* A driver for EC used on various Clevo laptops was added.
* Native Intel Lynxpoint code was added to replace the MRC.bin.
* Work continued for the process of adding ops structures to the
  devicetree.
* The crossgcc tool can now download the source packages, which are
  needed to build the coreboot toolchain, from coreboot’s own mirror if
  desired.
* A document with useful external resources related to firmware
  development was added at Documentation/external_docs.md.


New Mainboards
--------------

* AMD: Mayan for Phoenix SoC
* GIGABYTE: GA-H61M-DS2
* Google: Crystaldrift
* Google: Gladios
* Google: Dibbi
* Google: Gaelin
* Google: Marasov
* Google: Markarth
* Google: Omnigul
* Google: Voltorb
* Intel: Meteorlake-P RVP
* MSI: PRO Z690-A (WIFI)
* Siemens: MC_EHL3
* Star Labs: StarBook Mk VI (i3-1220P and i7-1260P)
* System76: darp8
* System76: galp6


Removed Mainboards
------------------

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
* Google: Morthal
* HP: ABM
* HP: Pavilion m6 1035dx
* Jetway: NF81_T56N_LF
* Lenovo: AMD G505s
* LiPPERT: FrontRunner-AF aka ADLINK CoreModule2-GF
* LiPPERT: Toucan-AF aka cExpress-GFR (+W83627DHG SIO)
* MSI: MS-7721 (FM2-A75MA-E35)
* PC Engines: APU1


Updated SoCs
------------

* Added soc/amd/glinda
* Renamed soc/amd/morgana to soc/amd/phoenix
* Removed cpu/amd/agesa/family14
* Removed cpu/amd/agesa/family15tn
* Removed cpu/amd/agesa/family16kb


Updated Chipsets
----------------

* Removed northbridge/amd/agesa/family14
* Removed northbridge/amd/agesa/family15tn
* Removed northbridge/amd/agesa/family16kb
* Removed southbridge/amd/agesa/hudson
* Removed southbridge/amd/cimx/sb800


Payloads
--------

* Updated GRUB from 2.04 to 2.06
* Updated SeaBIOS 1.16.0 to 1.16.1


Plans to move platform support to a branch
------------------------------------------


### Intel Icelake SoC & Icelake RVP mainboard

Intel Icelake is unmaintained and the only user of this platform ever
was the Intel CRB (Customer Reference Board). From the looks of the
code, it was never ready for production as only engineering sample
CPUIDs are supported.

Intel Icelake code will be removed following 4.19 and any maintenance
will be done on the 4.19 branch. This consists of the Intel Icelake SoC
and Intel Icelake RVP mainboard.


### Intel Quark SoC & Galileo mainboard

The SoC Intel Quark is unmaintained and different efforts to revive it
failed. Also, the only user of this platform ever was the Galileo
board.

Thus, to reduce the maintenance overhead for the community, support for
the following components will be removed from the master branch and will
be maintained on the release 4.20 branch.

  * Intel Quark SoC
  * Intel Galileo mainboard


Statistics from the 4.18 to the 4.19 release
--------------------------------------------

- Total Commits: 1608
- Average Commits per day: 17.39
- Total lines added: 93786
- Average lines added per commit: 58.32
- Number of patches adding more than 100 lines: 80
- Average lines added per small commit: 38.54
- Total lines removed: 768014
- Total difference between added and removed: -674228


Significant Known and Open Issues
---------------------------------

Issues from the coreboot bugtracker: https://ticket.coreboot.org/

| #   | Subject                                                         |
|-----|-----------------------------------------------------------------|
| 449 | ThinkPad T440p fail to start, continuous beeping & LED blinking |
| 448 | Thinkpad T440P ACPI Battery Value Issues                        |
| 446 | Optiplex 9010 No Post                                           |
| 445 | Thinkpad X200 wifi issue                                        |
| 439 | Lenovo X201 Turbo Boost not working (stuck on 2,4GHz)           |
| 427 | x200: Two battery charging issues                               |
| 414 | X9SAE-V: No USB keyboard init on SeaBIOS using Radeon RX 6800XT |
| 412 | x230 reboots on suspend                                         |
| 393 | T500 restarts rather than waking up from suspend                |
| 350 | I225 PCIe device not detected on Harcuvar                       |
| 327 | OperationRegion (OPRG, SystemMemory, ASLS, 0x2000) causes BSOD  |
