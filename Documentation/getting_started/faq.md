# coreboot FAQ

## General coreboot questions


### What is coreboot?

coreboot is a free and open software project designed to initialize
computers and embedded systems in a fast, secure, and auditable fashion.
The focus is on minimal hardware initialization: to do only what is
absolutely needed, then pass control to other software (a payload, in
coreboot parlance) in order to boot the operating system securely.


### What is a coreboot payload?

coreboot itself does not deal with boot media such as hard-drives,
SSDs, or USB flash-drives, beyond initializing the underlying hardware.
So in order to actually boot an operating system, another piece of
software which does do those things must be used. coreboot supports
a large number of diverse payloads; see below for more details.


### Is coreboot the same as UEFI?

No. coreboot and UEFI are both system firmware that handle the
initialization of the hardware, but are otherwise not similar.
coreboot’s goal is to **just** initialize the hardware and exit.
This makes coreboot smaller and simpler, leading to faster boot times,
and making it easier to find and fix bugs. The result is a higher
overall security.


### What's the difference between coreboot and UEFI?

UEFI is actually a firmware specification, not a specific software
implementation. Intel, along with the rest of the Tianocore project,
has released an open-source implementation of the overall framework,
EDK2, but it does not come with hardware support. Most hardware running
UEFI uses a proprietary implementation built on top of EDK2.

coreboot does not implement the UEFI specification, but it can be used to
initialize the system, then launch a UEFI payload such as EDK2 in order
to provide UEFI boot services.

The UEFI specification also defines and allows for many things that are
outside of coreboot’s scope, including (but not limited to):

* Boot device selection
* Updating the firmware
* A CLI shell
* Network communication
* An integrated setup menu


### Can coreboot boot operating systems that require UEFI?

Yes, but... again, coreboot **just** initializes the hardware. coreboot
itself doesn’t load operating systems from storage media other than the
flash chip. Unlike UEFI, coreboot does not, and will not contain a Wi-Fi
driver or communicate directly with any sort of network. That sort of
functionality is not related to hardware initialization.

To boot operating systems that require UEFI, coreboot can be compiled with
EDK2 as the payload. This allows coreboot to perform the hardware init,
with EDK2 supplying the UEFI boot interface and runtime services to
the operating system.


### What non-UEFI payloads does coreboot support?

* SeaBIOS, behaves like a classic BIOS, allowing you to boot operating
  systems that rely on the legacy interrupts.

* GRUB can be used as a coreboot payload, and is currently the most
  common approach to full disk encryption (FDE).

* A Linux kernel and initramfs stored alongside coreboot in the boot
  ROM can also be used as a payload. In this scenario coreboot
  initializes hardware, loads Linux from boot ROM into RAM, and
  executes it. The embedded Linux environment can look for a target OS
  kernel to load from local storage or over a network and execute it
  using kexec. This is sometimes called LinuxBoot.

* U-boot, depthcharge, FILO, etc.

