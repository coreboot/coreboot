coreboot 4.12
=============

coreboot 4.12 was released on May 12th, 2020.

Since 4.11 there were 2692 new commits by over 190 developers and of
these, 59 contributed for the first time, which is quite an amazing
increase.

Thank you to all developers who again helped made coreboot better
than ever, and a big welcome to our new contributors!

Maintainers
-----------

This release saw some activity on the MAINTAINERS file, showing more
persons, teams and companies declare publicly that they intend to
take care of mainboards and subsystems.

To all new maintainers, thanks a lot!

Documentation
-------------

Our documentation efforts in the code tree are picking up steam, with
some 70 commits in that general area. Everything from typo fixes to
documenting mainboard support or coreboot APIs.

There's still room to improve, but the contributions are getting more
and better.

Hardware support
----------------

The removals due to the announced deprecations as well as the
deduplication of boards into variants skew the stats a bit, so at
a top level view this is a rare coreboot release in that it removes
more boards (51) than it adds (49).

After accounting for the variant moves the numbers in favor of more
hardware supported than the previous version. Besides a whole lot
of Chrome OS devices (again), this release features a whole bunch
of retrofits for devices originally shipping with non-coreboot OEM
firmware, but also support for devices that come with coreboot right
out of the box.

For that, a shout out to System76, Protectli, Libretrend and the
Open Compute Project!

Cleanup
--------

We simplified the header that comes at the top of every file:
Instead of a lengthy reference to the license any given file
is under, or even the license text itself, we opted for simple
[SPDX](https://www.spdx.org) identifiers.

Since people also handled copyright lines differently, we now opt for
collecting authors in AUTHORS and let git history tell the whole story.

While at it, the content-free "This file is part of this-and-that
project" header was also dropped.

Besides that, there has also been more work to sort out the headers
we include across the tree to minimize the code impacting every
compilation unit.

Now that our board-variant mechanism matured, many boards that were
individual models so far were converted into variants, making it
easier to maintain families of devices.

Deprecations
------------

For the 4.12 release a few features on x86 became mandatory. These are
relocatable ramstage, postcar stage and C\_ENVIRONMENT\_BOOTBLOCK.

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
- all FSP1.0 platforms: BROADWELL\_DE, FSP\_BAYTRAIL, RANGELEY
- VIA VX900

In particular on FSP1.0 it is impossible to implement POSTCAR stage.
The reason is that FSP1.0 relocates the CAR region to the HOB before
returning to coreboot. This means that after FSP returns to coreboot
accessing variables via their original address is not possible. One
way of obtaining that behavior would be to set up Cache-as-Ram again
(but with open source code) and copy the relocated data from the HOB
there. This solution is deemed too hacky. Maybe a lesson can be
learned from this: blobs should not interfere with the execution
environment, as this makes proper integration much harder.

### 4.11\_branch

Given that some platforms supported by FSP1.0 are being produced and
popular, the 4.11 release was made into a branch in which further
development can happen.

Significant changes
-------------------

### SMMSTORE is now production ready

See [smmstore](../drivers/smmstore.md) for the documentation on
the API, but note that there will be an update to it featuring a
much-improved but incompatible API.

### Unit testing infrastructure

Unit testing of coreboot is now possible in a more structured way, with new
build subsystem and adoption of [Cmocka](https://cmocka.org/) framework. Tree
has new directory `tests/`, which comprises infrastructure and examples of unit
tests. See
[Unit testing coreboot](../technotes/2020-03-unit-testing-coreboot.md) for the
design document.

Final Notes
-----------

Your favorite new feature or supported board didn't make it to the
release notes? They're maintained collaboratively in the coreboot
tree, so when you land something noteworthy don't be shy, contribute
to the upcoming release's document in Documentation/releases!
