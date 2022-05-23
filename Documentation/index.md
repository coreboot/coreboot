# Welcome to the coreboot documentation

This is the developer documentation for [coreboot](https://coreboot.org).
It is built from Markdown files in the
[Documentation](https://review.coreboot.org/cgit/coreboot.git/tree/Documentation)
directory in the source code.

## Purpose of coreboot

coreboot is a project to develop open source boot firmware for various
architectures. Its design philosophy is to do the bare minimum necessary to
ensure that hardware is usable and then pass control to a different program
called the _payload_.

### Separation of concerns

The payload can then provide user interfaces, file system drivers,
various policies etc. to load the OS. Through this separation of concerns
coreboot maximizes reusability of the complicated and fundamental hardware
initialization routines across many different use cases, no matter if
they provide standard interfaces or entirely custom boot flows.

Popular [payloads](payloads.md) in use with coreboot are SeaBIOS,
which provides PCBIOS services, Tianocore, which provides UEFI services,
GRUB2, the bootloader used by many Linux distributions, or depthcharge,
a custom boot loader used on Chromebooks.

### No resident services (if possible)

Ideally coreboot completely hands over control to the payload with no
piece of coreboot remaining resident in the system, or even available
for callback.  Given the reality of contemporary computer design,
there's often a small piece that survives for the whole runtime of
the computer. It runs in a highly privileged CPU mode (e.g. SMM on x86)
and provides some limited amount of services to the OS. But here, too,
coreboot aims to keep everything at the minimum possible, both in scope
(e.g. services provided) and code size.

### No specification of its own

coreboot uses a very minimal interface to the payload, and otherwise
doesn't impose any standards on the ecosystem. This is made possible by
separating out concerns (interfaces and resident services are delegated
to the payload), but it's also a value that is deeply ingrained in the
project. We fearlessly rip out parts of the architecture and remodel it
when a better way of doing the same was identified.

That said, since there are attempts to coerce coreboot to move in various
directions by outside "standardization", long-established practices of
coreboot as well as aligned projects can be documented as best practices,
making them standards in their own right. However we reserve the right to
retire them as the landscape shifts around us.

### One tree for everything

Another difference to various other firmware projects is that we try
to avoid fragmentation: the traditional development model of firmware
is one of "set and forget" in which some code base is copied, adapted
for the purpose at hands, shipped and only touched again if there's an
important fix to do.

All newer development happens on another copy of some code base without
flowing back to any older copy, and so normally there's a huge amount
of fragmentation.

In coreboot, we try to keep everything in a single source tree, and
lift up older devices when we change something fundamentally. That way,
new and old devices benefit alike from new development in the common parts.

There's a downside to that: Some devices might have no maintainer anymore
who could ensure that coreboot is still functional for them after a big
rework, or maybe a rework even requires knowledge that doesn't exist
anymore within the project (for example because the developer moved on
to do something else).

In this case, we announce the deprecation of the device and defer the big
rework until the deprecation period passed, typically 6-12 months. This
gives interested developers a chance to step in and bring devices up to
latest standards.

While without this deprecation mechanism we could inflate the number
of supported devices (probably 300+), only a tiny fraction of them
would even work, which helps nobody.

## Scope of the coreboot project

coreboot as a project is closer to the Linux kernel than to most
user level programs. One place where this becomes apparent is the
distribution mechanism: The project itself only provides source code
and does not ship ready-to-install coreboot-based firmware binaries.

What the project distributes, even if - strictly speaking - it's not
part of the project, is a collection of vendor binaries (that we call
"blobs") that are redistributable. They cover the parts of hardware init
that we haven't managed to open up, and while some hardware requires them,
there's still hardware that can boot without any such binary components.

The build system can integrate them into the build automatically if
required, but that requires explicit opt-in and downloads a separate
repository to ensure that the distinction remains clear.

There are various [distributions](distributions.md), some shipping
coreboot with their hardware (e.g. Purism or Chromebooks), others
providing after-market images for various devices (e.g. Libreboot,
MrChromebox).

If you want to use coreboot on your system, that's great!

Please note that the infrastructure around coreboot.org is built for
development purposes. We gladly help out users through our communication
channels, but we also expect a "firmware developer mindset": If compiling
your own firmware and, at some point, recovering from a bad flash by
hooking wires onto chips in your computer sounds scary to you, you're
right, as it is.

If that's _way_ beyond your comfort zone, consider looking into the
various distributions, as they typically provide pre-tested binaries
which massively reduces the risk that the binary you write to flash is
one that won't boot the system (with the consequence that to get it to work
again, you'll need to attach various tools to various chips)

## The coreboot community

If you're interested in getting your hands dirty (incl. potentially wiring
up an external flasher to your computer), you've come to the right place!

We have various [forums](community/forums.md) where we discuss and coordinate
our activities, review patches, and help out each other. To
help promote a positive atmosphere, we established a [Code of
Conduct](community/code_of_conduct.md). We invested a lot of time
to balance it out, so please keep it in mind when engaging with the
coreboot community.

Every now and then, coreboot is present in one way or another at
[conferences](community/conferences.md). If you're around, come and
say hello!

## Getting the source code

coreboot is primarily developed in the
[git](https://review.coreboot.org/cgit/coreboot.git) version control
system, using [Gerrit](https://review.coreboot.org) to manage
contributions and code review.

In general we try to keep the `master` branch in the repository functional
for all hardware we support. So far, the only guarantee we can make is
that the master branch will (nearly) always build for all boards in a
standard configuration.

However, we're continually working on improvements to our infrastructure to
get better in that respect, e.g. by setting up boot testing facilities. This
is obviously more complex than regular integration testing, so progress
is slow.

### What our releases mean

We also schedule two source code releases every year, around April and
October. These releases see some very limited testing and mostly serve
as synchronization points for deprecation notices and for other projects
such as external distributions.

This approach and terminology differs somewhat from how other projects handle
releases where releases are well-tested artifacts and the development
repository tends to be unstable. The "rolling release" model of some projects,
for example OpenBSD, is probably the closest cousin of our approach.

Contents:

* [Getting Started](getting_started/index.md)
* [Tutorial](tutorial/index.md)
* [Contributing](contributing/index.md)
* [Community](community/index.md)
* [Payloads](payloads.md)
* [Distributions](distributions.md)
* [Technotes](technotes/index.md)
* [ACPI](acpi/index.md)
* [Native Graphics Initialization with libgfxinit](gfx/libgfxinit.md)
* [Display panel](gfx/display-panel.md)
* [CPU Architecture](arch/index.md)
* [Platform independent drivers](drivers/index.md)
* [Northbridge](northbridge/index.md)
* [System on Chip](soc/index.md)
* [Mainboard](mainboard/index.md)
* [Payloads](lib/payloads/index.md)
* [Libraries](lib/index.md)
* [Options](lib/option.md)
* [Security](security/index.md)
* [SuperIO](superio/index.md)
* [Vendorcode](vendorcode/index.md)
* [Utilities](util.md)
* [Project infrastructure & services](infrastructure/index.md)
* [Boards supported in each release directory](releases/boards_supported_on_branches.md)
* [Release notes](releases/index.md)
* [Documentation License](documentation_license.md)
