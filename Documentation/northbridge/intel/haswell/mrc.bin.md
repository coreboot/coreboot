# mrc.bin

All Haswell boards supported by coreboot currently require a proprietary
blob in order to initialise the DRAM and a few other components. The
blob, named `mrc.bin`, largely consists of Intel's memory reference code
(MRC), but it has been tailored specifically for ChromeOS. It is just
under 200 KiB in size. Another name for `mrc.bin` is the system agent
binary.

Having a replacement for `mrc.bin` using native coreboot code is very
much desired, but it is not an easy task.

## Obtaining mrc.bin

Unfortunately, it is not currently possible to distribute `mrc.bin` as
part of coreboot. Though, it can be obtained from a Haswell Chromebook
firmware image like so, starting in the root of the coreboot directory:

```bash
make -C util/cbfstool
cd util/chromeos
./crosfirmware.sh peppy
../cbfstool/cbfstool coreboot-*.bin extract -f mrc.bin -n mrc.bin -r RO_SECTION
```

Now, place `mrc.bin` in the root of the coreboot directory.
Alternatively, place `mrc.bin` anywhere you want, and set `MRC_FILE` to
its location when building coreboot.

## SPD Addresses

When porting a board from vendor firmware, the SPD addresses can be obtained
through `i2c-tools`, which can be found in many GNU/Linux distributions. A more
[detailed description](https://hannuhartikainen.fi/blog/hacking-ddr3-spd/) of
the procedure and beyond can be found in
[Hannu Hartikainen's blog](https://hannuhartikainen.fi).

First load the kernel modules:

```bash
modprobe i2c-dev
modprobe eeprom
```

Find the SMBus and the addresses of the DIMM's EEPROMs (example output):
```bash
$ decode-dimms | grep Decoding
Decoding EEPROM: /sys/bus/i2c/drivers/eeprom/7-0050
Decoding EEPROM: /sys/bus/i2c/drivers/eeprom/7-0052
```

Alternatively, look at the sys filesystem:
```bash
$ ls -l /sys/bus/i2c/drivers/eeprom/
total 0
lrwxrwxrwx 1 root root    0 Apr  4 01:46 6-0050 -> ../../../../devices/pci0000:00/0000:00:02.0/drm/card0/card0-eDP-1/i2c-6/6-0050/
lrwxrwxrwx 1 root root    0 Apr  4 01:46 7-0050 -> ../../../../devices/pci0000:00/0000:00:1f.3/i2c-7/7-0050/
lrwxrwxrwx 1 root root    0 Apr  4 01:46 7-0052 -> ../../../../devices/pci0000:00/0000:00:1f.3/i2c-7/7-0052/
--w------- 1 root root 4096 Apr  4 01:47 bind
lrwxrwxrwx 1 root root    0 Apr  4 01:47 module -> ../../../../module/eeprom/
--w------- 1 root root 4096 Apr  4 01:46 uevent
--w------- 1 root root 4096 Apr  4 01:47 unbind
```

The correct I2C bus is 7 in this case, and the EEPROMs are at `0x50` and `0x52`.
Note that the above values are actually hex values.

You can check the correctness of the SMBus and the addresses of the EEPROMs via
`i2cdetect`:

```bash
$ i2cdetect -l
i2c-3   unknown         i915 gmbus dpc                          N/A
i2c-1   unknown         i915 gmbus vga                          N/A
i2c-6   unknown         DPDDC-A                                 N/A
i2c-4   unknown         i915 gmbus dpb                          N/A
i2c-2   unknown         i915 gmbus panel                        N/A
i2c-0   unknown         i915 gmbus ssc                          N/A
i2c-7   unknown         SMBus I801 adapter at f040              N/A
i2c-5   unknown         i915 gmbus dpd                          N/A
```

Probing the SMBus:

```bash
$ i2cdetect -r 7
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-7 using receive byte commands.
I will probe address range 0x03-0x77.
Continue? [Y/n]
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: 30 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- 44 -- -- -- -- -- -- -- -- -- -- --
50: UU -- UU -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

The SPD addresses need to be left-shifted by 1 for `mrc.bin`, i.e., multiplied
by 2. For example, if the addresses read through `i2c-tools` when booted from
vendor firmware are `0x50` and `0x52`, the correct values would be `0xa0` and
`0xa4`. This is because the I2C addresses are 7 bits long.

## ECC DRAM

When `mrc.bin` has finished executing, ECC is active on the channels
populated with ECC DIMMs. However, `mrc.bin` was tailored specifically
for Haswell Chromebooks and Chomeboxes, none of which support ECC DRAM.
While ECC likely functions correctly, it is advised to further validate
the correct operation of ECC if data integrity is absolutely critical.
