# Frequency selection

## Introduction
This chapter explains the frequency selection done on Sandy Bridge and Ivy
Bridge memory initialization.

## Definitions
```{eval-rst}
+---------+-------------------------------------------------------------------+------------+--------------+
| Symbol  | Description                                                       | Units      | Valid region |
+=========+===================================================================+============+==============+
| SCK     | DRAM system clock cycle time                                      | s          |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| tCK     | DRAM system clock cycle time                                      | 1/256th ns |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| DCK     | Data clock cycle time: The time between two SCK clock edges       | s          |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| SPD     | Manufacturer set memory timings located on an EEPROM on every DIMM| bytes      |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| REFCK   | Reference clock, either 100 or 133                                | MHz        | 100, 133     |
+---------+-------------------------------------------------------------------+------------+--------------+
| MULT    | DRAM PLL multiplier                                               |            | [3-12]       |
+---------+-------------------------------------------------------------------+------------+--------------+
| XMP     | Extreme Memory Profiles                                           |            |              |
+---------+-------------------------------------------------------------------+------------+--------------+
```
## SPD
The [SPD](https://de.wikipedia.org/wiki/Serial_Presence_Detect "Serial Presence Detect")
located on every DIMM is factory program with various timings. One of them
specifies the maximum clock frequency the DIMM should be used with. The
operating frequency is stores as fixed point value (tCK), rounded to the next
smallest supported operating frequency. Some
[SPD](https://de.wikipedia.org/wiki/Serial_Presence_Detect "Serial Presence Detect")
contains additional and optional
[XMP](https://de.wikipedia.org/wiki/Extreme_Memory_Profile "Extreme Memory Profile")
data, that stores so called "performance" modes, that advertises higher clock
frequencies.

## XMP profiles
At time of writing coreboot's raminit is able to parse XMP profile 1 and 2.
Only **XMP profile 1** is being used in case it advertises:
* 1.5V operating voltage
* The channel's installed DIMM count doesn't exceed the XMP coded limit

In case the XMP profile doesn't fulfill those limits, the regular SPD will be
used.
> **Note:** XMP Profiles are supported since coreboot 4.4.

It is possible to ignore the max DIMM count limit set by XMP profiles.
By activating Kconfig option `NATIVE_RAMINIT_IGNORE_XMP_MAX_DIMMS` it is
possible to install two DIMMs per channel, even if XMP tells you not to do.

> **Note:** Ignoring XMP Profiles limit is supported since coreboot 4.7.

## Soft fuses
Every board manufacturer does program "soft" fuses to indicate the maximum
DRAM frequency supported. However, those fuses don't set a limit in hardware
and thus are called "soft" fuses, as it is possible to ignore them.

> **Note:** Ignoring the fuses might cause system instability !

On Sandy Bridge *CAPID0_A* is being read, and on Ivy Bridge *CAPID0_B* is being
read. coreboot reads those registers and honors the limit in case the Kconfig
option `CONFIG_NATIVE_RAMINIT_IGNORE_MAX_MEM_FUSES` wasn't set.
Power users that want to let their RAM run at DRAM's "stock" frequency need to
enable the Kconfig symbol.

It is possible to override the soft fuses limit by using a board-specific
[devicetree](#devicetree) setting.

> **Note:** Ignoring max mem freq. fuses is supported since coreboot 4.7.

## Hard fuses
"Hard" fuses are programmed by Intel and limit the maximum frequency that can
be used on a given CPU/board/chipset. At time of writing there's no register
to read this limit, before trying to set a given DRAM frequency. The memory PLL
won't lock, indicating that the chosen memory multiplier isn't available. In
this case coreboot tries the next smaller memory multiplier until the PLL will
lock.

## Devicetree
The devicetree register `max_mem_clock_mhz` overrides the "soft" fuses set
by the board manufacturer.

By using this register it's possible to force a minimum operating frequency.

## Reference clock
While Sandy Bridge supports 133 MHz reference clock (REFCK), Ivy Bridge also
supports 100 MHz reference clock. The reference clock is multiplied by the DRAM
multiplier to select the DRAM frequency (SCK) by the following formula:

 REFCK * MULT = 1 / DCK

> **Note:** Since coreboot 4.6 Ivy Bridge supports 100MHz REFCK.

## Sandy Bridge's supported frequencies
```{eval-rst}
+------------+-----------+------------------+-------------------------+---------------+
| SCK [Mhz]  | DDR [Mhz] | Mutiplier (MULT) | Reference clock (REFCK) | Comment       |
+============+===========+==================+=========================+===============+
| 400        | DDR3-800  | 3                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 533        | DDR3-1066 | 4                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 666        | DDR3-1333 | 5                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 800        | DDR3-1600 | 6                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 933        | DDR3-1866 | 7                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 1066       | DDR3-2166 | 8                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
```

## Ivy Bridge's supported frequencies
```{eval-rst}
+------------+-----------+------------------+-------------------------+---------------+
| SCK [Mhz]  | DDR [Mhz] | Mutiplier (MULT) | Reference clock (REFCK) | Comment       |
+============+===========+==================+=========================+===============+
| 400        | DDR3-800  | 3                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 533        | DDR3-1066 | 4                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 666        | DDR3-1333 | 5                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 800        | DDR3-1600 | 6                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 933        | DDR3-1866 | 7                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 1066       | DDR3-2166 | 8                | 133 MHz                 |               |
+------------+-----------+------------------+-------------------------+---------------+
| 700        | DDR3-1400 | 7                | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
| 800        | DDR3-1600 | 8                | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
| 900        | DDR3-1800 | 9                | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
| 1000       | DDR3-2000 | 10               | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
| 1100       | DDR3-2200 | 11               | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
| 1200       | DDR3-2400 | 12               | 100 MHz                 | '1            |
+------------+-----------+------------------+-------------------------+---------------+
```
> '1: since coreboot 4.6

## Multiplier selection
coreboot selects the maximum frequency to operate at by the following formula:
```
if devicetree's max_mem_clock_mhz > 0:
     freq_max := max_mem_clock_mhz
else:
     freq_max := soft_fuse_max_mhz

for i in SPDs:
     freq_max := MIN(freq_max, ddr_spd_max_mhz[i])
```

As you can see, by using DIMMs with different maximum DRAM frequencies, the
slowest DIMMs' frequency will be selected, to prevent over-clocking it.

The selected frequency gives the PLL multiplier to operate at. In case the PLL
locks (see Take me to [Hard fuses](#hard-fuses)) the frequency will be used for
all DIMMs. At this point it's not possible to change the multiplier again,
until the system has been powered off. In case the PLL doesn't lock, the next
smaller multiplier will be used until a working multiplier will be found.
