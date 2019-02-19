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

The requirements for each project aim for productive work on the project,
but it's always possible to learn them "on the job". If you have any
doubt if you can bring yourself up to speed in a required time frame
(e.g. for GSoC), feel free to ask in the community or the mentors listed
with the projects. We can then try together to figure out if you're a
good match for a project, even when requirements might not all be met.

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

## Add Kernel Address Sanitizer functionality to coreboot
The Kernel Address Sanitizer (KASAN) is a runtime dynamic memory error detector.
The idea is to check every memory access (variables) for its validity
during runtime and find bugs like stack overflow or out-of-bounds accesses.
Implementing this stub into coreboot like "Undefined behavior sanitizer support"
would help to ensure code quality and make the runtime code more robust.

### Requirements
* knowledge in the coreboot build system and the concept of stages
* the KASAN feature can be improved in a way so that the memory space needed
  during runtime is not on a fixed address provided during compile time but
  determined during runtime. For this to achieve a small patch to the GCC will
  be helpful. Therefore minor GCC knowledge would be beneficial.
* Implementation can be initially done in QEMU and improved on different
  mainboards and platforms

### Mentors
* Werner Zeh <werner.zeh@gmx.net>

## Port payloads to ARM, AArch64, MIPS or RISC-V
While we have a rather big set of payloads for x86 based platforms, all other
architectures are rather limited. Improve the situation by porting a payload
to one of the platforms, for example GRUB2, U-Boot (the UI part), Tianocore,
yabits, FILO, or Linux-as-Payload.

Since this is a bit of a catch-all idea, an application to GSoC should pick a
combination of payload and architecture to support.

### Requirements
* coreboot knowledge: Should know the general boot flow in coreboot
* other knowledge: It helps to be familiar with the architecture you want to
  work on.
* hardware requirements: Much of this can be done in QEMU or other emulators,
  but the ability to test on real hardware is a plus.

### Mentors
* Simon Glass <sjg@chromium.org> for U-Boot payload projects

## Fully support building coreboot with the Clang compiler
Most coreboot code is written in C, and it would be useful to support
a second compiler suite in addition to gcc. Clang is another popular
compiler suite and the build system generally supports building coreboot
with it, but firmware is a rather special situation and we need to
adjust coreboot and Clang some more to get usable binaries out of that
combination.

The goal would be to get the emulation targets to boot reliably first,
but also to support real hardware. If you don't have hardware around,
you likely will find willing testers for devices they own and work from
their bug reports.

### Requirements
* coreboot knowledge: Have a general concept of the build system
* Clang knowledge: It may be necessary to apply minor modifications to Clang
  itself, but at least there will be Clang-specific compiler options etc to
  adapt, so some idea how compilers work and how to modify their behavior is
  helpful.
* hardware requirements: If you have your own hardware that is already
  supported by coreboot that can be a good test target, but you will debug
  other people's hardware, too.
* debugging experience: It helps if you know how to get the most out of a bug
  report, generate theories, build patches to test them and figure out what's
  going on from the resulting logs.

### Mentors
* Patrick Georgi <patrick@georgi.software>
