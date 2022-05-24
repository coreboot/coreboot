# Platform independent drivers documentation

The drivers can be found in `src/drivers`. They are intended for onboard
and plugin devices, significantly reducing integration complexity and
they allow to easily reuse existing code across platforms.

For details on how to connect device drivers to a mainboard, see [Driver Devicetree Entries](dt_entries.md).

Some of the drivers currently available include:

* [Intel DPTF](dptf.md)
* [IPMI KCS](ipmi_kcs.md)
* [SMMSTORE](smmstore.md)
* [SMMSTOREv2](smmstorev2.md)
* [SoundWire](soundwire.md)
* [USB4 Retimer](retimer.md)
* [CBFS SMBIOS hooks](cbfs_smbios.md)
