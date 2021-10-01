# Intel Trusted Execution Technology

Intel TXT allows
1. Attestation of the authenticity of a platform and its operating system.
2. Assuring that an authentic operating system starts in a
   trusted environment, which can then be considered trusted.
3. Providing of a trusted operating system with additional
   security capabilities not available to an unproven one.

Intel TXT requirements:

1. Intel TXT requires a **TPM** to measure parts of the firmware before it's
   run on the BSP.
2. Intel TXT requires signed **Authenticated Code Modules** ([ACM]s), provided
   by Intel.
3. Intel TXT requires **CPU and Chipset** support (supported since
   Intel Core 2 Duo/ICH9).

## Authenticated Code Modules

The ACMs are Intel digitally signed modules that contain code to be run
before the traditional x86 CPU reset vector.

More details can be found here: [Intel ACM].

## Modified bootflow with Intel TXT

With Intel TXT the first instruction executed on the BSP isn't the
*reset vector*, but the [Intel ACM].
It initializes the TPM and measures parts of the firmware, the IBB.

### Marking the Initial Boot Block

Individual files in the CBFS can be marked as IBB.

More details can be found in the [Intel TXT IBB] chapter.

### Measurements
The IBBs (Initial Boot Blocks) are measured into TPM's PCR0 by the BIOS [ACM]
before the CPU reset vector is executed. To identify the regions that need
to be measured, the [FIT] contains one ore multiple *Type 7* entries, that
point to the IBBs.

### Authentication

After the IBBs have been measured, the ACM decides if the boot firmware is
trusted. There exists two validation modes:
1. HASH Autopromotion
   * Uses a known good HASH stored in TPM NVRAM
   * Doesn't allow to boot a fallback IBB
2. Signed BIOS policy
   * Uses a signed policy stored in flash containing multiple HASHes
   * The public key HASH of BIOS policy is burned into TPM by manufacturer
   * Can be updated by firmware
   * Allows to boot a fallback IBB

At the moment only *Autopromotion mode* is implemented and tested well.

In the next step the ACM terminates and the regular x86 CPU reset vector
is being executed on the BSP.

### Protecting Secrets in Memory

Intel TXT sets the `Secrets in Memory` bit, whenever the launch of the SINIT
ACM was successful.
The bit is reset when leaving the *MLE* by a regular shutdown or by removing
the CMOS battery.

When `Secrets in Memory` bit is set and the IBB isn't trusted, the memory
controller won't be unlocked, resulting in a platform that cannot access DRAM.

When `Secrets in Memory` bit is set and the IBB is trusted, the memory
controller will be unlocked, and it's the responsibility of the firmware to
[clear all DRAM] and wipe any secrets of the MLE.
The platform will be reset after all DRAM has been wiped and will boot
with the `Secrets in Memory` bit cleared.

### Configuring protected regions for SINIT ACM

The memory regions used by the SINIT ACM need to be prepared and protected
against DMA attacks.
The SINIT ACM as well as the SINIT handoff data are placed in memory.

### Locking TXT register

As last step the TXT registers are locked.

Whenever the SINIT ACM is invoked, it verifies that the hardware is in the
correct state. If it's not the SINIT ACM will reset the platform.

## For developers
### Configuring Intel TXT in Kconfig
Enable ``INTEL_TXT`` and set the following:

``INTEL_TXT_BIOSACM_FILE`` to the path of the BIOS ACM provided by Intel

``INTEL_TXT_SINITACM_FILE`` to the path of the SINIT ACM provided by Intel
### Print TXT status as early as possible
Add platform code to print the TXT status as early as possible, as the register
is cleared on cold reset.

## References
More information can be found here:
* [Intel TXT Software Development Guide]
* [Intel TXT enabling]
* [FIT]
* [Intel TXT Lab Handout]

[Intel TXT IBB]: txt_ibb.md
[FIT]: ../../soc/intel/fit.md
[Intel ACM]: acm.md
[ACM]: acm.md
[FIT table]: ../../soc/intel/fit.md
[clear all DRAM]: ../memory_clearing.md
[Intel TXT Lab Handout]: https://downloadmirror.intel.com/18931/eng/Intel%20TXT%20LAB%20Handout.pdf
[Intel TXT Software Development Guide]: https://www.intel.com/content/dam/www/public/us/en/documents/guides/intel-txt-software-development-guide.pdf
[Intel TXT enabling]: https://www.intel.com/content/dam/www/public/us/en/documents/guides/txt-enabling-guide.pdf
