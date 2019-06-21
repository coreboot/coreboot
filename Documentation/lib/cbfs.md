# coreboot Filesystem (CBFS)

The coreboot filesystem (CBFS) is a scheme for managing independent chunks
of data in a system ROM. Though not a true filesystem, the style and
concepts are similar. It is a flat, append-oriented archive designed to
be walked at any point in the boot process.

This document is an overview of the on-flash format, how it is consumed
at runtime, and of how `cbfstool` manipulates it.
The authoritative description of the binary format is
`src/commonlib/bsd/include/commonlib/bsd/cbfs_serialized.h`.

## Image layout

Way back at the beginning of coreboot, ROM images were only defined by the CBFS.
As images started to include additional binaries, the FMAP was adopted. FMAP
holds information about the entire flash and CBFS holds the coreboot binaries.
CBFS lives *inside* a FMAP region. A minimal image contains an `FMAP` region
and a `COREBOOT` region holding the primary CBFS.
See [Flashmap and Flashmap Descriptor](flashmap.md) for the region layout and
the descriptor format.

```text
/------------------------\ <- start of flash
| ...                    |
|------------------------|
| FMAP                   |   describes the region layout
|------------------------|
| COREBOOT               | --\
|  /-------------------\ |   |
|  | fallback/payload  | |   |
|  |-------------------| |   |
|  | fallback/ramstage | |   |
|  |-------------------| |   |-- a CBFS instance
|  | fallback/romstage | |   |
|  |-------------------| |   |
|  | ...               | |   |
|  |-------------------| |   |
|  | bootblock         | |   |
|  \-------------------/ |   |
|                        | --/
\------------------------/ <- end of flash (0xFFFFFFFF on x86)
```

The bootblock is a mandatory part of every image, but where it lives depends on
the architecture. On x86 it is a CBFS file of type `bootblock`, placed at the
highest flash address so that it covers the reset vector at `0xfffffff0`, which
is why the diagram above draws it at the bottom. On other architectures it
normally occupies its own `BOOTBLOCK` FMAP region and is not part of any CBFS.
`BOOTBLOCK_IN_CBFS` selects between the two and defaults to the CBFS form on
x86.

The above example shows a single `COREBOOT` CBFS instance, but a flash may hold
several CBFS regions to support A/B or RO/RW update schemes.
For example, Intel Top Swap redundancy pairs `COREBOOT`
with a `COREBOOT_B` update slot. The instance the platform boots from is
decided by a platform-specific mechanism, not by CBFS.

## File format

Every file starts with a metadata block, followed by the file data:

```text
/------------\  <- start of file, 64 byte aligned
| header     |
|------------|  <- 'attributes_offset'
| attributes |
|------------|  <- 'offset'
| data       |
| ...        |
\------------/  <- start + 'offset' + 'len'
```

The total size of the metadata block, that is header plus attributes,
is capped by `CBFS_METADATA_MAX_SIZE` (256 bytes).

**All CBFS metadata is stored big-endian on flash, regardless of the host or
target architecture.** Fields must be byte-swapped as they are read. Only the
metadata is byte-swapped, file data is opaque.

The header is:

```c
struct cbfs_file {
	char magic[8];
	uint32_t len;
	uint32_t type;
	uint32_t attributes_offset;
	uint32_t offset;
	char filename[];
} __packed;
```

`magic` identifies the header. It is always the string `LARCHIVE`.

`len` is the length of the file data, excluding all metadata.

