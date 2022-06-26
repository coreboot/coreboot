coreboot README
===============

coreboot is a Free Software project aimed at replacing the proprietary
firmware (BIOS/UEFI) found in most computers. coreboot performs the
required hardware initialization to configure the system, then passes
control to a different executable, referred to in coreboot as the
payload. Most often, the primary function of the payload is to boot the
operating system (OS).

With the separation of hardware initialization and later boot logic,
coreboot is perfect for a wide variety of situations. It can be used
for specialized applications that run directly in the firmware, running
operating systems from flash, loading custom bootloaders, or
implementing firmware standards, like PC BIOS services or UEFI. This
flexibility allows coreboot systems to include only the features
necessary in the target application, reducing the amount of code and
flash space required.


Source code
-----------

All source code for coreboot is stored in git. It is downloaded with
the command:

`git clone https://review.coreboot.org/coreboot.git`.

Code reviews are done in [the project's Gerrit
instance](https://review.coreboot.org/).

The code may be browsed via [coreboot's Gitiles
instance](https://review.coreboot.org/plugins/gitiles/coreboot/+/refs/heads/master).

The coreboot project also maintains a
[mirror](https://github.com/coreboot/coreboot) of the project on github.
This is read-only, as coreboot does not accept github pull requests,
but allows browsing and downloading the coreboot source.

Payloads
--------

After the basic initialization of the hardware has been performed, any
desired "payload" can be started by coreboot.

See <https://doc.coreboot.org/payloads.html> for a list of some of
coreboot's supported payloads.


Supported Hardware
------------------

The coreboot project supports a wide range of architectures, chipsets,
devices, and mainboards. While not all of these are documented, you can
find some information in the [Architecture-specific
documentation](https://doc.coreboot.org/arch/index.html) or the
[SOC-specific documentation](https://doc.coreboot.org/soc/index.html).

For details about the specific mainboard devices that coreboot supports,
please consult the [Mainboard-specific
documentation](https://doc.coreboot.org/mainboard/index.html) or the
[Board Status](https://coreboot.org/status/board-status.html) pages.


Releases
--------

Releases are currently done by coreboot every quarter. The
release archives contain the entire coreboot codebase from the time of
the release, along with any external submodules. The submodules
containing binaries are separated from the general release archives. All
of the packages required to build the coreboot toolchains are also kept
at coreboot.org in case the websites change, or those specific packages
become unavailable in the future.

All releases are available on the [coreboot
download](https://coreboot.org/downloads.html) page.

Please note that the coreboot releases are best considered as snapshots
of the codebase, and do not currently guarantee any sort of extra
stability.

Build Requirements and building coreboot
----------------------------------------

The coreboot build, associated utilities and payloads require many
additional tools and packages to build. The actual coreboot binary is
typically built using a coreboot-controlled toolchain to provide
reproducibility across various platforms. It is also possible, though
not recommended, to make it directly with your system toolchain.
Operating systems and distributions come with an unknown variety of
system tools and utilities installed. Because of this, it isn't
reasonable to list all the required packages to do a build, but the
documentation lists the requirements for a few different Linux
distributions.

To see the list of tools and libraries, along with a list of
instructions to get started building coreboot, go to the [Starting from
scratch](https://doc.coreboot.org/tutorial/part1.html) tutorial page.

That same page goes through how to use QEMU to boot the build and see
the output.


Website and Mailing List
------------------------

Further details on the project, as well as links to documentation and
more can be found on the coreboot website:

  <https://www.coreboot.org>

You can contact us directly on the coreboot mailing list:

  <https://doc.coreboot.org/community/forums.html>



Copyrights and Licenses
---------------------


### Uncopyrightable files

There are many files in the coreboot tree that we feel are not
copyrightable due to a lack of creative content.

"In order to qualify for copyright protection in the United States, a
work must satisfy the originality requirement, which has two parts. The
work must have “at least a modicum” of creativity, and it must be the
independent creation of its author."

  <https://guides.lib.umich.edu/copyrightbasics/copyrightability>

Similar terms apply to other locations.

These uncopyrightable files include:

- Empty files or files with only a comment explaining their existence.
  These may be required to exist as part of the build process but are
  not needed for the particular project.
- Configuration files either in binary or text form. Examples would be
  files such as .vbt files describing graphics configuration, spd files
  as binary .spd or text \*spd\*.hex representing memory chip
  configuration.
- Machine-generated files containing version numbers, dates, hash
  values or other "non-creative" content.

As non-creative content, these files are in the public domain by
default.  As such, the coreboot project excludes them from the project's
general license even though they may be included in a final binary.

If there are questions or concerns about this policy, please get in
touch with the coreboot project via the mailing list.


### Copyrights

The copyright on coreboot is owned by quite a large number of individual
developers and companies. A list of companies and individuals with known
copyright claims is present at the top level of the coreboot source tree
in the 'AUTHORS' file. Please check the git history of each of the
source files for details.


### Licenses

Because of the way coreboot began, using a significant amount of source
code from the Linux kernel, it's licensed the same way as the Linux
Kernel, with GNU General Public License (GPL) Version 2. Individual
files are licensed under various licenses, though all are compatible
with GPLv2. The resulting coreboot image is licensed under the GPL,
version 2. All source files should have an SPDX license identifier at
the top for clarification.

Files under coreboot/Documentation/ are licensed under CC-BY 4.0 terms.
As an exception, files under Documentation/ with a history older than
2017-05-24 might be under different licenses.

Files in the coreboot/src/commonlib/bsd directory are all licensed with
the BSD-3-clause license.  Many are also dual-licensed GPL-2.0-only or
GPL-2.0-or-later.  These files are intended to be shared with libpayload
or other BSD licensed projects.

The libpayload project contained in coreboot/payloads/libpayload may be
licensed as BSD or GPL, depending on the code pulled in during the build
process. All GPL source code should be excluded unless the Kconfig
option to include it is set.


The Software Freedom Conservancy
--------------------------------

Since 2017, coreboot has been a member of [The Software Freedom
Conservancy](https://sfconservancy.org/), a nonprofit organization
devoted to ethical technology and driving initiatives to make technology
more inclusive. The conservancy acts as coreboot's fiscal sponsor and
legal advisor.
