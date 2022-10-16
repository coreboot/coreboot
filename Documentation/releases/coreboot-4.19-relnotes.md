Upcoming release - coreboot 4.19
========================================================================

The 4.19 release is planned for January 2023.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.
* Note that all changes before the release are done are marked upcoming.
  A final version of the notes are done after the release.

Significant changes
-------------------

### Add significant changes here



Additional coreboot changes
---------------------------

* One or two line change comments go here



Payloads
--------

### Payload changes go here



Plans for Code Deprecation
--------------------------


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
