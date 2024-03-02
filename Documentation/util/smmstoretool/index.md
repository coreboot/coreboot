# smmstoretool

Offline SMMSTORE variable modification tool.

## Operation

### File formats

To discover SMMSTORE, the tool first looks for FMAP header and, if found, for
SMMSTORE region.  If FMAP is found but it has no SMMSTORE region, that's an
error.  If there is no FMAP, the whole file is assumed to be SMMSTORE
region (e.g., extracted via `cbfstool`).

### Storage initialization

If SMMSTORE presence isn't detected and an update operation is requested, the
store spanning the whole region is created automatically.  Size of the store
region must be a multiple of 64 KiB (block size in version 2 of SMMSTORE
protocol), the variable storage itself will be 64 KiB in size.  That's the way
EDK2 makes use of it.

Unlike online editing which mostly appends new variable entries each storage
update with this tool drops all deleted or incomplete entries.

### Unicode

There is no actual support for it.  ASCII bytes (or UTF-8 bytes if that was
passed in) is just extended to 16 bits.  And Unicode chars that are larger than
8 bit are turned into `?`.  Need UTF-8 to/from UTF-16 conversion functions for
proper support.

## Help

Start with:

```
$ smmstoretool -h
Usage: smmstoretool smm-store-file|rom sub-command
       smmstoretool -h|--help

Sub-commands:
 * get    - display current value of a variable
 * guids  - show GUID to alias mapping
 * help   - provide built-in help
 * list   - list variables present in the store
 * remove - remove a variable from the store
 * set    - add or updates a variable in the store
```

Then run `smmstoretool rom help sub-command-name` to get more details.

## Data types

EFI variables in the storage don't have an associated data type and it needs to
be specified on reading/writing variables.  Several basic types that correspond
to typical configuration values are supported:

 * `bool` (`true`, `false`)
 * `uint8` (0-255)
 * `uint16` (0-65535)
 * `uint32` (0-4294967295)
 * `ascii` (NUL-terminated)
 * `unicode` (widened and NUL-terminated)
 * `raw` (output only; raw bytes on output)

## Examples

`SMMSTORE` is the name of a file containing SMMSTORE data or a full ROM image
with FMAP that includes SMMSTORE region.

### Variable listing

```
$ smmstoretool SMMSTORE list
dasharo                            :NetworkBoot (1 byte)
c076ec0c-7028-4399-a07271ee5c448b9f:CustomMode (1 byte)
d9bee56e-75dc-49d9-b4d7b534210f637a:certdb (4 bytes)
9073e4e0-60ec-4b6e-99034c223c260f3c:VendorKeysNv (1 byte)
6339d487-26ba-424b-9a5d687e25d740bc:TCG2_DEVICE_DETECTION (1 byte)
6339d487-26ba-424b-9a5d687e25d740bc:TCG2_CONFIGURATION (1 byte)
6339d487-26ba-424b-9a5d687e25d740bc:TCG2_VERSION (16 bytes)
global                             :Boot0000 (66 bytes)
global                             :Timeout (2 bytes)
global                             :PlatformLang (3 bytes)
global                             :Lang (4 bytes)
global                             :Key0000 (14 bytes)
global                             :Boot0001 (102 bytes)
global                             :Key0001 (14 bytes)
04b37fe8-f6ae-480b-bdd537d98c5e89aa:VarErrorFlag (1 byte)
dasharo                            :Type1UUID (16 bytes)
dasharo                            :Type2SN (10 bytes)
global                             :Boot0002 (90 bytes)
global                             :BootOrder (8 bytes)
global                             :Boot0003 (76 bytes)
...
```

### Variable reading

```
$ smmstoretool SMMSTORE get -g dasharo -n UsbDriverStack -t bool
false
```

### Variable writing

```
$ smmstoretool SMMSTORE set -g dasharo -n UsbDriverStack -t bool -v true
```

### Variable deletion

```
$ smmstoretool SMMSTORE remove -g dasharo -n NetworkBoot
```

### Real-world usage

If one edits a newly generated Dasharo `coreboot.rom`:

```bash
# editing exported storage
cbfstool coreboot.rom read -r SMMSTORE -f SMMSTORE
smmstoretool SMMSTORE set -g dasharo -n NetworkBoot -t bool -v true
cbfstool coreboot.rom write -r SMMSTORE -f SMMSTORE

# editing in-place storage
smmstoretool coreboot.rom set -g dasharo -n NetworkBoot -t bool -v true
```

On the first boot, "Network Boot" setting will already be enabled.

Can also read SMMSTORE from a flash and examine some of its contents for
debugging purposes without adding new logging code or powering on the hardware:

```bash
smmstoretool SMMSTORE get -g global -n BootOrder -t raw | hexdump -C
```
