# HP Sure Start

According to the [HP Sure Start Technical Whitepaper], HP Sure Start is a chipset
and processor independent firmware intrusion detection and automatic repair system.
It is implemented in HP notebooks since 2013, and desktops since 2015.

This document talks about some mechanism of HP Sure Start on some machines, and
the method to bypass it.

## Laptops with SMSC MEC1322 embedded controller

Haswell EliteBook, ZBook and ProBook 600 series use SMSC MEC1322 embedded controller.
The EC firmware implements HP Sure Start.

A Haswell EliteBook has two flash chips. According to the strings in the EC firmware,
the 16MiB flash chip that stores the BIOS firmware is called the *system flash*, and
the 2MiB flash chip that stores part of the system flash content is called the
*private flash*. A Haswell ProBook 600 series laptop also uses MEC1322 and has similar
EC firmware, but the HP Sure Start functions are not enabled.

The private flash is connected to the EC, and is not accessible by the OS.
It contains the following:

- HP Sure Start policy header (starting with the string "POLI")
- A copy of the Intel Flash Descriptor
- A copy of the GbE firmware
- Machine Unique Data (MUD)
- Hashes of the IFD, GbE firmware and MUD, the hash algorithm is unknown
- A copy of the bootblock, UEFI PEI stage, and microcode

If the IFD of the system flash does not match the hash in the private flash, for example,
modifying the IFD with ``ifdtool -u`` or ``me_cleaner -S``, the EC will recover the IFD.

If the content of the private flash is lost, the EC firmware will still copy the IFD,
bootblock and PEI to the private flash. However, the IFD is not protected after that.

HP Sure Start also verifies bootblock, PEI, and microcode without using the private flash.
EC firmware reads them from an absolute address of the system flash chip, which is
hardcoded in the EC firmware. It looks like this verification is done with a digital
signature. If the PEI volume is modified, EC firmware will recover it using the copy
in the private flash. If the private flash has no valid copies of the PEI volume, and
the PEI volume is modified, the machine will refuse to boot with the CapsLock LED blinking.

## Bypassing HP Sure Start

First search the mainboard for the flash chips. If there are two flash chips,
the smaller one may be the private flash.

For Intel boards, try to modify the IFD with ``ifdtool -u``, power on and shut down
the machine, then read the flash again. If the IFD is not modified, it is likely to
be recovered from the private flash. Find the private flash and erase it, then the IFD
can be modified.

To bypass the bootblock and PEI verification, we can modify the IFD to make the
BIOS region not overlap with the protected region. Since the EC firmware is usually
located at the high address of the flash chip (and in the protected region),
we can leave it untouched, and do not need to extract the EC firmware to put it in
the coreboot image.

[HP Sure Start Technical Whitepaper]: http://h10032.www1.hp.com/ctg/Manual/c05163901
