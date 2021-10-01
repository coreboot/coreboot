# Using coreboot's verified boot on Lenovo devices

By default a single instance of coreboot is present in the firmware flash,
no verification is done and the flash is not write-protected, so as to allow
firmware updates from the OS.
The verified boot mechanism also called [vboot] allows secure firmware
updates using an A/B partitioning scheme once enabled.

## Enabling vboot
You can enable [vboot] in Kconfig's *Security* section. Besides a verified
boot you can also enable a measured boot by setting
`CONFIG_VBOOT_MEASURED_BOOT`. Both options need a working TPM, which is
present on all recent Lenovo devices.

## Updating and recovery
As the A/B partition is writeable you can still update them from the OS.
By using the [vboot] mechanism you store a copy of coreboot in the `RO`
partition that acts as failsafe in case the regular firmware update, that
goes to the `A` or `B` partition fails.

**Note:** The `RO` partition isn't write-protected by default, therefore you
have to enable the protection in the security Kconfig menu by yourself.

On *Lenovo* devices you can enable the *Fn* key as recovery mode switch, by
enabling `CONFIG_H8_FN_KEY_AS_VBOOT_RECOVERY_SW`.
Holding the *Fn* at boot will then switch to the recovery image, allowing
to boot and flash a working image to the A/B partition.

## 8 MiB ROM limitation
*Lenovo* devices with 8 MiB ROM only have a `RO`+`A` partition enabled in the
default FMAP. They are missing the `B` partition, due to size constraints.
You can still provide your own FMAP if you need `RO`+`A`+`B` partitions.

## CMOS
[vboot] on *Lenovo* devices uses the CMOS to store configuration data, like
boot failures and the last successfully booted partition.

[vboot]: ../../security/vboot/index.md
