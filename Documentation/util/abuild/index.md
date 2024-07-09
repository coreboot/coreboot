abuild
======

This utility is a great tool to check whether your coreboot tree
compiles for one or all targets. It compiles the 'default' build for a
mainboard. This is roughly equivalent to removing the .config file,
running `make menuconfig`, selecting the manufacturer and mainboard,
then saving the config without making any other changes.

It is run on all patches submitted via gerrit as part of the process.
Before submitting a patch, it is a very good idea to run abuild first
to make sure your patch compiles cleanly for all.

Note that abuild is a tool to do a simple build test, and binaries it
produces may well not boot if flashed to a system.

## Basic usage

abuild needs to be run from the coreboot directory. If you cd into the
coreboot/util/abuild directory and try to run it from there, it will
not run correctly.

If you invoke abuild with no parameters, it will build all boards
automatically.

You can also specify a single board to build with the -t option. For
example, to build the Lenovo X230 target, run:

```bash
$ util/abuild/abuild -t lenovo/x230
```

## Where builds and logs are stored

The resulting images and logs are stored in directory coreboot-builds/
under your current directory. This can be overridden with --outdir:

```bash
$ util/abuild/abuild --outdir /mnt/portable/coreboot-builds
```

This is useful if you want to divert the build to an external hard
drive, e.g. to keep the solid-state drive holding the coreboot tree
young.

(We will still refer to this directory as "coreboot-builds" below.)

After running the X230 build above, the build log will be in
coreboot-builds/LENOVO_X230/make.log.

For an overview of what passed and what failed, look at
coreboot-builds/passing_boards and coreboot-builds/failing_boards.
**These logs are overwritten with each abuild run.** Save them elsewhere
if you feel a need to reference the results later.

## Payloads

You can also specify a payload directory with -p:

```bash
mkdir payloads
cp /somewhere/filo.elf payloads
```

Then add a file payloads/payload.sh which prints the name of the
payload to use (and takes the mainboard as a parameter) such as:

```bash
echo "`dirname $0`/build/filo.elf"
```

Then you can build an image with payload by specifying:

```bash
util/abuild/abuild -t lenovo/x230 -p ./payloads
```

You can also tell abuild not to use a payload:

```bash
util/abuild/abuild -t lenovo/x230 -p none
```

## Build non-default configurations

Sometimes you do need to build test a custom, non-default configuration.
This can be accomplished by placing a config file in configs/.

First, clean your slate with `make distclean` or `rm .config`.

Then run `make menuconfig`, select the manufacturer and mainboard, and
configure the options you need to test building for.

Then save a minimal config file omitting options that did not change
from default:

```bash
make savedefconfig
```

This file is saved as `defconfig` and can be edited further.

Now this file can be saved in configs/ which will form the basis of a
custom configuration included in an abuild. However, it needs to be
named in a specific way for abuild to pick it up:

```
config.<board>_<suffix>
```

<board> is effectively the BOARD\_xxx Kconfig option without "BOARD\_".
<suffix> is a free form description of the configuration being built.

For example, a config for ASUS P8Z77-M PRO that tests building with MRC
raminit code (as opposed to the default native raminit) would be named
`config.asus_p8z77_m_pro_mrc_bin` and contains:

```
CONFIG_VENDOR_ASUS=y
CONFIG_BOARD_ASUS_P8Z77_M_PRO=y
# CONFIG_USE_NATIVE_RAMINIT is not set
CONFIG_CPU_MICROCODE_CBFS_NONE=y
# CONFIG_BOOTBLOCK_CONSOLE is not set
# CONFIG_POSTCAR_CONSOLE is not set
```

For what we are trying to do, not setting USE_NATIVE_RAMINIT is the
important part. The other three optional changes are meant to speed
things up. All these options can be selected during `make menuconfig`.

Path to MRC binary blob remains default and thus not included here.

Custom configurations can also be put in a file and applied to an entire
abuild run using -K. Assume for example you are not interested in
the postcar stage at all and just want it to shut up, you can create
a file named `myconfig` with this line:

```
# CONFIG_POSTCAR_CONSOLE is not set
```

and run `abuild -K myconfig` to build everything with a silent postcar
stage.

## Selectively build certain targets only (also config file naming caveats)

