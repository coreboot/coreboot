# Eltan Security

## Security
This code enables measured boot and verified boot support.
Verified boot is available in coreboot, but based on ChromeOS. This vendorcode
uses a small encryption library and leave much more space in flash for the
payload.

## Hashing Library
The library suppports SHA-1, SHA-256 and SHA-512. The required routines of
`3rdparty/vboot/firmware/2lib` are used.

## Measured boot
measured boot support will use TPM2 device if available. The items specified
in `mb_log_list[]` will be measured.

## Verified boot
verified boot support will use TPM2 device if available. The items specified
in the next table will be verified:
* `bootblock_verify_list[]`
* `verify_item_t romstage_verify_list[]`
* `ram_stage_additional_list[]`
* `ramstage_verify_list[]`
* `payload_verify_list[]`
* `oprom_verify_list[]`

## Enabling support

* Measured boot can be enabled using **CONFIG_MBOOT**
* Create mb_log_list table with list of item to measure
* Create tables bootblock_verify_list[], verify_item_t romstage_verify_list[],
  ram_stage_additional_list[], ramstage_verify_list[], payload_verify_list[],
  oprom_verify_list[]
* Verified boot can be enabled using **CONFIG_VERIFIED_BOOT**
* Added Kconfig values for verbose console output

## Debugging

You can enable verbose console output in *menuconfig*.
