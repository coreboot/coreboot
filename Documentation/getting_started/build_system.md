# The coreboot build system
(this document is still incomplete and will be filled in over time)

## General operation
The coreboot build system is based on GNU make but extends it significantly
to the point of providing its own custom language.
The overhead of learning this new syntax is (hopefully) offset by its lower
complexity.

The build system is defined in the toplevel `Makefile` and `toolchain.mk`
and is supposed to be generic (and is in fact used with a number of other
projects).  Project specific configuration should reside in files called
`Makefile.mk`.

In general, the build system provides a number of "classes" that describe
various parts of the build. These cover the various build targets in coreboot
such as the stages, subdirectories with more source code, and the general
addition of files.

Each class has a name (eg. `romstage`, `subdirs`, `cbfs-files`) and is used
by filling in a variable of that name followed by `-y` (eg. `romstage-y`,
`subdirs-y`, `cbfs-files-y`).
The `-y` suffix allows a simple interaction with our Kconfig build
configuration system: Kconfig options are available as variables starting
with a `CONFIG_` prefix and boolean options contain `y`, `n` or are empty.

This allows `class-$(CONFIG_FOO) += bar` to conditionally add `bar` to
`class` depending on the choice for `FOO`.

## classes
Classes can be defined as required. `subdirs` is handled internally since
it's parsed per subdirectory to add further directories to the rule set.

TODO: explain how to create new classes and how to evaluate them.

### subdirs
`subdirs` contains subdirectories (relative to the current directory) that
should also be handled by the build system. The build system expects these
directories to contain a file called `Makefile.mk`.

Subdirectories are not read at the point where the `subdirs` statement
resides but later, after the current directory is handled (and potentially
others, too).

### cbfs-files
This class is used to add files to the final CBFS image. Since several more
options need to be maintained than can comfortably fit in that single
variable, additional variables are used.

`cbfs-files-y` contains the file name used in the CBFS image (called `foo`
here). Additional options are added in `foo-$(option)` variables. The
supported options are:

*  `file`: The on-disk file to add as `foo` (required)
*  `type`: The file type. Can be `raw`, `stage`, `payload`, and `flat-binary`
   (required)
*  `compression`: Can be `none` or `lzma` (default: none)
*  `position`: An absolute position constraint for the placement of the file
   (default: none)
*  `align`: Minimum alignment for the file (default: none)
*  `options`: Additional cbfstool options (default: none)

`position` and `align` are mutually exclusive.

### Adding Makefile fragments

You can use the `add_intermediate` helper to add new post-processing steps for
the final `coreboot.rom` image. For example you can add new files to CBFS by
adding something like this to `site-local/Makefile.mk`

```
$(call add_intermediate, add_mrc_data)
	$(CBFSTOOL) $< write -r RW_MRC_CACHE -f site-local/my-mrc-recording.bin
```

Note that the second line must start with a tab, not spaces.

See also <project:../tutorial/managing_local_additions.md>.

#### FMAP region support
With the addition of FMAP flash partitioning support to coreboot, there was a
need to extend the specification of files to provide more precise control
which regions should contain which files, and even change some flags based on
the region.

Since FMAP policies depend on features using FMAP, that's kept separate from
the cbfs-files class.

The `position` and `align` options for file `foo` can be overwritten for a
region `REGION` using `foo-REGION-position` and `foo-REGION-align`.

The regions that each file should end in can be defined by overriding a
function called `regions-for-file` that's called as
`$(call regions-for-file,$(filename))` and should return a comma-separated
list of regions, such as `REGION1,REGION2,REGION3`.

The default implementation just returns `COREBOOT` (the default region) for
all files.

vboot provides its own implementation of `regions-for-file` that can be used
as reference in `src/vboot/Makefile.mk`.
