# Sandy Bridge Raminit

## Introduction

This documentation is intended to document the closed source memory controller
hardware for Intel 2nd Gen (Sandy Bride) and 3rd Gen (Ivy Bridge) core-i CPUs.

The memory initialization code has to take care of lots of duties:
1. Selection of operating frequency
* Selection of common timings
* Applying frequency specific compensation values
* Read training of all populated channels
* Write training of all populated channels
* Adjusting delay networks of address and command signals
* DQS training of all populated channels
* Programming memory map
* Report DRAM configuration
* Error handling

## Definitions
```eval_rst
+---------+-------------------------------------------------------------------+------------+--------------+
| Symbol  | Description                                                       | Units      | Valid region |
+=========+===================================================================+============+==============+
| SCK     | DRAM system clock cycle time                                      | s          |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| tCK     | DRAM system clock cycle time                                      | 1/256th ns |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| DCK     | Data clock cycle time: The time between two SCK clock edges       | s          |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| timA    | IO phase: The phase delay of the IO signals                       | 1/64th DCK | [0-512)      |
+---------+-------------------------------------------------------------------+------------+--------------+
| SPD     | Manufacturer set memory timings located on an EEPROM on every DIMM| bytes      |              |
+---------+-------------------------------------------------------------------+------------+--------------+
| REFCK   | Reference clock, either 100 or 133                                | Mhz        | 100, 133     |
+---------+-------------------------------------------------------------------+------------+--------------+
| MULT    | DRAM PLL multiplier                                               |            | [3-12]       |
+---------+-------------------------------------------------------------------+------------+--------------+
| XMP     | Extreme Memory Profiles                                           |            |              |
+---------+-------------------------------------------------------------------+------------+--------------+
```

## (Inoffical) register documentation
- [Sandy Bride - Register documentation](SandyBridge_registers.md)

## Frequency selection
- [Sandy Bride - Frequency selection](Sandybridge_freq.md)

## Read training
- [Sandy Bride - Read training](Sandybridge_read.md)

### SMBIOS type 17
The SMBIOS specification allows to report the memory configuration in use.
On GNU/Linux you can run `# dmidecode -t 17` to view it.
Example output of dmidecode:

```
Handle 0x0045, DMI type 17, 34 bytes
    Memory Device
	Array Handle: 0x0042
	Error Information Handle: Not Provided
	Total Width: 64 bits
	Data Width: 64 bits
	Size: 8192 MB
	Form Factor: DIMM
	Set: None
	Locator: ChannelB-DIMM0
	Bank Locator: BANK 2
	Type: DDR3
	Type Detail: Synchronous
	Speed: 933 MHz
	Manufacturer: 0420
	Serial Number: 00000000
	Asset Tag: 9876543210
	Part Number: F3-1866C9-8GSR
	Rank: 2
	Configured Clock Speed: 933 MHz
```
The memory frequency printed by dmidecode is the active memory frequency. It's
**not** the double datarate and it's **not** the one encoded maximum frequency
in each DIMM's SPD.

> **Note:** This feature is available since coreboot 4.4

### MRC cache
The name *MRC cache* might be missleading as in case of *Native ram init*
there's no MRC, but for historical reasons it's still named *MRC cache*.
The MRC cache is part of flash memory that is writeable by coreboot.
At the end of the boot process coreboot will write the RAM training results to
flash for future use, as RAM training is time intensive. Storing the results
allows to boot faster on normal boot and allows to support S3 resume,
as the RAM training results can't be stored in RAM (you need to configure
the memory controller first to access RAM).

The MRC cache needs to be invalidated in case the memory configuration has
been changed. To detect a changed memory configuration the CRC16 of each DIMM
is stored to MRC cache.
> **Note:** This feature is available since coreboot 4.4

### Error handling
As of writing the only supported error handling is to disable the failing
channel and restart the memory training sequence. It's very likely to succeed,
as memory channels operate independent of each other.
In case no DIMM could be initilized coreboot will halt. The screen will stay
black until you power of your device. On some platforms there's additional
feedback to indicate such an event.

If you find `dmidecode -t 17` to report only half of the memory installed,
it's likely that a fatal memory init failure had happened.
It is assumed, that a working board with less physical memory, is much better,
than a board that doesn't boot at all.

> **Note:** This feature is available since coreboot 4.5

Try to swap memory modules and or try to use a different vendor. If nothing
helps you could have a look at capter [Debuggin] or report a ticket
at [ticket.coreboot.org]. Please provide a full RAM init log,
that has been captured using EHCI debug.

To enable extensive RAM training logging enable the Kconfig option
`DEBUG_RAM_SETUP`
#### Lenovo Thinkpads
Lenovo Thinkpads do have an additional feature to indicate that RAM init has
failed and coreboot has died (it calls die() on fatal error, thus the name).
The Kconfig options
`H8_BEEP_ON_DEATH`
`H8_FLASH_LEDS_ON_DEATH`
enable blinking LEDs and enable a beep to indicate death.

> **Note:** This feature is available since coreboot 4.7

## Debugging
It's recommended to use an external debugger, such as serial or EHCI debug
dongle. In case of failing memory init the board might not boot at all,
preventing you from using CBMEM.
