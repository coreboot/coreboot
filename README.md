coreboot README
===============

coreboot is a Free Software project aimed at replacing the proprietary BIOS
(firmware) found in most computers.  coreboot performs a little bit of
hardware initialization and then executes additional boot logic, called a
payload.

With the separation of hardware initialization and later boot logic,
coreboot can scale from specialized applications that run directly
firmware, run operating systems in flash, load custom
bootloaders, or implement firmware standards, like PC BIOS services or
UEFI. This allows for systems to only include the features necessary
in the target application, reducing the amount of code and flash space
required.

coreboot was formerly known as LinuxBIOS.


Payloads
--------

After the basic initialization of the hardware has been performed, any
desired "payload" can be started by coreboot.

See <https://www.coreboot.org/Payloads> for a list of supported payloads.


Supported Hardware
------------------

coreboot supports a wide range of chipsets, devices, and mainboards.

For details please consult:

 * <https://www.coreboot.org/Supported_Motherboards>


Build Requirements
------------------

 * make
 * gcc / g++
   Because Linux distribution compilers tend to use lots of patches. coreboot
   does lots of "unusual" things in its build system, some of which break due
   to those patches, sometimes by gcc aborting, sometimes - and that's worse -
   by generating broken object code.
   Two options: use our toolchain (eg. make crosstools-i386) or enable the
   `ANY_TOOLCHAIN` Kconfig option if you're feeling lucky (no support in this
   case).
 * iasl (for targets with ACPI support)
 * pkg-config
 * libssl-dev (openssl)

Optional:

  * gdb (for better debugging facilities on some targets)
 * ncurses (for `make menuconfig` and `make nconfig`)
 * flex and bison (for regenerating parsers)


Building coreboot
-----------------

Please consult <https://www.coreboot.org/Build_HOWTO> for details.


Testing coreboot Without Modifying Your Hardware
------------------------------------------------

If you want to test coreboot without any risks before you really decide
to use it on your hardware, you can use the QEMU system emulator to run
coreboot virtually in QEMU.

Please see <https://www.coreboot.org/QEMU> for details.


Website and Mailing List
------------------------

Further details on the project, a FAQ, many HOWTOs, news, development
guidelines and more can be found on the coreboot website:

  <https://www.coreboot.org>

You can contact us directly on the coreboot mailing list:

  <https://www.coreboot.org/Mailinglist>


Copyright and License
---------------------

The copyright on coreboot is owned by quite a large number of individual
developers and companies. Please check the individual source files for details.

coreboot is licensed under the terms of the GNU General Public License (GPL).
Some files are licensed under the "GPL (version 2, or any later version)",
and some files are licensed under the "GPL, version 2". For some parts, which
were derived from other projects, other (GPL-compatible) licenses may apply.
Please check the individual source files for details.

This makes the resulting coreboot images licensed under the GPL, version 2.
