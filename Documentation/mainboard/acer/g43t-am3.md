# Acer G43T-AM3

The Acer G43T-AM3 is a microATX-sized desktop board. It was used for the
Acer models Aspire M3800, Aspire M5800 and possibly more.

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | Intel G43 (called x4x in coreboot code)          |
+------------------+--------------------------------------------------+
| Southbridge      | Intel ICH10R (called i82801jx in coreboot code)  |
+------------------+--------------------------------------------------+
| CPU socket       | LGA 775                                          |
+------------------+--------------------------------------------------+
| RAM              | 4 x DDR3-1066                                    |
+------------------+--------------------------------------------------+
| SuperIO          | ITE IT8720F                                      |
+------------------+--------------------------------------------------+
| Audio            | Realtek ALC888S                                  |
+------------------+--------------------------------------------------+
| Network          | Intel 82567V-2 Gigabit Ethernet                  |
+------------------+--------------------------------------------------+
```

There is no serial port. Serial console output is possible by soldering
to a point at the corresponding Super I/O pin and patching the
mainboard-specific code accordingly.

## Status

### Working

Tests were done with SeaBIOS 1.14.0 and slackware64-live from 2019-07-12
(linux-4.19.50).

+ Intel Core 2 processors at up to FSB 1333
+ All four DIMM slots at 1066 MHz (tested 2x2GB + 2x4GB)
+ Integrated graphics (libgfxinit)
+ HDMI and VGA ports
+ Both PCI slots
+ Both PCI-e slots
+ USB (8 internal, 4 external)
+ All six SATA ports
+ Onboard Ethernet
+ Onboard sound card with output on the rear stereo connector
+ PS/2 mouse and keyboard
    + With SeaBIOS, use CONFIG_SEABIOS_PS2_TIMEOUT, tested: 500
    + With FILO it works without further settings
+ Temperature readings from the Super I/O (including the CPU temperature
  via PECI)
+ Super I/O EC automatic fan control
+ S3 suspend/resume
+ Poweroff

### Not working

+ DDR3 memory with 512Mx8 chips (G43 limitation)
+ 4x4GB of DDR3 memory (works, but showed a single bit error within one
  pass of Memtest86+ 5.01)
+ Super I/O voltage reading conversions

### Untested

+ Other audio jacks or the front panel header
+ S/PDIF output
+ On-board Firewire
+ Wake-on-LAN

## Flashing coreboot

```eval_rst
+-------------------+---------------------+
| Type              | Value               |
+===================+=====================+
| Socketed flash    | No                  |
+-------------------+---------------------+
| Model             | Macronix MX25L1605D |
+-------------------+---------------------+
| Size              | 2 MiB               |
+-------------------+---------------------+
| Package           | 8-Pin SOP           |
+-------------------+---------------------+
| Write protection  | No                  |
+-------------------+---------------------+
| Dual BIOS feature | No                  |
+-------------------+---------------------+
| Internal flashing | Yes                 |
+-------------------+---------------------+
```

The flash is divided into the following regions, as obtained with
`ifdtool -f rom.layout backup.rom`:
```
00000000:00001fff fd
00100000:001fffff bios
00006000:000fffff me
00002000:00005fff gbe
```

In general, flashing is possible internally and from an external header. It
might be necessary to specify the chip type; `MX25L1605D/MX25L1608D/MX25L1673E`
is the correct one, not `MX25L1605`.

### Internal flashing

Internal access to the flash chip is unrestricted. When installing coreboot,
only the BIOS region should be updated by passing the `--ifd` and `-i bios`
parameters to flashrom. A full backup is advisable.

Here is an example:

```
$ sudo flashrom \
  -p internal \
  -c "MX25L1605D/MX25L1608D/MX25L1673E" \
  -r backup.rom
$ sudo flashrom \
  -p internal \
  -c "MX25L1605D/MX25L1608D/MX25L1673E" \
  --ifd -i bios \
  -w coreboot.rom
```

```eval_rst
In addition to the information here, please see the
:doc:`../../tutorial/flashing_firmware/index`.
```

### External flashing

The SPI flash chip on this board can be flashed externally through the
SPI_ROM1 header while the board is off and disconnected from power. There
seems to be a diode that prevents the external programmer from powering the
whole board.

The signal assigment on the header is identical to the pinout of the flash
chip. The pinout diagram below is valid when the PCI slots are on the left
and the CPU is on the right. Note that HOLD# and WP# must be pulled high
(to VCC) to be able to flash the chip.

                +---+---+
     SPI_CSn <- | x | x | -> VCC
                +---+---+
    SPI_MISO <- | x | x | -> HOLDn
                +---+---+
         WPn <- | x | x | -> SPI_CLK
                +---+---+
         GND <- | x | x | -> SPI_MOSI
                +---+---+

## Intel Management Engine

The Intel Management Engine (ME) can be disabled by setting the ME_DISABLE
jumper on the board. It pulls GPIO33 on the ICH10 low, causing the "Flash
Descriptor Security Override Strap" to be set. This disables the ME and also
disables any read/write restrictions to the flash chip that may be set in the
Intel Flash Descriptor (IFD) (none on this board). Note that changing this
jumper only comes into effect when starting the board from a shutdown or
suspend state, not during normal operation.

To completely remove the ME blob from the flash image and to decrease the size
of the ME region, thus increasing the size of the BIOS region, `me_cleaner` can
be used with the `-t`, `-r` and `-S` options.

## Fan control

There are two fan connectors that can be controlled individually. CPU_FAN
can only control a fan by a PWM signal and SYS_FAN only by voltage. See
the mainboard's `devicetree.cb` file for how coreboot configures the Super
I/O to control the fans.

## Variants

Various similar mainboards exist, like the Acer Q45T-AM. During a discussion
in #coreboot on IRC, ECS was suspected to be the original designer of this
series of mainboards. They have similar models such as the ECS G43T-WM.
