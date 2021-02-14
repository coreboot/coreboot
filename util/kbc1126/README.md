KBC1126 firmware tools
======================

Many HP laptops use 8051-based SMSC KBC1098/KBC1126 as embedded
controller. Two blobs can be found in the HP firmware images. The
`kbc1126_ec_dump` and `kbc1126_ec_insert` tools are used to dump the
two blobs from the factory firmware and insert them to the firmware
image.


Firmware format
---------------

We can easily find the BIOS region of the HP laptop firmware from the
HP firmware update tool, which can be downloaded from the HP
website. Now I take HP Elitebook 8470p as an example. This laptop has
a 16MB flash chip, the last 5MB of which is the BIOS region.

I use [radare2](https://radare.org) to analyze the firmware. Open the
firmware image, and we can see 8 bytes at `$s-0x100` (`$s` means the
image size).

  [0x00000000]> x @ $s-0x100
  - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF
  0x00ffff00  fff7 0008 f700 08ff 0000 0000 0000 0000  ................

X86 machines map the firmware at the end of the memory address
space. These 8 bytes tell the address of the two blobs, which we call
FW1 (uses bytes 0-3) and FW2 (uses bytes 4-7).

Let's look at FW1. The first two bytes mean the address of FW1 is
0xfff700 (these two bytes use big endian), i.e. `$s-0x900`. Byte 2 and
3 are just complements of byte 1 and 2 (in this case,
0x0008=0xffff-0xfff7).

  [0x00000000]> x @ $s-0x900
  - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF
  0x00fff700  fc07 c13e 02ff 1000 0000 0000 0000 0000  ...>............

Both FW1 and FW2 use the same format: the first two bytes is payload
length, then a two-byte checksum, then the payload. The payload length
and checksum are both in little endian. The checksum is
[SYSV checksum](https://en.wikipedia.org/wiki/SYSV_checksum).


How to use the tools
--------------------

`kbc1126_ec_dump` is used to dump FW1 and FW2. Run `kbc1126_ec_dump
bios.rom`, then bios.rom.fw1 and bios.rom.fw2 are generated in the
working directory.

`kbc1126_ec_insert` will overwrite a firmware image by inserting FW1
and FW2 in it. Please run it for its usage. You need to specify the
offsets for FW1 and FW2. Using negative offset is recommended, which
means the distance to the end of the image. For example, if we want to
insert FW1 and FW2 at `$s-0x900` and `$s-0x90000` as the hp/8470p
factory firmware to coreboot.rom, you can run `kbc1126_ec_insert
coreboot.rom bios.rom.fw1 bios.rom.fw2 -0x900 -0x90000`.