There’s [https://doc.coreboot.org/payloads.html](https://doc.coreboot.org/payloads.html)
with a list, although it’s not complete.


### What does coreboot leave in memory after it's done initializing the hardware?

While coreboot tries to remove itself completely from memory after
finishing, some tables and data need to remain for the OS. coreboot
reserves an area in memory known as CBMEM, to save this data after it
has finished booting. This contains things such as the boot log, tables
that get passed to the payload, SMBIOS, and ACPI tables for the OS.

In addition to CBMEM, on X86 systems, coreboot will typically set up
SMM, which will remain resident after coreboot exits.


## Platforms

### What’s the best coreboot platform for a user?

The choice of the best coreboot platform for a user can vary depending
on their specific needs, preferences, and use cases.

Typically, people who want a system with a minimum of proprietary
firmware are restricted to older systems like the Lenovo X220, or more
expensive, non-x86 solutions like TALOS, from Raptor Engineering.

There are a number of companies selling modern systems, but those all
require more proprietary binaries in addition to coreboot (e.g., Intel
FSP). However, unlike the older ThinkPads, many of these newer devices
use open-source embedded controller (EC) firmware, so there are
tradeoffs with either option.

The coreboot project mantains a list of companies selling machines
which use coreboot on the [website](https://coreboot.org/users.html).


### What’s the best platform for coreboot development?

Similar to the best platform for users, the best platform for
developers very much depends on what a developer is trying to do.

* QEMU is generally the easiest platform for coreboot development, just
  because it’s easy to run anywhere. However, it’s possible for things
  to work properly in QEMU but fail miserably on actual hardware.

While laptops tend to be harder to develop than desktop platforms, a
majority of newer platforms on coreboot tend to be laptops. The
development difficulty is due to a few different factors:

1. The EC (Embedded Controller) is a specialized microcontroller that
   typically handles keyboard and sometimes mouse input for a laptop.
   It also controls many power management functions such as fans, USB-C
   power delivery, etc. ECs run mainboard-specific firmware, which is
   typically undocumented.
2. ThinkPads (X230, 30-series, 20-series, T430, T540, T520). Sandy
   Bridge and Ivy Bridge are well-supported. Some may have
   difficult-to-reach SPI flash chips. Boards with two flash chips (e.g.
   30-series ThinkPads) are harder to externally reflash as one needs to
   make sure the non-targeted flash chip remains disabled at all times.
   The X230 is notoriously sensitive to external reflashing issues.
3. Laptops often lack a convenient method to obtain firmware boot logs.
   One can use EHCI debug on older systems and Chromebook-specific
   solutions for Chromebooks, but one often has to resort to flashconsole
   (writing coreboot logs to the flash chip where coreboot resides). On
   the other hand, several desktop mainboards still have a RS-232 serial
   port.

Some of the easiest physical systems to use for coreboot development
are Chromebooks. Newer Chromebooks allow for debug without opening the
case. Look for SuzyQ Cables or SuzyQables or instructions on how to
build one. These cables only work on a specific port in a specific
orientation. Google [supplies
specifications](https://chromium.googlesource.com/chromiumos/third_party/hdctools/+/master/docs/ccd.md#SuzyQ-SuzyQable)
for these cables.


### What platforms does coreboot support?

The most accurate way to determine what systems coreboot supports is by
browsing the src/mainboard tree or running “make menuconfig” and going
through the “Mainboard” submenu. You can also search Gerrit to see if
there are any unmerged ports for your board.

There is also the board status page
([https://coreboot.org/status/board-status.html](https://coreboot.org/status/board-status.html)),
however this does not currently show supported board variants.


## coreboot Development

### Can coreboot be ported to [this board]?

The best way to determine if coreboot can be ported to a system is to
see if the processor and chipset is supported. The next step is to see
whether the system is locked to the proprietary firmware which comes
with the board.

Intel Platforms:

* coreboot only supports a few northbridges (back when northbridges
  were on a separate package), and there's next to no support for
  "server" platforms (multi-socket and similar things). Here's a list
  of more recent supported Intel processors:
    * Alder Lake (2021 - Core Gen 12)
    * Apollo Lake (2016 - Atom)
    * Baytrail (2014 - Atom)
    * Braswell (2016 - Atom)
    * Broadwell (2014 - Core Gen 5)
    * Comet Lake (2019 - Core Gen 10)
    * Cannon Lake (2018 - Core Gen 8/9)
    * Denverton (2017)
    * Elkhart lake (2021 - Atom)
    * Haswell (2013 - Core Gen 4)
    * Ivy Bridge (2012 - Core Gen 3)
    * Jasper Lake (2021 - Atom)
    * Kaby Lake (2016 - Core Gen 7/8)
    * Meteor Lake (2023 - Gen 1 Ultra-mobile)
    * Sandy Bridge (2011 - Core Gen 2)
    * Sky Lake (2015 - Core Gen 6)
    * Tiger Lake (2020 - Core Gen 11)
    * Whiskey Lake (2018 - Core Gen 8)

* Intel Boot Guard is a security feature which tries to prevent loading
  unauthorized firmware by the mainboard. If supported by the platform,
  and the platform is supported by intelmetool, you should check if Boot
  Guard is enabled. If it is, then getting coreboot to run will be
  difficult or impossible even if it is ported. You can run
  `intelmetool -b` on supported platforms to see if Boot Guard is
  enabled (although it can fail because it wants to probe the ME
  beforehand).

AMD Ryzen-based platforms:

* The AMD platforms Ryzen-based platforms unfortunately are currently
  not well supported outside of the Chromebooks (and AMD reference
  boards) currently in the tree.
  The responsible teams are trying to fix this, but currently it's
  **very** difficult to do a new port. Recent supported SoCs:
    * Stoney Ridge
    * Picasso
    * Cezanne
    * Mendocino
    * Phoenix

General notes:

* Check the output of `lspci` to determine what processor/chipset
  family your system has. Processor/chipset support is the most
  important to determine if a board can be ported.
* Check the output of `superiotool` to see if it detects the Super I/O
  on the system. You can also check board schematics and/or boardviews
  if you can find them, or physically look at the mainboard for a chip
  from one of the common superio vendors.
* Check what EC your system has (mostly applicable to laptops, but some
  desktops have EC-like chips). You will likely need to refer to the
  actual board or schematics/boardviews for this. Physical observation
  is the most accurate identification procedure; software detection can
  then be used to double-check if the chip is correct, but one should
  not rely on software detection alone to identify an EC.


### How do I port coreboot to [this board]?

A critical piece for anyone attempting to do a board port is to make
sure that you have a method to recover your system from a failed flash.

We need an updated motherboard porting guide, but currently the guide
on the [wiki](https://www.coreboot.org/Motherboard_Porting_Guide) looks
to be the best reference.

At the moment, the best answer to this question is to ask for help on
one of the [various community
forums](https://doc.coreboot.org/community/forums.html).


### What about the Intel ME?

There seems to be a lot of FUD about what the ME can and can’t do.
coreboot currently does not have a clear recommendation on how to
handle the ME. We understand that there are serious concerns about the
ME, and would like to flatly recommend removing as much as possible,
however modifying the ME can cause serious stability issues.

Additionally, coreboot and the Intel ME are completely separate entites
which in many cases simply happen to occupy the same flash chip. It is
not necessary to run coreboot to modify the ME, and running coreboot
does not imply anything about the ME's operational state.


#### A word of caution about the modifying ME

Messing with the ME firmware can cause issues, and this is outside the
scope of the coreboot project.

If you do decide to modify the ME firmware, please make sure coreboot
works **before** messing with it. Even if the vendor boot firmware
works when the ME isn't operating normally, it's possible that coreboot
doesn't handle it the same way and something breaks. If someone asks
for help with coreboot and we think the ME state may be a factor, we'll
ask them to try reproducing the issue with the ME running normally to
reduce the number of variables involved. This is especially important
when flashing coreboot for the first time, as it's best for newbies to
start with small steps: start by flashing coreboot to the BIOS region
and leaving the remaining regions untouched, then tinker around with
coreboot options (e.g. other payloads, bootsplash, RAM overclock...),
or try messing with the ME firmware **without changing coreboot**.

Most people don't understand the implications of messing with the ME
firmware, especially the use of `me_cleaner`. We admit that we don't
know everything about the ME, but we try to understand it as much as
possible. The ME is designed to operate correctly with the HAP (or
AltMeDisable) bit set, and it will gracefully enter a debug state (not
normal, but not an error). However, when using `me_cleaner` to remove
parts of the ME firmware, the ME will often end up in an error state
because parts of its FW are missing. It is known that removing some of
these parts ([`EFFS` and `FCRS` on Cougar Point,
c.f.](https://review.coreboot.org/c/coreboot/+/27798/6/src/mainboard/asus/p8h61-m_lx/Kconfig#63))
can cause problems. We do not know whether the state the ME ends up in
after applying `me_cleaner` is as secure as the state the ME goes to
when only the HAP bit is set: the removed FW modules could contain
steps to lock down important settings for security reasons.

To sum up, **we do not recommend messing with the ME firmware**. But if
you have to, please use `ifdtool` to set the HAP bit initially before
progressing to `me_cleaner` if necessary.
