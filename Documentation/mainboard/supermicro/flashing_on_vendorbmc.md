# Flashing coreboot using SMC IPMI (BMC) firmware

## Metadata
In order to flash anything to the "BIOS" IC, it needs to contain a valid
BIOSINFO struct.

The BIOSINFO struct contains a `$FID` marker at the beginning and is
128 bytes in total. Besides the *BoardID* it contains the *firmware version*
and *build date*. The BMC verifies that the BoardID is correct and refuses to
flash if it's not.

The struct has no checksum or cryptographic protection.

## The smcinfobios tool

The smcbiosinfo tool can be found in `util/supermicro/smcbiosinfo`.

It parses the `build/build.h` header to get the current coreboot version and
build timestamp.
The *board ID* is passed as command line argument.

It will place a file in CBFS called `smcbiosinfo.bin`, which is then found
by the vendor tools. The file contains the struct described above.

## Flashing using SMCIPMItool

You can use the *SMCIPMITool* to remotely flash the BIOS:

`SMCIPMITool <remote BMC IP> <user> <password> bios update build/coreboot.rom`

Make sure that the ME isn't in recovery mode, otherwise you get an error
message on updating the BIOS.

## Flashing with disabled ME

If ME is disabled via `me_cleaner` or the ME recovery jumper, it is still
possible to flash remotely with the [`Supermicro Update Manager`](SUM) (`SUM`).

```sh
./sum -i <remote BMC IP> -u <user> -p <password> -c UpdateBios --reboot \
  --force_update --file build/coreboot.rom
```

[SUM]: https://www.supermicro.com/SwDownload/SwSelect_Free.aspx?cat=SUM
