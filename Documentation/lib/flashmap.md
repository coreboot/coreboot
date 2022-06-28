# Flashmap and Flashmap Descriptor in coreboot

## Flashmap

[Flashmap](https://code.google.com/p/flashmap) (FMAP) is a binary format to
describe partitions in a flash chip. It was added to coreboot to support the
requirements of ChromiumOS firmware but then was also used in other scenarios
where precise placement of data in flash was necessary, or for data that is
written to at runtime, as CBFS is considered too fragile for such situations.
The Flashmap implementation inside coreboot is the de facto standard today.

Flashmap partitions the image into clearly delimited sections and some of those
sections may be CBFSes that can hold arbitrary-length files (at least one, the
default CBFS, called `COREBOOT`). General guidance is that everything with
strict layout requirements (e.g. must be aligned to erase blocks or
something else) should have its own Flashmap section, and everything else should
normally go into CBFS.

The Flashmap itself starts with a header `struct fmap` and followed by a list of
section descriptions in `struct fmap_area`. All fields in those structures are
in little endian format.

### Header
The header `struct fmap` has following fields:
* `signature`: 8 characters as `"__FMAP__"`.
* `ver_major`: one byte for major version (currently only 1).
* `ver_minor`: one byte for minor version (current value is 1).
* `base`: 64 bit integer for the address of the firmware binary.
* `size`: 32 bit integer for the size of firmware binary in bytes.
* `name`: 32 characters for the name of the firmware binary.
* `nareas`: 16 bit integer for the number of area definitions (i.e., how many
  sections are in this firmware image) following the header.

### Area Definition
The section is defined by `struct fmap_area` with following fields:
* `offset`: 32 bit integer for where the area starts (relative to `base` in
  header).
* `size`: 32 bit integer for the size of area in bytes.
* `name`: 32 characters for a descriptive name of this area. Should be unique to
  all sections inside same Flashmap.
* `flags`: 16 bit integer for attributes of this area (see below).

### Area Flags
Currently the defined values for `flags` in `struct fmap_area` are:
* `FMAP_AREA_PRESERVE`: suggesting the section should be preserved when
  updating firmware, usually for product data like serial number, MAC address,
  or calibration and cache data.
* `FMAP_AREA_STATIC`: Not really used today.
* `FMAP_AREA_COMPRESSED`: Not really used today.
* `FMAP_AREA_RO`: Not really used today.

### FMAP section
The whole Flashmap (`struct fmap` and list of `struct fmap_area`) should be
stored in a standalone section named as `FMAP` (which should be also described
by the Flashmap itself in `struct fmap_area`). There's no restriction for where
it should be located (or how large), but usually we need to do a linear or
binary search on whole firmware binary image to find Flashmap so a properly
aligned address would be better.

### COREBOOT section
coreboot firmware images (`coreboot.rom`) should have at least one Flashmap
section that is reserved for CBFS. Usually it is named as `COREBOOT`.

## Flashmap Descriptor

Since coreboot is starting to use a "partition" of Flashmap to describe the
flash chip layout (both at runtime and when flashing a new image onto a
chip), the project needs a reasonably expressive plain text format for
representing such sections in the source tree.

Flashmap Descriptor (FMD) is a [language and
compiler](https://chromium-review.googlesource.com/#/c/255031) inside coreboot
utility folder that can be used to generate final firmware images (i.e.
`coreboot.rom`) formatted by Flashmap.

The FMD implementation is in coreboot `utils/cbfstool` folder. Here's an
informal language description:

```
# <line comment>
<image name>[@<memory-mapped address>] <image size> {
    <section name>[(flags)][@<offset from start of image>] [<section size>] [{
        <subsection name>[@<offset from start of parent section>] [<subsection size>] [{
            # Sections can be nested as deeply as desired
            <subsubsection name>[(flags)][@...] [...] [{...}]
        }]
        [<subsection name>[(flags)][@...] [...] [{...}]]
        # There can be many subsections at each level of nesting: they will be inserted
        # sequentially, and although gaps are allowed, any provided offsets are always
        # relative to the closest parent node's and must be strictly increasing with neither
        # overlapping nor degenerate-size sections.
    }]
}
```

Note that the above example contains a few symbols that are actually meta
syntax, and therefore have neither meaning nor place in a real file. The `<.*>`s
indicate placeholders for parameters:

* The names are strings, which are provided as single-word (no white space)
  groups of syntactically unimportant symbols (i.e. every thing except `@`, `{`,
  and `}`): they are not surrounded by quotes or any other form of delimiter.
* The other fields are non-negative integers, which may be given as decimal or
  hexadecimal; in either case, a `K`, `M`, or `G` may be appended (without
  intermediate white space) as a multiplier.
* Comments consist of anything one manages to enter, provided it doesn't start a
  new line.

The `[.*]`s indicate that a portion of the file could be omitted altogether:

* Just because something is noted as optional doesn't mean it is in every case:
  the answer might actually depend on which other information is---or
  isn't---provided.
* The "flag" specifies the attribute or type for given section. The most
  important supported flag is "CBFS", which indicates the section will contain
  a CBFS structure.
* In particular, it is only legal to place a (CBFS) flag on a leaf section; that
  is, choosing to add child sections excludes the possibility of putting a CBFS
  in their parent. Such flags are only used to decide where CBFS empty file
  headers should be created, and do not result in the storage of any additional
  metadata in the resulting FMAP section.

Additionally, it's important to note these properties of the overall file and
its values:

* Other than within would-be strings and numbers, white space is ignored. It
  goes without saying that such power comes with responsibility, which is why
  this sentence is here.
* Although the `section name` must be globally unique, one of them may (but is
  not required to) match the image name.
* It is a syntax error to supply a number (besides 0) that begins with the
  character `0`, as there is no intention of adding octals to the mix.
* The image's memory address should be present on (and only on) layouts for
  memory-mapped chips.
* Although it may be evident from above, all `section` offsets are relative only
  to the immediate parent. There is no way to include an absolute offset (i.e.
  from the beginning of flash), which means that it is "safe" to reorder the
  sections within a particular level of nesting, as long as the change doesn't
  cause their positions and sizes to necessitate overlap or zero sizes.
* A `section` with omitted offset is assumed to start at as low a position as
  possible (with no consideration of alignment) and one with omitted size is
  assumed to fill the remaining space until the next sibling or before the end
  of its parent.
* It's fine to omit any `section`'s offset, size, or both, provided its position
  and size are still unambiguous in the context of its *sibling* sections and
  its parent's *size*. In particular, knowledge of one .*section 's children or
  the `section`s' common parent's siblings will not be used for this purpose.
* Although `section`s are not required to have children, the flash chip as a
  whole must have at least one.
* Though the braces after `section`s may be omitted for those that have no
  children, if they are present, they must contain at least one child.

To see the formal description of the language, please refer to the Lex and Yacc
files: `fmd_scanner.l` and `fmd_scanner.y`.
