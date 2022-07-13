coreboot 4.15
================================

coreboot 4.15 was released on November 5th, 2021.

Since 4.14 there have been more than 2597 new commits by more than 219 developers.
Of these, over 73 contributed to coreboot for the first time.

Welcome to the project!

Thank you to all the developers who continue to make coreboot the
great open source firmware project that it is.

Important Announcement
----------------------
We are going to be changing the cadence from every 6 months, to every 3 months.
That means the 4.16 release will be coming in February, 2022.


New mainboards
--------------
* Asus p8h61-m_pro_cm6630
* Asus p8h77-v
* Asus p8z77-v
* Google nipperkin
* Lenovo w541
* Siemens mc_ehl
* Supermicro x9sae
* System76 addw1
* System76 addw2
* System76 bonw14
* System76 darp6
* System76 darp7
* System76 galp2
* System76 galp3
* System76 galp3-b
* System76 galp4
* System76 galp5
* System76 gaze14
* System76 lemp10
* System76 oryp7
* System76 oryp8

Removed mainboards
------------------
* Google Mancomb

Deprecations and incompatible changes
-------------------------------------

### COREBOOTPAYLOAD option

Drop the deprecated COREBOOTPAYLOAD option, and replace it with MrChromebox's
updated UefiPayloadPkg option. Simplify the Kconfig options to make it easier
to build from upstream edk2 master. Drop the EDK2_USE_8254_TIMER Kconfig
option since it applies only to CorebootPayloadPkg. Clean up the Makefile now
that we're only building from a single edk2 package/target.

### Remove old lp4x and ddr4 versions of spd_tools

The migration to the new unified version of spd_tools is complete, so
the old lp4x and ddr4 versions can be removed.

### Remove AMD PI 00630F01

No board currently uses AMD PI 00630F01 so remove it.

Significant changes
-------------------

### Merged family of Asus mainboards using H61 chipset

By using newer coreboot features like board variants and override devicetrees,
lots of code can now be shared. This should ease maintenance and also make it
easier for newcomers to add support for even more mainboards.

### Changed default setting for Intel chipset lockdown

Previously, the default behaviour for Intel chipset lockdown was to let the FSP
do it. Since all related mainboards used the coreboot mechanisms for chipset
lockdown, the default behaviour was changed to that.

### Payloads unit testing

Libpayload now supports the mock architecture, which can be used for unit testing
payloads. (For examples see
[depthcharge](https://chromium.googlesource.com/chromiumos/platform/depthcharge/)
payload)

### Unit testing infrastructure

Unit testing of libpayload is now possible in the same fashion as in the main
coreboot tree.

### Introduce new method for accessing cpu_info

There is currently a fundamental flaw in the current cpu_info()
implementation. It assumes that current stack is CONFIG_STACK_SIZE
aligned. This assumption breaks down when performing SMM relocation.

The first step in performing SMM relocation is changing the SMBASE. This
is accomplished by installing the smmstub at 0x00038000, which is the
default SMM entry point. The stub is configured to set up a new stack
with the size of 1 KiB (CONFIG_SMM_STUB_STACK_SIZE), and an entry point
of smm_do_relocation located in RAMSTAGE RAM.

This means that when smm_do_relocation is executed, it is running in SMM
with a different sized stack. When cpu_info() gets called it will be
using CONFIG_STACK_SIZE to calculate the location of the cpu_info
struct. This results in reading random memory. Since cpu_info() has to
run in multiple environments, we can't use a compile time constant to
locate the cpu_info struct.

This CL introduces a new way of locating cpu_info. It uses a per-cpu
segment descriptor that points to a per-cpu segment that is allocated on
the stack. By using a segment descriptor to point to the per-cpu data,
we no longer need to calculate the location of the cpu_info struct. This
has the following advantages:
* Stacks no longer need to be CONFIG_STACK_SIZE aligned.
* Accessing an unconfigured segment will result in an exception. This
  ensures no one can call cpu_info() from an unsupported environment.
* Segment selectors are cleared when entering SMM and restored when
  leaving SMM.
* There is a 1:1 mapping between cpu and cpu_info. When using
  COOP_MULTITASKING, a new cpu_info is currently allocated at the top of
  each thread's stack. This no longer needs to happen.
