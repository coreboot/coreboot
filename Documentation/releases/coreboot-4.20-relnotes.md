Upcoming release - coreboot 4.20
========================================================================

The 4.20 release is planned for the 20th of April 2023.


The 4.21 release is planned for around the 17th of July, 2023


Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.
* Note that all changes before the release are done are marked upcoming.
  A final version of the notes are done after the release.

* This document may also be edited at the google doc copy:
  https://docs.google.com/document/d/1_0PeRxzT7ep8dIZobzIqG4n6Xwz3kkIDPVQURX7YTmM/edit

Significant or interesting changes
----------------------------------

### Add changes that need a full description here

* Remove Yabits payload, Yabits project is deprecated and archived.

### Toolchain updates

* Upgrade MPC from version 1.2.1 to 1.3.1
* Upgrade MPFR from version 4.1.1 to 4.2.0
* Upgrade CMake from version 3.25.0 to 3.26.2
* Upgrade LLVM from version 15.0.6 to 15.0.7
* Upgrade GCC from version 11.2.0 to 11.3.0
* Upgrade binutils from version 2.37 to 2.40

Additional coreboot changes
---------------------------

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* Changes that only need a line or two of description go here.




Plans to move platform support to a branch
------------------------------------------

### Mainboard Scaleway Tagada

According to the author of the mainboard scaleway/tagada, the mainboard
is not used anymore. Since the mainboard is not publicly available for
purchase and not used anywhere else, the usual deprecation process of 6
months is not needed.

Thus, to reduce the maintenance overhead for the community, support for
the following components will be removed from the master branch and will
be maintained on the release 4.19 branch.

  * Mainboard Scaleway Tagada


### Intel Quark SoC & Galileo mainboard

The SoC Intel Quark is unmaintained and different efforts to revive it
have so far failed.  The only user of this SoC ever was the Galileo
board.

Thus, to reduce the maintanence overhead for the community, support for
the following components will be removed from the master branch and will
be maintained on the release 4.20 branch.

  * Intel Quark SoC
  * Intel Galileo mainboard


Statistics from the 4.19 to the 4.20 release
--------------------------------------------

* To be filled in immediately before the release by the release team


Significant Known and Open Issues
---------------------------------

* To be filled in immediately before the release by the release team

