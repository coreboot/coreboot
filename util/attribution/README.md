# attribution

`util/attribution` generates a human-readable **third-party attribution /
notices** file that credits the projects whose software ends up in a coreboot
image - compiled into it, borrowed from, or stitched in as a binary - and
stores it in CBFS as a raw file named `attribution`.

Unlike coreboot's [SBOM](../../sbom/sbom.md) feature (a machine-readable uSWID
document produced with the `goswid` tool), this is a plain text notices file
intended to be read by humans. The two features are independent and can be used
together or separately.

## What it does

The list of credited projects is derived from the **enabled configuration
options**, so it reflects what is actually in *this* build. Most entries are
gated on the `CONFIG_*` symbol that also gates the project's sources. The
compression decoders (LZMA, LZ4, zstd) are the exception: rather than mirror
the several Kconfig options that can select each one, the Makefile scans the
aggregated per-stage source lists for the decoder's source file
(`lzmadecode.c` / `lz4_wrapper.c` / `zstd_decompress.c`) and lists a decoder
only when its code is actually linked in. xxHash, by contrast, is always
compiled in and is always listed. Coverage spans in-tree third-party source
(LZMA, LZ4, zstd, xxHash, UBSan
runtime, Wuffs, x86emu, Ada libgfxinit/libhwbase, fonts, ...), the selected
payload and secondary payloads, and vendor firmware blobs (Intel FSP/ME/GbE/ACM,
AMD FSP/PSP, microcode, VGA option ROM, ARM Trusted Firmware, OpenSBI).

Each entry is a short summary block: project name, license (an SPDX identifier
where one applies, or free text for proprietary vendor blobs), upstream URL and
a copyright/notes line. Full license texts are not embedded; they live in the
coreboot source tree under `LICENSES/`.

Entries are grouped into three sections, controlled by the manifest `kind`
field:

* **software compiled into the image** (`kind` empty) - libraries/components
  built from the coreboot tree (coreboot, LZMA, LZ4, zstd, xxHash, vboot,
  EDK II, openSIL, AGESA, ...).
* **binaries added to the image** (`kind = binary`) - standalone binaries the
  build stitches in but does *not* compile from coreboot source: the payload
  (SeaBIOS, GRUB2, depthcharge, ...), vendor firmware blobs (Intel FSP/ME,
  AMD FSP/PSP, microcode) and separately-built programs (ARM Trusted Firmware,
  OpenSBI).
* **code used from other projects** (`kind = used`) - individual files coreboot
  borrows from a larger project it does not otherwise bundle (Linux kernel,
  U-Boot, depthcharge, Etherboot, ChromiumOS EC, the Sortix UBSan runtime).

## Enabling it

Enable `CONFIG_INCLUDE_ATTRIBUTION_FILE` (menuconfig: *General setup -> Include a
third-party attribution/notices file into the ROM image*). This adds a couple of
KB to the image. The file is generated at build time and added to the `COREBOOT`
CBFS region.

## Reading it back

Extract the file from a built image with `cbfstool`:

```console
$ cbfstool build/coreboot.rom extract -n attribution -f attribution.txt
$ cat attribution.txt
coreboot third-party attribution / notices

This firmware image includes software from the following projects:

* coreboot
    License:  GPL-2.0-only
    Upstream: https://coreboot.org
    ...
```

You can also see the entry in the CBFS listing:

```console
$ cbfstool build/coreboot.rom print | grep attribution
attribution      0x40d80    raw    558   LZMA (1044 decompressed)
```

## Files

* `gen_attribution.sh` - POSIX-sh renderer. Invoked as
  `gen_attribution.sh <manifest> <key>...`; it emits a block for each
  enabled key, in manifest order, to stdout.
* `attribution.list` - the curated data manifest. One project per line,
  pipe-delimited: `key | Project name | SPDX-id(s) | URL | copyright | kind`.
  The optional `kind` field (`binary`, `used`, or empty) selects the section
  the entry renders under. This is the single place to edit project metadata.
* `Makefile.mk` - selects which keys are included from the enabled `CONFIG_*`
  options, runs the renderer, and registers the result in CBFS. Included in the
  build via `subdirs-$(CONFIG_INCLUDE_ATTRIBUTION_FILE) += util/attribution`.

## Adding a project

1. Add a row to `attribution.list` with a new `key`, setting the `kind` field
   (`binary` for a stitched-in payload/blob, `used` for a borrowed file, or
   empty for source compiled into coreboot).
2. In `Makefile.mk`, add `attribution-keys-$(CONFIG_<symbol>) += <key>` (or an
   `ifneq (...)` block for an OR of several symbols) using the same `CONFIG_*`
   condition that gates the project's sources.

The renderer silently ignores keys with no matching manifest row, so the two
files can be updated independently.
