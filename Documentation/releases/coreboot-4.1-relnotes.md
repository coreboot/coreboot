coreboot 4.1 release notes
==========================

Dear coreboot community,

It has been more than 5 years since we have "released" coreboot 4.0.
That last release marked some very important milestones that we
originally prototyped in the abandoned LinuxBIOS v3 efforts, like the
coreboot filesystem (CBFS), Kconfig support, and (strictly) separate
device trees, build logic and configuration.

Since then there have been as many significant original developments,
such as support for many new architectures (ARM, ARM64, MIPS, RISC-V),
and related architectural changes like access to non-memory mapped SPI
flash, or better insight about the internals of coreboot at runtime
through the cbmem console, timestamp collection, or code coverage
support.

It became clear that a new release is overdue. With our new release
process only slowly getting in shape, I decided to take a random commit
and call it 4.1.

The release itself happens at an arbitrary point in time, but will serve
as a starting point for other activities that require some kind of
starting point to build on, described below.

Future releases will happen more frequently, and with more guarantees
about the state of the release, like having a cool down phase where
boards can be tested and so on. I plan to create a release every three
months, so the changes between any two release don't become too
overwhelming.

With the release of coreboot 4.1, you get an announcement (this email),
a git tag (4.1), and tar archives at <http://www.coreboot.org/releases/>,
for the coreboot sources and the redistributable blobs.

Starting with coreboot 4.1, we will maintain a high level changelog and
'flag days' document. The latter will provide a concise list of changes
which went into coreboot that require chipset or mainboard code to
change to keep it working with the latest upstream coreboot.

For the time being, I will run these efforts, but I'll happily share
documentation duties with somebody else. It is a great opportunity to
keep track of things, learn about the project and its design and various
internals, while contributing to the project without the need to code.

Please contact me (for example by email or on IRC) if you're interested,
and we'll work out how to collaborate on this.

The process should enable users of coreboot to follow releases if they
want a more static base to build on, while making it easier to follow
along with new developments by providing upgrade documentation.

Since moving away from a rolling (non-)release model is new for
coreboot, things may still be a bit rough around the edges, but I'll
provide support for any issues that arise from the release process.

Patrick
