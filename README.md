coreboot
========

coreboot is a free software project aimed at replacing the proprietary BIOS
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

See http://www.coreboot.org/Payloads for a list of supported payloads.


Supported Hardware
------------------

coreboot supports a wide range of chipsets, devices, and mainboards.

For details please consult:

 * http://www.coreboot.org/Supported_Motherboards
 * http://www.coreboot.org/Supported_Chipsets_and_Devices


Build Requirements
------------------

 * gcc / g++
 * make

Optional:

 * doxygen (for generating/viewing documentation)
 * iasl (for targets with ACPI support)
 * gdb (for better debugging facilities on some targets)
 * ncurses (for `make menuconfig`)
 * flex and bison (for regenerating parsers)


Building coreboot
-----------------

Please consult http://www.coreboot.org/Build_HOWTO for details.


Testing coreboot Without Modifying Your Hardware
------------------------------------------------

If you want to test coreboot without any risks before you really decide
to use it on your hardware, you can use the QEMU system emulator to run
coreboot virtually in QEMU.

Please see http://www.coreboot.org/QEMU for details.


Website and Mailing List
------------------------

Further details on the project, a FAQ, many HOWTOs, news, development
guidelines and more can be found on the coreboot website:

  http://www.coreboot.org

You can contact us directly on the coreboot mailing list:

  http://www.coreboot.org/Mailinglist


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

