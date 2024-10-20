# AMD Family 15h [SOC|Processors]

## Abstract

Family 15h is a line of AMD x86 products first introduced in 2011. The initial
microarchitecture, codenamed "Bulldozer", introduced the concept of a "Compute
Unit" (CU) where some parts of the processor are shared between two cores and
some parts are unique for each core. Family 15h offerings matured into various
models with increased performance and features targeting Enterprise, Client,
and Embedded designs. Notice that a particular model can address more than one
market(see models references below).

## Introduction

The first CU designs were 2 x86 cores with separate integer processors but
sharing cache, code branch prediction engine and floating point processor. A die
can have up to 8 CU. The floating point processor is composed of two symmetrical
128-bit FMAC. Provided each x86 core is doing 128-bit floating point arithmetic,
they both do floating point simultaneously. If one is doing 256-bit floating
point, the other x86 core can't do floating point simultaneously. Later models
changed how resources were shared, and introduced other performance improvements.

Family 15h products range from SOCs to 3-chip solutions. Devices designed to
contain on-die graphics (including headless) are commonly referred to as APUs,
not CPUs.

Later SOCs include a Platform Security Processor (PSP), a small ARM processor
responsible for security related measures: For example, if secure boot is
enabled, the cores will not exit reset until the BIOS image within the SPI
flash is authenticated through its OEM signature, thus ensuring that only OEM
produced BIOS can run the platform.

Support in coreboot for modern AMD products is based on AMD’s reference code:
AMD Generic Encapsulated Software Architecture (AGESA™). AGESA contains the
code for enabling DRAM, configuring proprietary core logic, assistance with
generating ACPI tables, and other features.

While coreboot contains support for most models, some implementations use a
separate cpu/north/south bridge directory structure. Newer products for models
60h-6Fh (Merlin Falcon) and 70h-7Fh (Stoney Ridge) rely on modern SOC directory
structure.

## References

1. [Models 00h-0Fh BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/42301_15h_Mod_00h-0Fh_BKDG.pdf)
2. [Models 10h-1Fh BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/42300_15h_Mod_10h-1Fh_BKDG.pdf)
3. [Models 30h-3Fh BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/49125_15h_Models_30h-3Fh_BKDG.pdf)
4. [Models 60h-6Fh BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/50742_15h_Models_60h-6Fh_BKDG.pdf)
5. [Models 70h-7Fh BKDG](https://www.amd.com/content/dam/amd/en/documents/archived-tech-docs/programmer-references/55072_AMD_Family_15h_Models_70h-7Fh_BKDG.pdf)
6. [PSP Integration](psp_integration.md)
