# ASRock H77 Pro4-M

The ASRock H77 Pro4-M is a microATX-sized desktop board for Intel Sandy
Bridge and Ivy Bridge CPUs.

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | Intel H77 (bd82x6x)                              |
+------------------+--------------------------------------------------+
| CPU socket       | LGA 1155                                         |
+------------------+--------------------------------------------------+
| RAM              | 4 x DDR3-1600                                    |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6776                                  |
+------------------+--------------------------------------------------+
| Audio            | Realtek ALC892                                   |
+------------------+--------------------------------------------------+
| Network          | Realtek RTL8111E                                 |
+------------------+--------------------------------------------------+
| Serial           | Internal header (RS-232)                         |
+------------------+--------------------------------------------------+
```

## Status

Tests were done with SeaBIOS 1.14.0 and slackware64-live from 2019-07-12
(linux-4.19.50).

### Working

- Sandy Bridge and Ivy Bridge CPUs (tested: i5-2500, Pentium G2120)
- Native RAM initialization with four DIMMs
- PS/2 combined port (mouse or keyboard)
- Integrated GPU by libgfxinit on all monitor ports (DVI-D, HDMI, D-Sub)
- PCIe graphics in the PEG slot
- All three additional PCIe slots
- All rear and internal USB2 ports
- All rear and internal USB3 ports
- All six SATA ports from the PCH (two 6 Gb/s, four 3 Gb/s)
- All two SATA ports from the ASM1061 PCIe-to-SATA bridge (6 Gb/s)
- Rear eSATA connector (multiplexed with one ASM1061 port)
- Gigabit Ethernet
- Console output on the serial port
- SeaBIOS 1.14.0 and 1.15.0 to boot Windows 10 (needs VGA BIOS) and Linux via
extlinux
- Internal flashing with flashrom-1.2, see
[Internal Programming](#internal-programming)
- External flashing with flashrom-1.2 and a Raspberry Pi 1
- S3 suspend/resume from either Linux or Windows 10
- Poweroff

### Not working

- Booting from the two SATA ports provided by the ASM1061
- Automatic fan control with the NCT6776D Super I/O

### Untested

- EHCI debug
- S/PDIF audio
- Other audio jacks than the green one, and the front panel header
- Parallel port
- Infrared/CIR
- Wakeup from anything but the power button

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | yes        |
+---------------------+------------+
| Model               | W25Q64.V   |
+---------------------+------------+
| Size                | 8 MiB      |
+---------------------+------------+
| Package             | DIP-8      |
+---------------------+------------+
| Write protection    | no         |
+---------------------+------------+
| Dual BIOS feature   | no         |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
```

The flash is divided into the following regions, as obtained with
`ifdtool -f rom.layout backup.rom`:
```
00000000:00000fff fd
00200000:007fffff bios
00001000:001fffff me
```

### Internal programming

The main SPI flash can be accessed using flashrom. By default, only
the BIOS region of the flash is writable. If you wish to change any
other region (Management Engine or flash descriptor), then an external
programmer is required.

The following command may be used to flash coreboot:

```
$ sudo flashrom --noverify-all --ifd -i bios -p internal -w coreboot.rom
```

The use of `--noverify-all` is required since the Management Engine
region is not readable even by the host.

```eval_rst
In addition to the information here, please see the
:doc:`../../tutorial/flashing_firmware/index`.
```

## Hardware monitoring and fan control

There are two fan headers for the CPU cooler, CPU_FAN1 and CPU_FAN2. They share
a single fan tachometer input on the Super I/O while some dedicated logic
selects which one is allowed to reach it. Two GPIO pins on the Super I/O are
used to control that logic. The firmware has to set them; coreboot selects
CPU_FAN1 by default, but the user can change that setting if it was built with
CONFIG_USE_OPTION_TABLE:

```
$ sudo nvramtool -e cpu_fan_header
[..]
$ sudo nvramtool -w cpu_fan_header=CPU_FAN2
$ sudo nvramtool -w cpu_fan_header=None
$ sudo nvramtool -w cpu_fan_header=Both
```

The setting will take effect after a reboot. Selecting and connecting both fan
headers is possible but the Super I/O will report wrong fan speeds.

Currently there is no automatic, OS-independent fan control, but a software
like `fancontrol` from the lm-sensors package can be used instead.

## Serial port header

Serial port 1, provided by the Super I/O, is exposed on a pin header. The
RS-232 signals are assigned to the header so that its pin numbers map directly
to the pin numbers of a DE-9 connector. If your serial port doesn't seem to
work, check if your bracket expects a different assignment. Also don't try to
connect it directly to a device that operates at TTL levels - it would need a
level converter like a MAX232.

Here is a top view of the serial port header found on this board:

                 +---+---+
             N/C |   | 9 | RI  -> pin 9
                 +---+---+
    Pin 8 <- CTS | 8 | 7 | RTS -> pin 7
                 +---+---+
    Pin 6 <- DSR | 6 | 5 | GND -> pin 5
                 +---+---+
    Pin 4 <- DTR | 4 | 3 | TxD -> pin 3
                 +---+---+
    Pin 2 <- RxD | 2 | 1 | DCD -> pin 1
                 +---+---+

## eSATA

The eSATA port on the rear I/O panel and the internal connector SATA3_A1 share
the same controller port on the ASM1061. Attaching an eSATA drive causes a
multiplexer chip to disconnect the internal port from the SATA controller and
connect the eSATA port instead. This can be seen on GP23 of the Super I/O
GPIOs: it is '0' when something is connected to the eSATA port and '1'
otherwise.
