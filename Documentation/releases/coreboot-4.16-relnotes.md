Upcoming release - coreboot 4.16
================================

The 4.16 release is planned for February, 2022.

We are increasing the frequency of releases in order to enable others to release quarterly on
a fresher version of coreboot.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Significant changes
-------------------

### Add significant changes here

### Option to disable Intel Management Engine
Disable the Intel (Converged Security) Management Engine ((CS)ME) via HECI based
on Intel Core processors from Skylake to Alder Lake. State is set based on a
CMOS value of `me_state`. A value of `0` will result in a (CS)ME state of `0`
(working) and value of `1` will result in a (CS)ME state of `3` (disabled). For
an example CMOS layout and more info, see
[cse.c](../../src/soc/intel/common/block/cse/cse.c).
