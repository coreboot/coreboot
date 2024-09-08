# Intel Firmware Support Package (FSP)-specific documentation

This section contains documentation about Intel-FSP in public domain.

## Integration Guidelines

Some guiding principles when working on the glue to integrate FSP into
coreboot, e.g. on how to configure a board in devicetree when that affects
the way FSP works:

* It should be possible to replace FSP based boot with a native coreboot
  implementation for a given chipset without touching the mainboard code.
* The devicetree configures coreboot and part of what coreboot does with the
  information is setting some FSP UPDs. The devicetree isn't supposed to
  directly configure FSP.

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

* FSP Notify(EnumInitPhaseAfterPciEnumeration) hangs if 00:02.03/00:02.03 are hidden
  * Release MR2
  * Seems to get stuck on some SKUs only if hidden after MemoryInit
  * Workaround: Hide before MemoryInit
  * Issue on public tracker: [Issue 35]

### KabylakeFsp
* MfgId and ModulePartNum in the DIMM_INFO struct are empty
  * Release 3.7.1
  * Those values are typically consumed by SMBIOS type 17
  * Workaround: none
  * Issue on public tracker: [Issue 22]

* MRC forces memory re-training on cold boot on boards with Intel SPS
  * Releases 3.7.1, 3.7.6
  * Workaround: Flash Intel ME instead of SPS
  * Issue on public tracker: [Issue 41]

### BraswellFsp
* Internal UART can't be disabled using PcdEnableHsuart*
  * Release MR2
  * Workaround: Disable internal UART manually after calling FSP
  * Issue on public tracker: [Issue 10]

### CoffeeLakeFsp
* Disabling the internal graphics causes a crash in FSP-M
  * 7.0.68.40 and older version
  * Workaround: Set "tconfig->PanelPowerEnable = 0"
  * Issue on public tracker: [Issue 49]

## Open Source Intel FSP specification

* [About Intel FSP](https://firmware.intel.com/learn/fsp/about-intel-fsp)

* [FSP Specification 1.0](https://www.intel.in/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec.pdf)

* [FSP Specification 1.1](https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec-v1-1.pdf)

* [FSP Specification 2.0](https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/fsp-architecture-spec-v2.pdf)

* [FSP Specification 2.1](https://cdrdv2.intel.com/v1/dl/getContent/611786)

## Additional Features in FSP 2.1 specification

```{toctree}
:maxdepth: 1

PPI <ppi/ppi.md>
```

## Official bugtracker

- [IntelFSP/FSP](https://github.com/IntelFsp/FSP/issues)

[Issue 10]: https://github.com/IntelFsp/FSP/issues/10
[Issue 13]: https://github.com/IntelFsp/FSP/issues/13
[Issue 15]: https://github.com/IntelFsp/FSP/issues/15
[Issue 22]: https://github.com/IntelFsp/FSP/issues/22
[Issue 35]: https://github.com/IntelFsp/FSP/issues/35
[Issue 41]: https://github.com/IntelFsp/FSP/issues/41
[Issue 49]: https://github.com/IntelFsp/FSP/issues/49

