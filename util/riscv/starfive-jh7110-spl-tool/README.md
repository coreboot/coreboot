## DESCRIPTION

spl_tool is a jh7110 signature tool used to generate spl header information and generate u-boot-spl.bin.normal.out.

spl_tool can also fix the issue of emmc booting.

## Prerequisites

Install required additional packages:

```bash
$ sudo apt-get install gcc make git
```

## Build

just run `make`

```bash
$ make
```

## Run

usage

```bash
$ ./spl_tool  -h

			StarFive spl tool

usage:
-c, --creat-splhdr	creat spl hdr
-i, --fix-imghdr	fixed img hdr for emmc boot.
-a, --spl-bak-addr	set backup SPL addr(default: 0x200000)
-v, --version		set version (default: 0x01010101)
-f, --file		input file name(spl/img)
-h, --help		show this information
```

Generate uboot-spl.bin.normal.out

```bash
$./spl_tool -c -f $(Uboot_PATH)/spl/u-boot-spl.bin
ubsplhdr.sofs:0x240, ubsplhdr.bofs:0x200000, ubsplhdr.vers:0x1010101 name:$(Uboot_PATH)/spl/u-boot-spl.bin
SPL written to $(Uboot_PATH)/spl/u-boot-spl.bin.normal.out successfully.
```

Fix the emmc boot issue

```bash
$ ./spl_tool -i -f sdcard.img
IMG  sdcard.img fixed hdr successfully.
```