`type` describes the content of the data, see [File types](#file-types).

`attributes_offset` is the offset of the attributes from the start of the
header, or 0 if the file has no attributes.

`offset` is the offset of the file data from the start of the header
(also the total size of the metadata).

`filename` is an inline NUL-terminated string.

Files are aligned to `CBFS_ALIGNMENT`, which is fixed at 64 bytes.

### File attributes

CBFS header fields are the same for every file. Anything a particular file needs
beyond them is carried as an attribute, an optional struct in the metadata.
Compression, hashes, placement constraints and a stage's load
address are all attributes.

Attributes are packed back to back between `attributes_offset` (the end of the
CBFS header) and `offset` (the data). Each begins with a common tag and length:

```c
struct cbfs_file_attribute {
	uint32_t tag;
	/* len covers the whole structure, incl. tag and len */
	uint32_t len;
	uint8_t data[];
} __packed;
```

Every attribute size must be a multiple of `CBFS_ATTRIBUTE_ALIGN` (4). A parser
that does not recognize a tag skips `len` bytes and continues, so new attributes
can be added without breaking existing readers.

| Tag | Structure | Purpose |
| --- | --- | --- |
| `COMPRESSION` | `cbfs_file_attr_compression` | Algorithm and decompressed size of the file data |
| `HASH` | `cbfs_file_attr_hash` | `vb2_hash` over the file data, used by CBFS verification |
| `STAGEHEADER` | `cbfs_file_attr_stageheader` | Load address, entry point and memory size of a stage |
| `POSITION` | `cbfs_file_attr_position` | Pins the file to a fixed offset |
| `ALIGNMENT` | `cbfs_file_attr_align` | Forces a stronger alignment than 64 bytes |
| `IBB` | -- | Marks the file as part of the Initial Boot Block |
| `PADDING` | -- | Reserves empty space, used to satisfy the above constraints |

Compression is a property of the file, not of a particular file type, and covers
the whole data area. The supported algorithms are none (0), LZMA (1), LZ4 (2)
and Zstandard (3). Which of them a given stage can use depends on what the
decompressor in the preceding coreboot stage has available.

### Lookup

CBFS has no index. Finding a file means walking the region from the start, as
implemented by `cbfs_walk()` in `src/commonlib/bsd/cbfs_private.c`:

1. Start at offset 0 of the CBFS region.
2. Read 8 bytes and compare against `LARCHIVE`. If they do not match, advance by
   `CBFS_ALIGNMENT` (64) and retry. Running past the end of the region means the
   file is not present.
3. On a match, read the rest of the metadata and range-check `len`,
   `attributes_offset` and `offset` against the region size. A header that fails
   validation is skipped.
4. Compare `filename` against the name being looked up. On a match the data
   begins at `offset` bytes into the file.
5. Otherwise continue the search at `ALIGN_UP(file_start + offset + len, 64)`.

Deleted files are not removed. Their type is set to `CBFS_TYPE_DELETED` (0) or
`CBFS_TYPE_NULL` (0xffffffff) and the space they occupy stays in the walk as a
hole. `cbfstool compact` merges those holes back together.

Because rescanning flash in every stage is expensive, coreboot builds a metadata
cache (`src/commonlib/bsd/cbfs_mcache.c`) during the first walk and passes it to
later stages through CBMEM.

## File types

`type` identifies what the data is, so that coreboot and host tools know how to
handle it. The full list is `enum cbfs_type` in `cbfs_serialized.h`. The types
seen in a typical image are:

| Name | Value | Content |
| --- | --- | --- |
| `BOOTBLOCK` | 0x01 | The bootblock |
| `CBFSHEADER` | 0x02 | Legacy master header |
| `STAGE` | 0x11 | A coreboot stage |
| `SELF` | 0x20 | A [SELF] payload |
| `FIT_PAYLOAD` | 0x21 | A [FIT] payload |
| `OPTIONROM` | 0x30 | A PCI option ROM |
| `RAW` | 0x50 | Uninterpreted data |
| `MICROCODE` | 0x53 | CPU microcode |
| `INTEL_FIT` | 0x54 | Intel Firmware Interface Table |
| `FSP` | 0x60 | Intel Firmware Support Package |
| `AMDFW` | 0x80 | AMD firmware container |
| `CMOS_DEFAULT` | 0xaa | Default CMOS contents |
| `CMOS_LAYOUT` | 0x1aa | CMOS layout description |

Note that `FIT_PAYLOAD` (a flattened image tree) and `INTEL_FIT` (a table of
pointers consumed by the Intel CPU microcode) are unrelated despite the shared
acronym.

### Stages

Stages are the pieces of coreboot itself: romstage, ramstage, and on some
platforms a separate verstage. They are loaded and executed by the preceding
stage.

A stage is a flat binary plus a `STAGEHEADER` attribute:

```c
struct cbfs_file_attr_stageheader {
	uint32_t tag;
	uint32_t len;
	uint64_t loadaddr;	/* Memory address to load the code to. */
	uint32_t entry_offset;	/* Offset of entry point from loadaddr. */
	uint32_t memlen;	/* Total length (including BSS) in memory. */
} __packed;
```

`entry_offset` is relative to `loadaddr`, not an absolute address. `memlen`
covers BSS, so it is generally larger than the decompressed data.

To load a stage, coreboot decompresses the data to `loadaddr` according to the
`COMPRESSION` attribute, zeroes the remainder up to `memlen`, and jumps to
`loadaddr + entry_offset`. An uncompressed stage that is already at `loadaddr`
on memory-mapped flash is executed in place, without being copied.

Type 0x10 is `CBFS_TYPE_LEGACY_STAGE`, an older format that carried this
information in a header prepended to the data instead of in an attribute.

### Payloads

A payload is the program coreboot hands control to at the end of the boot
process. The native format is SimpleELF (SELF). Statically linked [ELF] binaries
are converted to SELF by `cbfstool` at build time, which splits the payload into
segments, each with its own loading address and compression (LZMA, LZ4, ZSTD or
none).

A SELF payload is a sequence of segment headers followed by the segment data:

```c
struct cbfs_payload_segment {
	uint32_t type;
	uint32_t compression;
	uint32_t offset;
	uint64_t load_addr;
	uint32_t len;
	uint32_t mem_len;
} __packed;
```

`type` is one of:

| Name | Value | Meaning |
| --- | --- | --- |
| `PAYLOAD_SEGMENT_CODE` | 0x434F4445 | Executable code |
| `PAYLOAD_SEGMENT_DATA` | 0x44415441 | Data |
| `PAYLOAD_SEGMENT_BSS` | 0x42535320 | Memory to be zeroed |
| `PAYLOAD_SEGMENT_ENTRY` | 0x454E5452 | Entry point of the payload |

`compression` applies to this segment only. Unlike other file types, SELF
payloads are compressed per segment rather than through the file-level
`COMPRESSION` attribute.

`offset` locates the segment data relative to the start of the file data,
`load_addr` is where it goes in memory, `len` is its size in the file and
`mem_len` its size in memory.

A [FIT] payload (type 0x21) is supported on some architectures. It does not
support whole-file compression, individual images inside the FIT carry their own.

The SELF loader implementation is in `src/lib/selfboot.c`. It decompresses
each segment and places it in memory. Every segment is checked against the
bootmem map before anything is loaded, so it must target usable RAM; a
payload overlapping the loading stage (*RAMSTAGE*) or any reserved region
aborts the boot. SELF payloads are **never** relocatable and are always
placed at the address they specify; if that address is unavailable, the
system won't boot.

The SELF loader is not limited to payloads. It is also used to load arm64
*BL31*, RISC-V *OpenSBI* and vendor firmware blobs.

#### Calling conventions

The SELF payload is called with a pointer to the coreboot tables as first
argument.

**Note:** One exception is made on [RISC-V], which prepends the *HARTID* and
a pointer to the *FDT*, so the coreboot tables arrive as third argument.

### Raw files and everything else

Most other types have no additional header. The data is exactly what was passed
to `cbfstool add`, after optional compression. `RAW` is the catch-all for data a
driver or payload reads by name, for example an SPD blob or a splash image.

Option ROMs (0x30) similarly carry no extra header, just the ROM image.

## Verification

When `CBFS_VERIFICATION` is enabled, every file carries a `HASH` attribute
covering its data, and coreboot checks it before using the file. The file
metadata itself, including those hashes, is covered by a single metadata hash
computed over the whole CBFS walk.

That metadata hash is stored in a `struct metadata_hash_anchor` embedded in the
uncompressed bootblock (`src/commonlib/bsd/include/commonlib/bsd/metadata_hash.h`),
which also holds a hash of the FMAP. The bootblock is therefore the root of
trust. Verifying it covers the FMAP and the whole read-only CBFS.

Updatable CBFS instances cannot be covered by an anchor baked into the read-only
bootblock, so their metadata hash is supplied by whatever verified the instance.
See [verified boot](../security/vboot/index.md) for how that works in practice.

## Legacy master header

Pre-FMAP x86 images described their CBFS with a master header rather than a
flash map. It records the image size, the offset of the first file and the file
alignment, and is stored as a CBFS file of type `CBFSHEADER`. The last four
bytes of the image hold its offset relative to the end of the image as a signed
32-bit integer, so early code can locate it without parsing anything.

New designs should not use this. It cannot describe more than one CBFS, and
several `cbfstool` operations including `expand` and `truncate` refuse to work
on such images. See `struct cbfs_header` in `cbfs_serialized.h` for the layout.

## cbfstool

`cbfstool` is the host utility for building and inspecting images. It is built
as part of coreboot into `build/cbfstool`, and the build system uses it to
assemble `coreboot.rom`.

Unless `-r`/`--fmap-regions` says otherwise, commands operate on the `COREBOOT`
region.

| Command | Purpose |
| --- | --- |
| `create -M flashmap` | Create a new image from an FMAP descriptor |
| `add` | Add a file, with `-t` type and optional `-c` compression |
| `add-payload` | Convert an ELF to SELF and add it |
| `add-stage` | Convert an ELF to a stage and add it |
| `add-flat-binary` | Add a flat binary with an explicit load address and entry point |
| `add-int` | Add a raw 64-bit integer |
| `remove` | Mark a file deleted |
| `print` | List the contents of a region |
| `extract` | Write a file out, `-U` to skip decompression |
| `layout` | List the FMAP regions of the image |
| `read` / `write` | Copy a whole region out of or into the image |
| `copy` | Duplicate a CBFS instance into another region |
| `compact` | Merge the holes left by `remove` |
| `expand` / `truncate` | Grow a CBFS to fill its region, or shrink it to its contents |

A typical inspection looks like:

```shell
build/cbfstool build/coreboot.rom layout
build/cbfstool build/coreboot.rom print -r COREBOOT
```

Run `cbfstool -h` for the full option list, including the supported
architectures and file type names.

### Adding files (at build time)

Calling `cbfstool add` by hand only modifies an image that has already been
built. To have a file included in every build, add it to the `cbfs-files-y`
make class, which the build system turns into the corresponding
`cbfstool` invocation. The class and its `file`, `type`, `compression`,
`position` and `align` options are described in
[the coreboot build system](../getting_started/build_system.md).

[FIT]: payloads/fit.md
[SELF]: #payloads
[ELF]: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
[RISC-V]: ../arch/riscv/index.md
