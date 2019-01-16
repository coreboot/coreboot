# Project Ideas

This section collects ideas to improve coreboot and related projects and
should serve as a pool of ideas for people who want to enter the field
of firmware development but need some guidance what to work on.

These tasks can be adopted as part of programs like Google Summer of
Code or by motivated individuals outside such programs.

Each entry should outline what would be done, the benefit it brings
to the project, the pre-requisites, both in knowledge and parts. They
should also list people interested in supporting people who want to work
on them - since we started building this list for Google Summer of Code,
we'll adopt its term for those people and call them mentors.

## Provide toolchain binaries
Our crossgcc subproject provides a uniform compiler environment for
working on coreboot and related projects. Sadly, building it takes hours,
which is a bad experience when trying to build coreboot the first time.

Provide packages/installers of our compiler toolchain for Linux distros,
Windows, Mac OS. For Windows, this should also include the environment
(shell, make, ...).

### Requirements

* coreboot knowledge: Should know how to build coreboot images and where
  the compiler comes into play in our build system.
* other knowledge: Should know how packages or installers for their
  target OS work. Knowledge of the GCC build system is a big plus
* hardware requirements: Nothing special

### Mentors
* Patrick Georgi <patrick@georgi.software>

## Support Power9/Power8 in coreboot
There are some basic PPC64 stubs in coreboot, and there's open hardware
in TALOS2 and its family. While they already have fully open source
firmware, coreboot support adds a unified story for minimal firmware
across architectures.

### Requirements
* coreboot knowledge: Should be familiar with making chipset level
  changes to the code.
* other knowledge: A general idea of the Power architecture, the more,
  the better
* hardware requirements: QEMU Power bring-up exists, and even if it
  probably needs to be fixed up, that shouldn't be an exceedingly large
  task. For everything else, access to real Power8/9 hardware and recovery
  tools (e.g. for external flashing) is required.

### Mentors
* Timothy Pearson <tpearson@raptorengineering.com>

## Support QEMU AArch64 or MIPS
Having QEMU support for the architectures coreboot can boot helps with
some (limited) compatibility testing: While QEMU generally doesn't need
much hardware init, any CPU state changes in the boot flow will likely
be quite close to reality.

That could be used as a baseline to ensure that changes to architecture
code doesn't entirely break these architectures

### Requirements
* coreboot knowledge: Should know the general boot flow in coreboot.
* other knowledge: This will require knowing how the architecture
  typically boots, to adapt the coreboot payload interface to be
  appropriate and, for example, provide a device tree in the platform's
  typical format.
* hardware requirements: since QEMU runs practically everywhere and
  needs no recovery mechanism, these are suitable projects when no special
  hardware is available.

### Mentors
