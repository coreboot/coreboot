# Gigabyte GA-G41M-ES2L rev 1.1

This page describes how to use coreboot on the [Gigabyte GA-G41M-ES2L rev 1.1](https://www.gigabyte.com/Motherboard/GA-G41M-ES2L-rev-11) mainboard.

This motherboard [also works with Libreboot](https://libreboot.org/docs/install/ga-g41m-es2l.html).

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Type             | Value                                            |
+==================+==================================================+
| BIOS flash chips | 2 x SST25VF080B (8 Mbit SPI) (DUAL BIOS)         |
+------------------+--------------------------------------------------+
| Northbridge      | Intel G41                                        |
+------------------+--------------------------------------------------+
| Southbridge      | Intel ICH7                                       |
+------------------+--------------------------------------------------+
| CPU socket       | LGA775                                           |
+------------------+--------------------------------------------------+
| RAM              | 2 x DDR2 800, max. 8 GiB                         |
+------------------+--------------------------------------------------+
| SuperIO          | ITE IT8718F-S                                    |
+------------------+--------------------------------------------------+
| Audio            | Realtek ALC888B                                  |
+------------------+--------------------------------------------------+
| Network          | Realtek RTL8111C PCIe Gigabit Ethernet           |
+------------------+--------------------------------------------------+
```

## Preparation

```eval_rst
For more datails how to get sources and build the toolchain, see :doc:`../../tutorial/part1`.
```

### Devuan 4 Chimaera

This probably works also for any fresh Debian/Ubuntu-based distros.

Install tools and libraries needed for coreboot:

```shell
sudo apt-get -V install bison build-essential curl flex git gnat libncurses5-dev m4 zlib1g-dev wget python2 python-is-python2 flashrom
```

### Get sources

You need about 700 MB disk space for sources only and ~2GB disk space for sources + build results

```shell
git clone --recursive https://review.coreboot.org/coreboot.git
```

### Build toolchain

Your system compilers can be different with versions, tested by coreboot developers.
So, it is recommended to build cross-compilers with special versions, which were tested with coreboot.

It is possible to skip this time-consuming part and use `ANY_TOOLCHAIN=y`, but this not recommended.

You can build them for all platforms: `make crossgcc CPUS=2` but this takes ~2 hours with Intel core2duo E8400.

The best way, probably, is to build cross-compilers for your platform (this takes ~20 minutes with Intel core2duo E8400):

```shell
make crossgcc-i386 CPUS=2
```

### Save MAC-address of internal LAN

Run `ip -c link show`, you will find MAC-address like 6c:f0:49:xx:xx:xx

```
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
    link/ether 6c:f0:49:xx:xx:xx brd ff:ff:ff:ff:ff:ff
```

## Configure

Create file `payloads/external/SeaBIOS/.config_seabios`:

```shell
CONFIG_COREBOOT=y
CONFIG_ATA_DMA=y
CONFIG_VGA_COREBOOT=y
```

Edit file `configs/config.gigabyte_ga-g41m-es2l`, replace `CONFIG_REALTEK_8168_MACADDRESS` value with your MAC-address.

Run

```shell
make defconfig KBUILD_DEFCONFIG="configs/config.gigabyte_ga-g41m-es2l"
```

## Build

Just execute:

```shell
make
```

It takes ~2 minutes with Intel core2duo E8400.

Example of last part in the output:

```
    CBFSPRINT  coreboot.rom

FMAP REGION: COREBOOT
Name                           Offset     Type           Size   Comp
cbfs master header             0x0        cbfs header        32 none
fallback/romstage              0x80       stage           62316 none
cpu_microcode_blob.bin         0xf480     microcode      180224 none
fallback/ramstage              0x3b500    stage           98745 none
vgaroms/seavgabios.bin         0x53700    raw             28672 none
config                         0x5a740    raw               301 none
revision                       0x5a8c0    raw               675 none
build_info                     0x5abc0    raw               103 none
fallback/dsdt.aml              0x5ac80    raw              8447 none
rt8168-macaddress              0x5cdc0    raw                17 none
vbt.bin                        0x5ce40    raw               802 LZMA (1899 decompressed)
cmos.default                   0x5d1c0    cmos_default      256 none
cmos_layout.bin                0x5d300    cmos_layout      1040 none
fallback/postcar               0x5d740    stage           20844 none
fallback/payload               0x62900    simple elf      70270 none
payload_config                 0x73bc0    raw              1699 none
payload_revision               0x742c0    raw               237 none
(empty)                        0x74400    null           482904 none
bootblock                      0xea280    bootblock       23360 none
    HOSTCC     cbfstool/ifwitool.o
    HOSTCC     cbfstool/ifwitool (link)

Built gigabyte/ga-g41m-es2l (GA-G41M-ES2L)
```

## Flashing coreboot

```eval_rst
In addition to the information here, please see the
:doc:`../../tutorial/flashing_firmware/index`.
```

### Do backup

The above commands read the SPI flash chip(s), write into file and then verify content again with the chip:

```shell
sudo flashrom -p internal:dualbiosindex=0 -r m_bios.rom
sudo flashrom -p internal:dualbiosindex=0 -v m_bios.rom
sudo flashrom -p internal:dualbiosindex=1 -r b_bios.rom
sudo flashrom -p internal:dualbiosindex=1 -v b_bios.rom
```

If access error appeared, then add `iomem=relaxed` to Linux kernel parameters and restart your Linux system.

You can also repeat backup and compare checksums manually.

Backup file should be stored elsewhere, so that in case the coreboot build is faulty, some external procedure can be used without having to extract the backup from the target device first.

### Write new flash image

Let's write new image into SPI flash chip, verify checksum again and erase second flash chip:

```shell
sudo flashrom -p internal:dualbiosindex=0 -w build/coreboot.rom
sudo flashrom -p internal:dualbiosindex=0 -v build/coreboot.rom
sudo flashrom -p internal:dualbiosindex=1 -E
```

## Set text mode for GRUB

Update your `/etc/default/grub` with:

```shell
GRUB_TERMINAL=console
```

And recreate GRUB configuration `/boot/grub/grub.cfg` by command

```shell
sudo update-grub
```

## Boot with new firmware

Restart your system:

```shell
sudo shutdown -r now
```

If it is needed, use <kbd>Esc</kbd> key to choose boot device.

Remove `iomem=relaxed` from Linux kernel parameters.

Enjoy!

## Status

```
+-----------------------+--------------------------+--------+-------------------------------+
| coreboot version      | Date of sources checkout | Status | Comment                       |
+-----------------------+--------------------------+--------+-------------------------------+
| 4.13-1531-g2fae1c0494 | 2021-01-28               | Good   |                               |
+-----------------------+--------------------------+--------+-------------------------------+
| 4.13-2182-g6410a0002f | 2021-02-18               | Good   |                               |
+-----------------------+--------------------------+--------+-------------------------------+
```

### Known issues

Lm-sensors shows wrong values from it87:

```
coretemp-isa-0000
Adapter: ISA adapter
Core 0:       +27.0°C  (high = +80.0°C, crit = +100.0°C)
Core 1:       +31.0°C  (high = +80.0°C, crit = +100.0°C)

it8718-isa-0290
Adapter: ISA adapter
in0:           1.06 V  (min =  +0.00 V, max =  +4.08 V)
in1:           1.90 V  (min =  +0.00 V, max =  +4.08 V)
in2:           3.34 V  (min =  +0.00 V, max =  +4.08 V)
+5V:           2.96 V  (min =  +0.00 V, max =  +4.08 V)
in4:         224.00 mV (min =  +0.00 V, max =  +4.08 V)
in5:           4.08 V  (min =  +0.00 V, max =  +4.08 V)  ALARM
in6:           4.08 V  (min =  +0.00 V, max =  +4.08 V)  ALARM
in7:           3.09 V  (min =  +0.00 V, max =  +4.08 V)
Vbat:          2.82 V
fan1:        1290 RPM  (min =    0 RPM)
fan2:           0 RPM  (min =    0 RPM)
temp1:        -54.0°C  (low  =  +0.0°C, high = +127.0°C)  sensor = thermistor
temp2:         -1.0°C  (low  =  +0.0°C, high = +127.0°C)  sensor = thermistor
temp3:        +44.0°C  (low  =  +0.0°C, high = +127.0°C)  sensor = thermal diode
cpu0_vid:    +1.100 V
intrusion0:  ALARM
```

### Working

- RAM 1,2x1GiB DDR2 PC2-6400 Kingston KTC1G-UDIMM (1.8V, 2Rx8 ?)
- RAM 1x1GiB DDR2 PC2-5300 Brooktree AU1G08E32-667P005 / Apogee AU1G082-667P005 CL6 (1.8V, 2Rx8 ?)
- CPU E8400
- ACPI
- CPU frequency scaling
- flashrom under coreboot
- Gigabit Ethernet
- Hardware monitoring
- Integrated graphics
- SATA
- PCI POST card

### Not working

- SuperIO based fan control: PWM fan speed is not changing in depend of CPU temperature
- RAM 1,2x4GiB DDR2 PC2-6400 Samsung M378T5263AZ3-CF7 (2Rx4 PC2-6400U-666-12-E3)

### Not tested

- KVM virtualization
- Onboard audio
- PCI
- PCIe
- PS/2 keyboard mouse (during payload, bootloader)
- Serial port
- USB (disabling XHCI controller makes to work as fallback USB2.0 ports)
- IOMMU

## Interesting facts

`lshw` output is different for BIOS and coreboot.

```shell
diff --side-by-side --ignore-all-space --strip-trailing-cr \
     Documentation/mainboard/gigabyte/ga-g41m-es2l_lshw_before_coreboot.txt \
     Documentation/mainboard/gigabyte/ga-g41m-es2l_lshw_after_coreboot.txt
```
