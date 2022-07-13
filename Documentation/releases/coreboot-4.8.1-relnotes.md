coreboot 4.8 & 4.8.1 release notes
==================================

The 4.8.1 release contains 2 commits: 5f0b80b880 and 6794ce02d4.  This
minor release fixes an issue with adding payloads. The 4.8 release
covers commit 6dd2f69878 to commit ebdeb4d07d

Since the last release, the coreboot project had 1198 commits by 124
authors.

There are PGP signed 4.8 and 4.8.1 tags in the git repository.  A branch
for 4.8 releases (4.8_branch) has been created.

A big thank you to everyone involved in making this release happen. We
couldn't have done this without the 35 new commit authors, the
experienced developers, the many reviewers, documentation writers and
the fantastic community supporting users on both the mailing list and
the IRC channel.

In general, this has been a calm release cycle. Several old devices were
removed from the master branch early in the release, as they hinder
development and nobody stepped up doing the porting effort or was
willing to test coreboot on them. If there is the desire to get a board
back, it isn't lost as it’s still in the git history.

Intel i945 platform
-------------------
* On Intel 945 devices, native graphics initialization is now skipped
saving around 100 ms during resume from S3. The OS drivers need to be
able to handle that. Linux’ i915 driver is able to handle it, but not
the frame buffer driver.

AMD Stoney Ridge
----------------------------------
* Significant cleanup from older AGESA based platforms
* Fixes to get S3 working
* Updates to GPIO code to match other modern coreboot chips
* AGESA interface cleanup - Use native coreboot functions when
possible

Lenovo mainboards
-----------------
* Started integration of VBT (Video BIOS Table) binary files to
support native graphics initialisation

Internal changes
----------------
* Rename of payload type 'payload' to 'simple_elf'
* Progress in removing typedef device_t
* Migrated all Intel platforms to a common VBT codebase
* Ongoing cleanup of whitespace, spelling and formatting
* Support for PCI in ramstage on non-x86
* Ongoing Intel platform code deduplication

Console changes
---------------
* Reduce default loglevel to DEBUG
* Introduce a way for mainboard to override the loglevel
* Restrict console messages to after console initialization

Fixed Bugs
----------
* qemu-i440fx: Fix ACPI checksum corruption
* intelmetool: Fix crash, support ME11+ platforms, fix bootguard
detection
* tpm: Fix TPM software stack vulnerability in tlcl_read() for TPM 1.2 (https://github.com/nccgroup/TPMGenie)
* asrock/b75pro3-m: Fixed HDMI
* Intel/ibexpeak: Fix missing ACPI PIRQ entries
* Intel/nehalem: Fix freeze during chipset lockdown

Payloads
--------
* Bumped SeaBIOS to 1.11.1
* Improved edk2 integration

Security
--------
* Start of refactoring the TPM software stack
* Introduced coreboot security section in kconfig
* vboot & TPM code moved into src/security

Intelmetool
-----------
* Add Intel Boot Guard status support

Documentation
-------------
* Switch from Hugo to Sphinx for the Documentation
* Working on markdown documentation for https://doc.coreboot.org

Added 17 mainboards
-------------------
* Asus MAXIMUS_IV_GENE_Z                               Intel Sandybridge
* Google ATLAS                                         Intel Kabylake
* Google BIP                                           Intel Geminilake
* Google CHEZA                                         Qualcomm SDM845
* Google NOCTURNE                                      Intel Kabylake
* Google OCTOPUS                                       Intel Geminilake
* Google PHASER                                        Intel Geminilake
* Google YORP                                          Intel Geminilake
* HP 8770W                                             Intel Ivybridge
* HP FOLIO_9470M                                       Intel Ivybridge
* Intel KBLRVP8                                        Intel Skylake
* Lenovo W520                                          Intel Sandybridge
* OCP MONOLAKE                                         Intel Broadwell DE
* OCP WEDGE100S                                        Intel Broadwell DE
* Purism Librem 15 v2                                  Intel Broadwell
* Scaleway TAGADA                                      Intel Denverton
* SiFive HIFIVE_UNLEASHED                              SiFive FU540

Removed 39 mainboards
---------------------
* Abit BE6_II_V2_0
* AMD DINAR
* AMD RUMBA
* Asus DSBF
* Asus MEW_AM
* Asus MEW_VM
* A-trend ATC_6220
* A-trend ATC_6240
* AZZA PT_6IBD
* Biostar M6TBA
* Compaq DESKPRO_EN_SFF_P600
* DMP EX
* ECS P6IWP_FE
* Gigabyte GA_6BXC
* Gigabyte GA_6BXE
* HP E_VECTRA_P2706T
* Intel D810E2CB
* Intel EAGLEHEIGHTS
* Intel MTARVON
* Intel TRUXTON
* Iwave RAINBOW_G6
* Lanner EM8510
* Lippert FRONTRUNNER
* Mitac 6513WU
* MSI MS_6119
* MSI MS_6147
* MSI MS_6156
* MSI MS_6178
* NEC POWERMATE_2000
* Nokia IP530
* RCA RM4100
* Soyo SY_6BA_PLUS_III
* Supermicro H8QGI
* Supermicro H8SCM
* Supermicro X7DB8
* Thomson IP1000
* Tyan S1846
* Tyan S8226
* Wyse S50

Added 2 socs
------------
* Qualcomm sdm845
* SiFive fu540

Removed 2 socs
--------------
* DMP vortex86ex
* Intel sch

Removed 5 processors
--------------------
* AMD agesa-family15
* AMD geode-gx2
* Intel ep80579
* Intel model-f0x
* Intel model-f1x

Statistics
----------
* Total commits: 1198
* Average Commits per day: 9.85
* Total authors: 124
* New authors: 35
* Total lines added: 386113
* Total lines removed: 291201
* Total lines difference: 94912
