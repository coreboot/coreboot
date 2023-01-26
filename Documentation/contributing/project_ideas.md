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

## Easy projects

This is a collection of tasks which don't require deep knowledge on
coreboot itself. If you are a beginner and want to get familiar with the
the project and the code base, or if you just want to get your hands
dirty with some easy tasks, then these are for you.

  * Resolve static analysis issues reported by [scan-build] and
    [Coverity scan]. More details on the page for
    [Coverity scan integration].

  * Resolve issues reported by the [linter][Linter issues]

[scan-build]: https://coreboot.org/scan-build/
[Coverity scan]: https://scan.coverity.com/projects/coreboot
[Coverity scan integration]: ../infrastructure/coverity.md
[Linter issues]: https://qa.coreboot.org/job/coreboot-untested-files/lastSuccessfulBuild/artifact/lint.txt

## Provide toolchain binaries
Our crossgcc subproject provides a uniform compiler environment for
working on coreboot and related projects. Sadly, building it takes hours,
which is a bad experience when trying to build coreboot the first time.

Provide packages/installers of our compiler toolchain for Linux distros,
Windows, Mac OS. For Windows, this should also include the environment
(shell, make, ...). A student doesn't have to cover _all_ platforms, but
pick a set of systems that match their interest and knowledge and lay
out a plan on how to do this.

The scripts to generate these packages should be usable on a Linux
host, as that's what we're using for our automated build testing system
that we could extend to provide current packages going forward. This
might include automating some virtualization system (eg. QEMU or CrosVM) for
non-Linux builds or Docker for different Linux distributions.

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

## Port payloads to ARM, AArch64 or RISC-V
While we have a rather big set of payloads for x86 based platforms, all other
architectures are rather limited. Improve the situation by porting a payload
to one of the platforms, for example GRUB2, U-Boot (the UI part), edk2,
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

## Extend Ghidra to support analysis of firmware images
[Ghidra](https://ghidra-sre.org) is a recently released cross-platform
disassembler and decompiler that is extensible through plugins. Make it
useful for firmware related work: Automatically parse formats (eg. by
integrating UEFITool, cbfstool, decompressors), automatically identify
16/32/64bit code on x86/amd64, etc.

This has been done in 2019 with [some neat
features](https://github.com/al3xtjames/ghidra-firmware-utils) being
developed, but it may be possible to expand support for all kinds of firmware
analyses.

## Learn hardware behavior from I/O and memory access logs
[SerialICE](https://www.serialice.com) is a tool to trace the behavior of
executable code like firmware images. One result of that is a long log file
containing the accesses to hardware resources.

It would be useful to have a tool that assists a developer-analyst in deriving
knowledge about hardware from such logs. This likely can't be entirely
automatic, but a tool that finds patterns and can propagate them across the
log (incrementially raising the log from plain I/O accesses to a high-level
description of driver behavior) would be of great use.

This is a research-heavy project.

### Requirements
* Driver knowledge: Somebody working on this should be familiar with
  how hardware works (eg. MMIO based register access, index/data port
  accesses) and how to read data sheets.
* Machine Learning: ML techniques may be useful to find structure in traces.

### Mentors
* Ron Minnich <rminnich@google.com>

## Libpayload based memtest payload
[Memtest86+](https://www.memtest.org/) has some limitations: first and
foremost it only works on x86, while it can print to serial console the
GUI only works in legacy VGA mode.

This project would involve porting the memtest suite to libpayload and
build a payload around it.

### Requirements
* coreboot knowledge: Should know how to build coreboot images and
  include payloads.
* other knowledge: Knowledge on how dram works is a plus.
* hardware requirements: Initial work can happen on qemu targets,
  being able to test on coreboot supported hardware is a plus.

### Mentors
* TODO

## Fix POST code handling
coreboot supports writing POST codes to I/O port 80.
There are various Kconfigs that deal with POST codes, which don't have
effect on most platforms.
The code to send POST codes is scattered in C and Assembly, some use
functions, some use macros and others simply use the `outb` instruction.
The POST codes are duplicated between stages and aren't documented properly.


Tasks:
* Guard Kconfigs with a *depends on* to only show on supported platforms
* Remove duplicated Kconfigs
* Replace `outb(0x80, ...)` with calls to `post_code(...)`
* Update Documentation/POSTCODES
* Use defines from console/post_codes.h where possible
* Drop duplicated POST codes
* Make use of all possible 255 values

### Requirements
* knowledge in the coreboot build system and the concept of stages
* other knowledge: Little experience with C and x86 Assembly
* hardware requirements: Nothing special

### Mentors
* Patrick Rudolph <patrick.rudolph@9elements.com>
* Christian Walter <christian.walter@9elements.com>

## Board status replacement
The [Board status page](https://coreboot.org/status/board-status.html) allows
to see last working commit of a board. The page is generated by a cron job
that runs on a huge git repository.

Build an open source replacement written in Golang using existing tools
and libraries, consisting of a backend, a frontend and client side
scripts. The backend should connect to an SQL database with can be
controlled using a RESTful API. The RESTful API should have basic authentication
for management tasks and new board status uploads.

At least one older test result should be kept in the database.

The frontend should use established UI libraries or frameworks (for example
Angular) to display the current board status, that is if it's working or not
and some details provided with the last test. If a board isn't working the last
working commit (if any) should be shown in addition to the broken one.

Provide a script/tool that allows to:
1. Push mainboard details from coreboot master CI
2. Push mainboard test results from authenticated users containing
   * working
   * commit hash
   * bootlog (if any)
   * dmesg (if it's booting)
   * timestamps (if it's booting)
   * coreboot config

### Requirements
* coreboot knowledge: Non-technical, needed to perform requirements analysis
* software knowledge: Golang, SQL for the backend, JS for the frontend

### Mentors
* Patrick Rudolph <patrick.rudolph@9elements.com>
* Christian Walter <christian.walter@9elements.com>
