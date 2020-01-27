# Eltan Security

This code enables measured boot and verified boot support.
Verified boot is available in coreboot, but based on ChromeOS. This vendorcode security
solution is intended to be used for system without ChromeOS support.

This solution allows implementing verified boot support for systems that do not contain a TPM.

## Hashing Library
The API functions of `3rdparty/vboot/firmware` are used.

## Measured boot
Measured boot support requires a TPM2 device.

The items specified in `mb_log_list[]` and `*_verify_list[]` will be measured.

The `mb_log_list[]` should only contain items that are not contained in one of the verify_lists
below (except for the `bootblock_verify_list[]`).

The list can contain the following items: `config`, `revision`, `cmos_layout.bin`.
`oemmanifest.bin` should be added to the list when Verified boot is enabled.

## Verified boot
Verified boot support will use the OEM manifest to verify the items.

The verification process is controlled using the following verify lists:
* `bootblock_verify_list[]` (will not be measured, verified in bootblock)
* `romstage_verify_list[]`  (verified in early romstage)
* `postcar_verify_list[]`   (verified in just before postcar loading)
* `ramstage_verify_list[]`  (verified in just before ramstage loading)
* `payload_verify_list[]`   (verified in just before payload loading)
* `oprom_verify_list[]`     (verified before option rom execution)

A verify_list entry contains a `related_items` member. This can point to an additional `verify_list`
which will be verified before the specified item is verified. As an example the `grub` entry in
`payload_verify_list[]` can point to the `grub_additional_list[]` that contains the items used by
the grub payload and the `seabios` entry in `payload_verify_list[]` can point to the
`seabios_additional_list[]` that contains the items used by the seabios payload. By doing this the
entries that are verified (and measured) depend on the payload selected at runtime.

## Creating private and public keys
Create private key in RSA2048 format: `openssl genrsa -F4 -out <private_key_file> 2048`

Create public key using private key:
`futility --vb1 create <private_key_file> <public_key_file_without_extension>`

The public key will be included into coreboot and used for verified boot only.

## Enabling support
To enable measured boot support:
* Enabled *VENDORCODE_ELTAN_MBOOT*
* Create `mb_log_list` table with list of items to measure

To enable verified boot support:
* Enable *VENDORCODE_ELTAN_VBOOT*
* Create the verify lists `*_verify_list[]`
* *VENDORCODE_ELTAN_VBOOT_KEY_FILE* must point to location of the public key file created with `futility`

## Creating signed binary

During build of coreboot binary an empty `oemmanifest.bin` is added to the binary.

This binary must be replaced by a correct (signed) binary when *VENDORCODE_ELTAN_VBOOT* is enabled

The `oemmanifest.bin` file contains the SHA-256 (or SHA-512) hashes of all the different parts
contained in verify_lists.

When *VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST* is enabled the manifest should be signed and the
signature should appended to the manifest.

Please make sure the public key is in the RO part of the coreboot image. The `oemmanifest.bin` file
should be in the RW part of the coreboot image.

### Hashing

The `oemmanifest.bin` file contains the hashes of different binaries parts of the binary e.g.:
bootblock, romstage, postcar, ramstage, fsp etc.

The total number of items must match `VENDORCODE_ELTAN_OEM_MANIFEST_ITEMS`.

For every part the SHA (SHA-256) must be calculated. First extract the binary from the coreboot
image using: `cbfstool <coreboot_file_name> extract -n <cbfs_name> -f <item_binary_file_name>`
followed by: `openssl dgst -sha256 -binary -out <hash_file_name> <item_binary_file_name>`

Replace -sha256 with -sha512 when `VENDORCODE_ELTAN_VBOOT_USE_SHA512` is enabled.

All the hashes must be combined to a hash binary. The hashes need to be placed in the same order as
defined by the `HASH_IDX_XXX` values.

### Signing

The oemmanifest needs to be signed when `VENDORCODE_ELTAN_VBOOT_SIGNED_MANIFEST` is enabled.

This can be done with the following command:
`openssl dgst -sign <private_key_file_name> -sha256 -out <signature_binary> <hash_binary>`

The signed manifest can be created by adding the signature to the manifest:
`cat <hash_binary> <signature_binary> >hash_table.bin`

## Create binary
The `oemmanifest.bin` file must be replaced in the coreboot binary by the generated
`hash_table.bin`.

To replace the binary: Remove using:
`cbfstool <coreboot_file_name> remove -n oemmanifest.bin`
Then add the new image using:
`cbfstool coreboot.bin add -f <hash_table_file_name> -n oemmanifest.bin -t raw \`
`-b <CONFIG_VENDORCODE_ELTAN_OEM_MANIFEST_LOC>`

## Debugging

You can enable verbose console output in *menuconfig*.
