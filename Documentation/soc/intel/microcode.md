# Microcode updates

When booting a modern x86 platform, one task of the firmware is to update
[microcode] to correct hardware bugs and mitigate security issues found
after silicon has been shipped. The [Pentium FDIV bug] could have been
fixed with a microcode update, had the Pentium used updateable microcode.
Starting with the Pentium Pro, CPU microcode can be updated by software.

As per BIOS Writer's Guides, Intel defines a processor as the silicon and
the accompanying microcode update, and considers any processor that does
not have its microcode updated to be running out of specification. This
suggests that microcode is a crucial ingredient for correct operation.

On multi-processor or Hyper-Threading-enabled systems, each thread has
its own microcode. Therefore, microcode must be updated on every thread.

## When to update microcode

When a CPU core comes out of reset, it uses microcode from an internal
ROM. This “default” microcode often contains bugs, so it needs to be
updated as soon as possible. For example, Core 2 CPUs can boot without
microcode updates, but have stability problems. On newer platforms,
it is nearly impossible to boot without having updated the microcode.
On some platforms, an updated microcode is required in order to enable
Cache-As-RAM or to be able to successfully initialize the DRAM.

Plus, microcode needs to be loaded multiple times. Intel Document 504790
explains that this is because of so-called *enhanced microcode updates*,
which are large updates with errata workarounds for both core and uncore.
In order to correctly apply enhanced microcode updates, the [MP-Init]
algorithm must be decomposed into multiple initialization phases.

### Firmware Interface Table

Beginning with 4th generation Intel Core processors, it is possible for
microcode to be updated before the CPU is taken out of reset. This is
accomplished by means of [FIT], a data structure which contains pointers
to various firmware ingredients in the BIOS flash.

In rare cases, FIT microcode updates may not be successful. Therefore,
it is important to check that the microcode is up-to-date and, if not,
update it. This needs to be done as early as possible, like on older
processor generations without FIT support.

Whether all threads on a processor get their microcode updated through
FIT is not clear. According to Intel Documents 493770 and 535094, FIT
microcode updates are applied to all cores within the package. However,
Intel Document 550049 states that FIT microcode updates are applied to
all threads within the package.

## SMM bring-up

Prior to SMM relocation, microcode must have been updated at least once.

## Multi-Processor bring-up

The BWG briefly describes microcode updates as part of the *MP-Init*.

### MP-Init

As part of the [MP-Init] sequence, two microcode updates are required.

* The first update must happen as soon as one AP comes out of reset.
* The second update must happen after the MP-Init sequence has written MTRRs,
  PRMRR, DCU mode and prefetcher configuration, SMM has been relocated, but
  before clearing the MCE banks.

## Recommendations

The Linux kernel developer's recommendations are:
* Serialize microcode updates if possible.
* Idle as many APs as possible while updating.
* Idle the sibling thread on a Hyper-Threading enabled CPU while updating.

## Platform BWGs

The requirements specified in BWGs differ between platforms:

### Sandy Bridge

* Before setting up Cache-As-RAM, load microcode update into the SBSP.
* Losing (non-SBSP) NBSPs must load their microcode update before being placed
  back in the wait-for-SIPI state.
* Sibling threads on HT must use a semaphore.
* Microcode update loading has been done prior to SMM relocation.
* In MP-Init the microcode update on an AP must be done before initializing the
  cache, MTRRs, SMRRs and PRMRRs.
* In MP-Init a second update must happen on all threads after initializing the
  cache, MTRRs, SMRRs and PRMRRs.

Refer to Intel Document 504790 for details.

### Haswell/Broadwell Client

* A microcode update must exist in FIT.
* During the race to the BSP semaphore, each NBSP must load its microcode update.
* All HT enabled threads can load microcode in parallel. However, the
  IA32_BIOS_UPDT_TRIG MSR is core-scoped, just like on other platforms.
* Microcode update loading has been done prior to SMM relocation.
* In MP-Init the microcode update on an AP must be done before initializing the
  cache, MTRRs, SMRRs and EMRR.
* In MP-Init a second update must happen on all threads after initializing the
  cache, MTRRs, SMRRs and EMRR and after SMM initialization.

Refer to Intel Document 493770 and 535094 for details.

### Broadwell Server

* A microcode update must exist in FIT.
* Before setting up Cache-As-RAM, load microcode update into each BSP.
* In MP-Init the microcode update on an AP must be done before initializing the
  cache, MTRRs, SMRRs and EMRR.
* In MP-Init a second update must happen on all threads after initializing the
  cache, MTRRs, SMRRs and EMRR and after SMM initialization.

Refer to Intel Document 546625 for details.

### Skylake/Kaby Lake/Coffee Lake/Whiskey Lake/Comet Lake

* A microcode update must exist in FIT.
* Before setting up Cache-As-RAM, load microcode update into the BSP.
* Microcode update loading has been done prior to SMM relocation.
* In MP-Init the first update must happen as soon as one AP comes out of reset.
* In MP-Init the second update must happen after the MP-Init sequence has
  written MTRRs, PRMRR, DCU mode and prefetcher configuration, but before
  clearing the MCE banks.
* Microcode updates must happen on all threads.
* Sibling threads on HT should use a semaphore.

Refer to Intel Document 550049 for details.

[microcode]: https://en.wikipedia.org/wiki/Microcode
[Pentium FDIV bug]: https://en.wikipedia.org/wiki/Pentium_FDIV_bug
[FIT]: fit.md
[SDM]: https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf
[MP-Init]: mp_init/mp_init.md
