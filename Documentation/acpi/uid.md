# ACPI SSDT \_UID generation

According to the ACPI spec:

> The _UID must be unique across all devices with either a common _HID or _CID.


When generating SSDTs in coreboot the independent drivers don't know
which \_UID is already in use for a specific \_HID or \_CID. To generate
unique \_UIDs the ACPI device's path is hashed and used as ID. As every ACPI
device has a different path, the hash will be also different for every device.

Windows 10 verifies all devices with the same \_HID or \_CID and makes
sure that no \_UID is duplicated. If it is it'll BSOD.
