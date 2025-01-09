# SMM based flash storage driver

This documents the API exposed by the x86 system management based
storage driver.

## SMMSTORE

SMMSTORE is a [SMM] mediated driver to read from, write to and erase a
predefined region in flash. It can be enabled by setting
`CONFIG_SMMSTORE=y` in menuconfig.

This can be used by the OS or the payload to implement persistent
storage to hold for instance configuration data, without needing
to implement a (platform specific) storage driver in the payload
itself.

The API provides append-only semantics for key/value pairs.

## API

### Storage region

By default SMMSTORE will operate on a separate FMAP region called
`SMMSTORE`. The default generated FMAP will include such a region.
On systems with a locked FMAP, e.g. in an existing vboot setup
with a locked RO region, the option exists to add a cbfsfile
called `smm_store` in the `RW_LEGACY` (if CHROMEOS) or in the
`COREBOOT` FMAP regions. It is recommended for new builds using
a handcrafted FMD that intend to make use of SMMSTORE to include a
sufficiently large `SMMSTORE` FMAP region. It is recommended to
align the `SMMSTORE` region to 64KiB for the largest flash erase
op compatibility.

When a default generated FMAP is used the size of the FMAP region
is equal to `CONFIG_SMMSTORE_SIZE`. UEFI payloads expect at least
64KiB. Given that the current implementation lacks a way to rewrite
key-value pairs at least a multiple of this is recommended.

### generating the SMI

SMMSTORE is called via an SMI, which is generated via a write to the
IO port defined in the smi_cmd entry of the FADT ACPI table. `%al`
contains `APM_CNT_SMMSTORE=0xed` and is written to the smi_cmd IO
port. `%ah` contains the SMMSTORE command. `%ebx` contains the
parameter buffer to the SMMSTORE command.

### Return values

If a command succeeds, SMMSTORE will return with
`SMMSTORE_RET_SUCCESS=0` on `%eax`. On failure SMMSTORE will return
`SMMSTORE_RET_FAILURE=1`. For unsupported SMMSTORE commands
`SMMSTORE_REG_UNSUPPORTED=2` is returned.

**NOTE1**: The caller **must** check the return value and should make
no assumption on the returned data if `%eax` does not contain
`SMMSTORE_RET_SUCCESS`.

**NOTE2**: If the SMI returns without changing `%ax` assume that the
SMMSTORE feature is not installed.

### Calling arguments

SMMSTORE supports 3 subcommands that are passed via `%ah`, the additional
calling arguments are passed via `%ebx`.

**NOTE**: The size of the struct entries are in the native word size of
smihandler. This means 32 bits in almost all cases.


#### - SMMSTORE_CMD_CLEAR = 1

This clears the `SMMSTORE` storage region. The argument in `%ebx` is
unused.

#### - SMMSTORE_CMD_READ = 2

The additional parameter buffer `%ebx` contains a pointer to
the following struct:

```C
struct smmstore_params_read {
	void *buf;
	ssize_t bufsize;
};
```

INPUT:
- `buf`: is a pointer to where the data needs to be read
- `bufsize`: is the size of the buffer

OUTPUT:
- `buf`
- `bufsize`: returns the amount of data that has actually been read.

#### - SMMSTORE_CMD_APPEND = 3

SMMSTORE takes a key-value approach to appending data. key-value pairs
are never updated, they are always appended. It is up to the caller to
walk through the key-value pairs after reading SMMSTORE to find the
latest one.

The additional parameter buffer `%ebx` contains a pointer to
the following struct:

```C
struct smmstore_params_append {
	void *key;
	size_t keysize;
	void *val;
	size_t valsize;
};
```

INPUT:
- `key`: pointer to the key data
- `keysize`: size of the key data
- `val`: pointer to the value data
- `valsize`: size of the value data

#### Security

Pointers provided by the payload or OS are checked to not overlap with the SMM.
That protects the SMM handler from being manipulated.

*However there's no validation done on the source or destination pointing to
DRAM. A malicious application that is able to issue SMIs could extract arbitrary
data or modify the currently running kernel.*

## External links

* [A Tour Beyond BIOS Implementing UEFI Authenticated Variables in SMM with EDK II](https://github.com/tianocore-docs/Docs/raw/master/White_Papers/A_Tour_Beyond_BIOS_Implementing_UEFI_Authenticated_Variables_in_SMM_with_EDKII_V2.pdf)

Note, this differs significantly from coreboot's implementation.

[SMM]: ../security/smm.md
