# SMM based flash storage driver Version 2

This documents the API exposed by the x86 system management based
storage driver.

## SMMSTOREv2

SMMSTOREv2 is a [SMM] mediated driver to read from, write to and erase
a predefined region in flash. It can be enabled by setting
`CONFIG_SMMSTORE=y` and `CONFIG_SMMSTORE_V2=y` in menuconfig.

This can be used by the OS or the payload to implement persistent
storage to hold for instance configuration data, without needing to
implement a (platform specific) storage driver in the payload itself.

### Storage size and alignment

SMMSTORE version 2 requires a minimum alignment of 64 KiB, which should
be supported by all flash chips. Not having to perform read-modify-write
operations is desired, as it reduces complexity and potential for bugs.

This can be used by a FTW (FaultTolerantWrite) implementation that uses
at least two regions in an A/B update scheme. The FTW implementation in
edk2 uses three different regions in the store:

- The variable store
- The FTW spare block
- The FTW working block

All regions must be block-aligned, and the FTW spare size must be larger
than that of the variable store. FTW working block can be much smaller.
With 64 KiB as block size, the minimum size of the FTW-enabled store is:

- The variable store: 1 block = 64 KiB
- The FTW spare block: 2 blocks = 2 * 64 KiB
- The FTW working block: 1 block = 64 KiB

Therefore, the minimum size for edk2 FTW is 4 blocks, or 256 KiB.

## API

The API provides read and write access to an unformatted block storage.

### Storage region

By default SMMSTOREv2 will operate on a separate FMAP region called
`SMMSTORE`. The default generated FMAP will include such a region. On
systems with a locked FMAP, e.g. in an existing vboot setup with a
locked RO region, the option exists to add a cbfsfile called `smm_store`
in the `RW_LEGACY` (if CHROMEOS) or in the `COREBOOT` FMAP regions. It
is recommended for new builds using a handcrafted FMD that intend to
make use of SMMSTORE to include a sufficiently large `SMMSTORE` FMAP
region. It is mandatory to align the `SMMSTORE` region to 64KiB for
compatibility with the largest flash erase operation.

When a default generated FMAP is used, the size of the FMAP region is
equal to `CONFIG_SMMSTORE_SIZE`. UEFI payloads expect at least 64 KiB.
To support a fault tolerant write mechanism, at least a multiple of
this size is recommended.

### Communication buffer

To prevent malicious ring0 code to access arbitrary memory locations,
SMMSTOREv2 uses a communication buffer in CBMEM/HOB for all transfers.
This buffer has to be at least 64 KiB in size and must be installed
before calling any of the SMMSTORE read or write operations. Usually,
coreboot will install this buffer to transfer data between ring0 and
the [SMM] handler.

In order to get the communication buffer address, the payload or OS
has to read the coreboot table with tag `0x0039`, containing:

```C
struct lb_smmstorev2 {
	uint32_t tag;
	uint32_t size;
	uint32_t num_blocks;	/* Number of writeable blocks in SMM */
	uint32_t block_size;	/* Size of a block in byte. Default: 64 KiB */
	uint32_t mmap_addr;	/* MMIO address of the store for read only access */
	uint32_t com_buffer;	/* Physical address of the communication buffer */
	uint32_t com_buffer_size;	/* Size of the communication buffer in byte */
	uint8_t apm_cmd;	/* The command byte to write to the APM I/O port */
	uint8_t unused[3];	/* Set to zero */
};
```

The absence of this coreboot table entry indicates that there's no
SMMSTOREv2 support.

### Blocks

The SMMSTOREv2 splits the SMMSTORE FMAP partition into smaller chunks
called *blocks*. Every block is at least the size of 64KiB to support
arbitrary NOR flash erase ops. A payload or OS must make no further
assumptions about the block or communication buffer size.

### Generating the SMI

