# Platform independent drivers documentation

The drivers can be found in `src/drivers`. They are intended for onboard
and plugin devices, significantly reducing integration complexity and
they allow to easily reuse existing code across platforms.

For details on how to connect device drivers to a mainboard, see [Driver Devicetree Entries](dt_entries.md).

```{toctree}
:maxdepth: 1
:hidden:

Driver Devicetree Entries <dt_entries.md>
```

Some of the drivers currently available include:

```{toctree}
:maxdepth: 1

ACPI Five-Level Fan Control <acpi_fan_control.md>
CFR <cfr.md>
CFR use within coreboot <cfr_internal.md>
Intel DPTF <dptf.md>
IPMI BT (Block Transfer) <ipmi_bt.md>
IPMI KCS <ipmi_kcs.md>
SMMSTORE <smmstore.md>
SMMSTOREv2 <smmstorev2.md>
SoundWire <soundwire.md>
USB4 Retimer <retimer.md>
CBFS SMBIOS hooks <cbfs_smbios.md>
```
