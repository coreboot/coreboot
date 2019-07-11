# Intel Firmware Support Package (FSP)-specific documentation

This section contains documentation about Intel-FSP in public domain.

## Bugs
As Intel doesn't even list known bugs, they are collected here until
those are fixed. If possible a workaround is described here as well.

### BroadwellDEFsp

* IA32_FEATURE_CONTROL MSR is locked in FSP-M
  * Release MR2
  * Writing the MSR is required in ramstage for Intel TXT
  * Workaround: none
  * Issue on public tracker: [Issue 10]

* FSP-S asserts if the thermal PCI device 00:1f.6 is disabled
  * Release MR2
  * FSP expects the PCI device to be enabled
  * FSP expects BARs to be properly assigned
  * Workaround: Don't disable this PCI device
  * Issue on public tracker: [Issue 13]

### KabylakeFsp
* MfgId and ModulePartNum in the DIMM_INFO struct are empty
  * Release 3.7.1
  * Those values are typically consumed by SMBIOS type 17
  * Workaround: none
  * Issue on public tracker: [Issue 22]

### BraswellFsp
* Internal UART can't be disabled using PcdEnableHsuart*
  * Release MR2
  * Workaround: Disable internal UART manually after calling FSP
  * Issue on public tracker: [Issue 10]


## Open Source Intel FSP specification

* [About Intel FSP](https://firmware.intel.com/learn/fsp/about-intel-fsp)

* [FSP Specification 1.0](https://www.intel.in/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec.pdf)

* [FSP Specification 1.1](https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec-v1-1.pdf)

* [FSP Specification 2.0](https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec-v2.pdf)

## Additional Features in FSP 2.1 specification

- [PPI](ppi/ppi.md)

## Official bugtracker

- [IntelFSP/FSP](https://github.com/IntelFsp/FSP/issues)

[Issue 10]: https://github.com/IntelFsp/FSP/issues/10
[Issue 13]: https://github.com/IntelFsp/FSP/issues/13
[Issue 15]: https://github.com/IntelFsp/FSP/issues/15
[Issue 22]: https://github.com/IntelFsp/FSP/issues/22

