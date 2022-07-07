# CBFS SMBIOS hooks

The document describes the coreboot options how to make CBFS files populate
platform-unique SMBIOS data.

## SMBIOS Serial Number

The [DMTF SMBIOS specification] defines a field in the type 1 System
Information and type 2 Baseboard Information called Serial Number. It
is a null-terminated string field assumed to be unique per platform. Certain
mainboard ports have SMBIOS hooks to generate the Serial Numbers from external
data, e.g. Lenovo Thinkpads (see DRIVER_LENOVO_SERIALS). This driver aims to
provide an option to populate the Serial Numbers from CBFS for boards that
can't generate the it from any source.

### Usage

In the coreboot configuration menu (`make menuconfig`) go to `Generic Drivers`
and select an option `Serial number in CBFS`. The Kconfig system will enable
`DRIVERS_GENERIC_CBFS_SERIAL` and the relevant code parts will be compiled into
coreboot image.

After the coreboot build for your board completes, use the cbfstool to include
the file containing the serial number:

```shell
./build/cbfstool build/coreboot.rom add -n serial_number -t raw -f /path/to/serial_file.txt
```

Where `serial_file.txt` is the unterminated string representation of the SMBIOS
type 1 or type 2 Serial Number, e.g. `5Q4Q7Y1`. If you use vboot with 1 or 2 RW
partitions you will have to specify the RW regions where the file is going to
be added too. By default the RW CBFS partitions are truncated, so the files
would probably not fit, one needs to expand them first.

```shell
./build/cbfstool build/coreboot.rom expand -r FW_MAIN_A
./build/cbfstool build/coreboot.rom add -n serial_number -t raw \
	-f /path/to/serial_file.txt -r FW_MAIN_A
./build/cbfstool build/coreboot.rom truncate -r FW_MAIN_A

./build/cbfstool build/coreboot.rom expand -r FW_MAIN_B
./build/cbfstool build/coreboot.rom add -n serial_number -t raw \
	-f /path/to/serial_file.txt -r FW_MAIN_B
./build/cbfstool build/coreboot.rom truncate -r FW_MAIN_B
```

By default cbfstool adds files to COREBOOT region only, so when vboot is
enabled and the platform is booting from RW partition, the file would not be
picked up by the driver.

One may retrieve the Serial Number from running system (if it exists) using one
of the following commands:

```shell
# Type 1
echo -n `sudo dmidecode -s system-serial-number` > serial_file.txt
# OR Type 2
echo -n `sudo dmidecode -s baseboard-serial-number` > serial_file.txt
```

Ensure the file does not end with whitespaces like LF and/or CR. The above
commands will not add any whitespaces. The driver automatically terminates the
Serial Number with the NULL character. If the CBFS file is not present, the
driver will fall back to the string defined in `MAINBOARD_SERIAL_NUMBER` build
option.

Please note that this driver provides `smbios_mainboard_serial_number` hook
overriding the default implementation which returns `MAINBOARD_SERIAL_NUMBER`
build option. If you wish to populate only type 2 Serial Number field your
board code needs to implement `smbios_system_serial_number`, otherwise the weak
implementation of `smbios_system_serial_number` will call
`smbios_mainboard_serial_number` from the `DRIVERS_GENERIC_CBFS_SERIAL`
implementation overriding it. So selecting the `DRIVERS_GENERIC_CBFS_SERIAL`
has a side-effect of populating both SMBIOS type 1 and type 2 Serial Numbers
if the board does not implement its own `smbios_system_serial_number`.

There is also SMBIOS type 3 Chassis Information Serial Number, but it is not
populated by `DRIVERS_GENERIC_CBFS_SERIAL` nor by the default weak
implementation (returns empty string). If you wish to populate type 3 Serial
Number, your board code should override the default
`smbios_chassis_serial_number` weak implementation.

## SMBIOS System UUID

The [DMTF SMBIOS specification] defines a field in the type 1 System
Information Structure called System UUID. It is a 16 bytes value compliant with
[RFC4122] and assumed to be unique per platform. Certain mainboard ports have
SMBIOS hooks to generate the UUID from external data, e.g. Lenovo Thinkpads
(see DRIVER_LENOVO_SERIALS). This driver aims to provide an option to populate
the UUID from CBFS for boards that can't generate the UUID from any source.

### Usage

In the coreboot configuration menu (`make menuconfig`) go to `Generic Drivers`
and select an option `System UUID in CBFS`. The Kconfig system will enable
`DRIVERS_GENERIC_CBFS_UUID` and the relevant code parts will be compiled into
coreboot image.

After the coreboot build for your board completes, use the cbfstool to include
the file containing the UUID:

```shell
./build/cbfstool build/coreboot.rom add -n system_uuid -t raw -f /path/to/uuid_file.txt
```

Where `uuid_file.txt` is the unterminated string representation of the SMBIOS
type 1 UUID, e.g. `4c4c4544-0051-3410-8051-b5c04f375931`. If you use vboot with
1 or 2 RW partitions you will have to specify the RW regions where the file is
going to be added too. By default the RW CBFS partitions are truncated, so the
files would probably not fit, one needs to expand them first.

```shell
./build/cbfstool build/coreboot.rom expand -r FW_MAIN_A
./build/cbfstool build/coreboot.rom add -n system_uuid -t raw \
	-f /path/to/uuid_file.txt -r FW_MAIN_A
./build/cbfstool build/coreboot.rom truncate -r FW_MAIN_A

./build/cbfstool build/coreboot.rom expand -r FW_MAIN_B
./build/cbfstool build/coreboot.rom add -n system_uuid -t raw \
	-f /path/to/uuid_file.txt -r FW_MAIN_B
./build/cbfstool build/coreboot.rom truncate -r FW_MAIN_B
```

By default cbfstool adds files to COREBOOT region only, so when vboot is
enabled and the platform is booting from RW partition, the file would not be
picked up by the driver.

One may retrieve the UUID from running system (if it exists) using the
following command:

```shell
echo -n `sudo dmidecode -s system-uuid` > uuid_file.txt
```

The above command ensures the file does not end with whitespaces like LF and/or
CR. The above command will not add any whitespaces. But the driver will handle
situations where up to 2 additional bytes like CR and LF will be included in
the file. Any more than that will make the driver fail to populate UUID in
SMBIOS.

[DMTF SMBIOS specification]: https://www.dmtf.org/standards/smbios
[RFC4122]: https://www.ietf.org/rfc/rfc4122.txt
