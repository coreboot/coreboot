# Intel DQ67SW

The Intel DQ67SW is a microATX-sized desktop board for Intel Sandy Bridge CPUs.

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | Intel Q67 (bd82x6x)                              |
+------------------+--------------------------------------------------+
| CPU socket       | LGA 1155                                         |
+------------------+--------------------------------------------------+
| RAM              | 4 x DDR3-1333                                    |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton/Winbond W83677HG-i                       |
+------------------+--------------------------------------------------+
| Audio            | Realtek ALC888S                                  |
+------------------+--------------------------------------------------+
| Network          | Intel 82579LM Gigabit Ethernet                   |
+------------------+--------------------------------------------------+
| Serial           | Internal header                                  |
+------------------+--------------------------------------------------+
```

## Status

### Working

- Sandy Bridge and Ivy Bridge CPUs (tested: i5-2500, Pentium G2120)
- Native RAM initialization with four DIMMs
- Integrated GPU with libgfxinit
- PCIe graphics in the PEG slot
- Additional PCIe slots
- PCI slot
- All rear (4x) and internal (8x) USB2 ports
- Rear USB3 ports (2x)
- All four internal SATA ports (two 6 Gb/s, two 3 Gb/s)
- Two rear eSATA connectors (3 Gb/s)
- SATA at 6 Gb/s
- Gigabit Ethernet
- SeaBIOS 1.16.1 + libgfxinit (legacy VGA) to boot slackware64 (Linux 5.15)
- SeaBIOS 1.16.1 + extracted VGA BIOS to boot Windows 10 (21H2)
- edk2 UefiPayload (uefipayload_202207) + libgfxinit (high-res) to boot:
    - slackware64 (Linux 5.15)
    - Windows 10 (22H2)
- External in-circuit flashing with flashrom-1.2 and a Raspberry Pi 1
- Poweroff
- Resume from S3
- Console output on the serial port

### Not working

- Automatic fan control. One can still use OS-based fan control programs,
  such as fancontrol on Linux or SpeedFan on Windows.
- Windows 10 booted from SeaBIOS + libgfxinit (high-res). The installation
  works, but once Windows Update installs drivers, it crashes and enters a
  bootloop.

### Untested

- Firewire (LSI L-FW3227-100)
- EHCI debug
- S/PDIF audio
- Audio jacks other than the green one

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Model               | W25Q64.V   |
+---------------------+------------+
| Size                | 8 MiB      |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Write protection    | yes        |
+---------------------+------------+
| Dual BIOS feature   | no         |
+---------------------+------------+
| Internal flashing   | see below  |
+---------------------+------------+
| In circuit flashing | see below  |
+---------------------+------------+
```

The flash is divided into the following regions, as obtained with
`ifdtool -f rom.layout backup.rom`:

    00000000:00000fff fd
    00580000:007fffff bios
    00003000:0057ffff me
    00001000:00002fff gbe

Unfortunately the SPI interface to the chip is locked down by the vendor
firmware. The BIOS Lock Enable (BLE) bit of the `BIOS_CNTL` register, part of
the PCI configuration space of the LPC Interface Bridge, is set.

It is possible to program the chip is to attach an external programmer
with an SOIC-8 clip.

Another way is to boot the vendor firmware in UEFI mode and exploit the
unpatched S3 Boot Script vulnerability. See this page for a similar procedure:
<project:../lenovo/ivb_internal_flashing.md>.

On this specific board it is possible to prevent the BLE bit from being set
when it resumes from S3. One entry in the S3 Boot Script must be modified,
e.g. with a patched version of [CHIPSEC](https://github.com/chipsec/chipsec)
that supports this specific type of S3 Boot Script, for example from strobo5:

    $ git clone -b headerless https://github.com/strobo5/chipsec.git
    $ cd chipsec
    $ python setup.py build_ext -i
    $ sudo python chipsec_main.py -m tools.uefi.s3script_modify -a replace_op,mmio_wr,0xe00f80dc,0x00,1

The boot script contains an entry that writes 0x02 to memory at address
0xe00f80dc. This address points at the PCIe configuration register at offset
0xdc for the PCIe device 0:1f.0, which is the BIOS Control Register of the LPC
Interface Bridge [0][1]. The value 0x02 sets the BLE bit, and the modification
prevents this by making it write a 0 instead.

After suspending and resuming the board, the BIOS region can be flashed with
a coreboot image, e.g. using flashrom. Note that the ME region is not readable,
so the `--noverify-all` flag is necessary. Please refer to the
<project:../../tutorial/flashing_firmware/index.md>.

## Hardware monitoring and fan control

Currently there is no automatic, OS-independent fan control.

## Serial port header

Serial port 1, provided by the Super I/O, is exposed on a pin header. The
RS-232 signals are assigned to the header so that its pin numbers map directly
to the pin numbers of a DE-9 connector. If your serial port doesn't seem to
work, check if your bracket expects a different assignment.

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

## References

[0]: Intel 6 Series Chipset and Intel C200 Series Chipset Datasheet,
May 2011,
Document number 324645-006

[1]: Accessing PCI Express Configuration Registers Using Intel Chipsets,
December 2008,
Document number 321090