SMMSTOREv2 is called via an SMI, which is generated via a write to the
IO port defined in the smi_cmd entry of the FADT ACPI table. `%al`
contains `APM_CNT_SMMSTORE=0xed` and is written to the smi_cmd IO
port. `%ah` contains the SMMSTOREv2 command. `%ebx` contains the
parameter buffer to the SMMSTOREv2 command.

### Return values

If a command succeeds, SMMSTOREv2 will return with
`SMMSTORE_RET_SUCCESS=0` in `%eax`. On failure SMMSTORE will return
`SMMSTORE_RET_FAILURE=1`. For unsupported SMMSTORE commands
`SMMSTORE_REG_UNSUPPORTED=2` is returned.

**NOTE 1**: The caller **must** check the return value and should make
no assumption on the returned data if `%eax` does not contain
`SMMSTORE_RET_SUCCESS`.

**NOTE 2**: If the SMI returns without changing `%ax`, it can be assumed
that the SMMSTOREv2 feature is not installed.

### Calling arguments

SMMSTOREv2 supports 3 subcommands that are passed via `%ah`, the
additional calling arguments are passed via `%ebx`.

**NOTE**: The size of the struct entries are in the native word size of
smihandler. This means 32 bits in almost all cases.

#### - SMMSTORE_CMD_INIT = 4

This installs the communication buffer to use and thus enables the
SMMSTORE handler. This command can only be executed once and is done
by the firmware. Calling this function at runtime has no effect.

The additional parameter buffer `%ebx` contains a pointer to the
following struct:

```C
struct smmstore_params_init {
	uint32_t com_buffer;
	uint32_t com_buffer_size;
} __packed;
```

INPUT:
- `com_buffer`: Physical address of the communication buffer (CBMEM)
- `com_buffer_size`: Size in bytes of the communication buffer

#### - SMMSTORE_CMD_RAW_READ = 5

SMMSTOREv2 allows reading arbitrary data. It is up to the caller to
initialize the store with meaningful data before using it.

The additional parameter buffer `%ebx` contains a pointer to the
following struct:

```C
struct smmstore_params_raw_read {
	uint32_t bufsize;
	uint32_t bufoffset;
	uint32_t block_id;
} __packed;
```

INPUT:
- `bufsize`: Size of data to read within the communication buffer
- `bufoffset`: Offset within the communication buffer
- `block_id`: Block to read from

#### - SMMSTORE_CMD_RAW_WRITE = 6

SMMSTOREv2 allows writing arbitrary data. It is up to the caller to
erase a block before writing it.

The additional parameter buffer `%ebx` contains a pointer to
the following struct:

```C
struct smmstore_params_raw_write {
        uint32_t bufsize;
        uint32_t bufoffset;
        uint32_t block_id;
} __packed;
```

INPUT:
- `bufsize`: Size of data to write within the communication buffer
- `bufoffset`: Offset within the communication buffer
- `block_id`: Block to write to

#### - SMMSTORE_CMD_RAW_CLEAR = 7

SMMSTOREv2 allows clearing blocks. A cleared block will read as `0xff`.
By providing multiple blocks the caller can implement a fault tolerant
write mechanism. It is up to the caller to clear blocks before writing
to them.


```C
struct smmstore_params_raw_clear {
	uint32_t block_id;
} __packed;
```

INPUT:
- `block_id`: Block to erase

#### Security

Pointers provided by the payload or OS are checked to not overlap with
SMM. This protects the SMM handler from being compromised.

As all information is exchanged using the communication buffer and
coreboot tables, there's no risk that a malicious application capable
of issuing SMIs could extract arbitrary data or modify the currently
running kernel.

## External links

* [A Tour Beyond BIOS Implementing UEFI Authenticated Variables in SMM with EDKI](https://software.intel.com/sites/default/files/managed/cf/ea/a_tour_beyond_bios_implementing_uefi_authenticated_variables_in_smm_with_edkii.pdf)
Note that this differs significantly from coreboot's implementation.

[SMM]: ../security/smm.md