The P8Z77-M PRO example above would fail for P8Z77-M, because the
config file name is ambiguous. `abuild` would pick up this config when
building for P8Z77-M, but fails when it sees that this config isn't
meant for P8Z77-M (but for P8Z77-M PRO). To avoid this error, you have
to skip this config using --skip_set:

```bash
util/abuild/abuild --skip_set BOARD_ASUS_P8Z77_M_PRO
```

To complete the test, run abuild again specifically for this board
variant (see next section).

You can skip building other targets based on other Kconfigs. To skip
building targets without a Kconfig set, use --skip_unset:

```bash
util/abuild/abuild --skip_unset USE_NATIVE_RAMINIT
```
This example skips building configs not using (Sandy/Ivy Bridge) native
RAM init.

## Additional Examples

Many boards have multiple variants. You can build for a specific
variant of a board:

```bash
util/abuild/abuild -t asus/p8x7x-series -b p8z77-m_pro -p none
```

Many of the boards need files from the 'blobs' repository, which will
be initialized by the -B option. If the blobs repo has already been
initialized in your local tree, it won't hurt to add the -B.

```bash
util/abuild/abuild -B -t lenovo/x230 -p none
```

Adding ccache to your system and telling abuild to use it with the -y
option will speed things up a bit:

```bash
util/abuild/abuild -B -y -t lenovo/x230 -p none
```

Telling abuild to use multiple cores with the -c option helps speed
things up as well:

```bash
util/abuild/abuild -B -y -c 8 -t lenovo/x230 -p none
```

Of course, the real power of abuild is in testing multiple boards.

```bash
util/abuild/abuild -B -y -c 8 -p none
```

## Full options list

```text
coreboot autobuild v0.11.01 (Feb 3, 2023)
[...]
Usage: util/abuild/abuild [options]
       util/abuild/abuild [-V|--version]
       util/abuild/abuild [-h|--help]

Options:
    [-a|--all]                    Build previously succeeded ports as well
    [-A|--any-toolchain]          Use any toolchain
    [-b|--board-variant <name>]   Build specific board variant under the
                                  given target.
    [-B|--blobs]                  Allow using binary files
    [--checksum <path/basefile>]  Store checksums at path/basefile
    [-c|--cpus <numcpus>]         Build on <numcpus> at the same time
    [-C|--config]                 Configure-only mode
    [-d|--dir <dir>]              Directory containing config files
    [-e|--exitcode]               Exit with a non-zero errorlevel on failure
    [-J|--junit]                  Write JUnit formatted xml log file
    [-K|--kconfig <name>]         Prepend file to generated Kconfig
    [-l|--loglevel <num>]         Set loglevel
    [-L|--clang]                  Use clang on supported arch
    [-n|--name]                   Set build name - also sets xmlfile if not
                                  already set
    [-o|--outdir <path>]          Store build results in path
                                  (defaults to coreboot-builds)
    [-p|--payloads <dir>]         Use payloads in <dir> to build images
    [-P|--prefix <name>]          File name prefix in CBFS
    [-q|--quiet]                  Print fewer messages
    [-r|--remove]                 Remove output dir after build
    [-R|--root <path>]            Absolute path to coreboot sources
                                  (defaults to /usr/src/coreboot)
    [--scan-build]                Use clang's static analyzer
    [--skip_set <value>]          Skip building boards with this Kconfig set
    [--skip_unset <value>]        Skip building boards with this Kconfig not set
    [--timeless]                  Generate timeless builds
    [-t|--target <vendor/board>]  Attempt to build target vendor/board only
    [-T|--test]                   Submit image(s) to automated test system
    [-u|--update]                 Update existing image
    [-v|--verbose]                Print more messages
    [-x|--chromeos]               Build with CHROMEOS enabled
                                  Skip boards without ChromeOS support
    [-X|--xmlfile <name>]         Set JUnit XML log file filename
                                  (defaults to /usr/src/coreboot/abuild.xml)
    [-y|--ccache]                 Use ccache
    [-z|--clean]                  Remove build results when finished
    [-Z|--clean-somewhat]         Remove build but keep coreboot.rom + config

    [-V|--version]                Print version number and exit
    [-h|--help]                   Print this help and exit

    [-s|--silent]                 obsolete
```
