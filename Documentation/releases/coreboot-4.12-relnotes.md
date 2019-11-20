Upcoming release - coreboot 4.12
================================

The 4.12 release is planned for April 2020

Update this document with changes that should be in the release
notes.
* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Deprecations
------------

For the 4.12 release a few features on x86 became mandatory. These are
relocatable ramstage, postcar stage and C_ENVIRONMENT_BOOTBLOCK.

### Relocatable ramstage

Relocatable stages are a feature implemented only on x86, where stages
can be relocated at runtime. This is used to place ramstage in a better
location that does not collide with memory the OS or the payload tends
to use. The rationale behind making this mandatory is that you always
want cbmem to be cached so it's a good location to run ramstage from.
It avoids using lower memory altogether so the OS can make use of it
and no backing up needs to happen on S3 resume.

### Postcar stage

With Postcar stage tearing down Cache-as-Ram is done in a separate
stage. This means that romstage has a clean program boundary and
that all variables in romstage can be accessed via their linked
addresses without runtime resolution. There is no need to link
global and static variables via the CAR\_GLOBAL macro and no need
to access them with car\_set/get\_var/ptr functions.

### C\_ENVIRONMENT\_BOOTBLOCK

Historically the bootblock on x86 platforms has been compiled with
romcc. This means that the generated code only uses CPU registers
and therefore no stack. This 20K+ LOC compiler is limited and hard
to maintain and so is the code that one has to write in that
environment. A different solution is to set up Cache-as-Ram in the
bootblock and run GCC compiled code in the bootblock. The advantages
are increased flexibility and consistency with other architectures as
well as other stages: e.g. printing to console is possible and
VBOOT can run before romstage, making romstage updatable via RW FMAP
regions.

### Platforms dropped from master

The following platforms did not implement those feature are dropped
from master to allow the master branch to move on:
- AMDFAM10
- all FSP1.0 platforms: BROADWELL_DE, FSP_BAYTRAIL, RANGELEY
- VIA VX900
- TODO (AMD?)

In particular on FSP1.0 it is impossible to implement POSTCAR stage.
The reason is that FSP1.0 relocates the CAR region to the HOB before
returning to coreboot. This means that after FSP returns to coreboot
accessing variables via their original address is not possible. One
way of obtaining that behavior would be to set up Cache-as-Ram again
(but with open source code) and copy the relocated data from the HOB
there. This solution is deemed too hacky. Maybe a lesson can be
learned from this: blobs should not interfere with the execution
environment, as this makes proper integration much harder.

### 4.11_branch

Given that some platforms supported by FSP1.0 are being produced and
popular, the 4.11 release was made into a branch in which further
development can happen.

Significant changes
-------------------

### SMMSTORE is now production ready

See [smmstore](../drivers/smmstore.md) for the documentation on the API.

### Add significant changes here
