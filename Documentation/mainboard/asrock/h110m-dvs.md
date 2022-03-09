# ASRock H110M-DVS

This page describes how to run coreboot on the [ASRock H110M-DVS].

## Required proprietary blobs

Mainboard is based on Intel Skylake/Kaby Lake processor and H110 Chipset.
Intel company provides [Firmware Support Package (2.0)](../../soc/intel/fsp/index.md)
(intel FSP 2.0) to initialize this generation silicon. Please see this
[document](../../soc/intel/code_development_model/code_development_model.md).

FSP Information:

```eval_rst
+-----------------------------+-------------------+-------------------+
| FSP Project Name            | Directory         | Specification     |
+-----------------------------+-------------------+-------------------+
| 7th Generation Intel® Core™ | KabylakeFspBinPkg | 2.0               |
| processors  and chipsets    |                   |                   |
| (formerly Kaby Lake)        |                   |                   |
+-----------------------------+-------------------+-------------------+
```

## Building coreboot

The following steps set the default parameters for this board to build a
fully working image:

```bash
make distclean
touch .config
./util/scripts/config --enable VENDOR_ASROCK
./util/scripts/config --enable BOARD_ASROCK_H110M_DVS
./util/scripts/config --set-str REALTEK_8168_MACADDRESS "xx:xx:xx:xx:xx:xx"
make olddefconfig
```

However, it is strongly advised to use `make menuconfig` afterwards
(or instead), so that you can see all of the settings.

Use the following command to disable the serial console if debugging
output is not required:

```bash
./util/scripts/config --disable CONSOLE_SERIAL
```

However, a more flexible method is to change the console log level from
within an OS using `util/nvramtool`, or with the `nvramcui` payload.

Now, run `make` to build the coreboot image.

## Flashing coreboot

### Internal programming

The main SPI flash can be accessed using [flashrom]. By default, only
the BIOS region of the flash is writable. If you wish to change any
other region, such as the Management Engine or firmware descriptor, then
an external programmer is required (unless you find a clever way around
the flash protection). More information about this [here](../../tutorial/flashing_firmware/index.md).

### External programming

The flash chip is a 8 MiB socketed DIP-8 chip. Specifically, it's a
Macronix MX25L6473E, whose datasheet can be found [here][MX25L6473E].
The chip is located to the bottom right-hand side of the board. For
a precise location, refer to section 1.3 (Motherboard Layout) of the
[H110M-DVS manual], where the chip is labelled "64Mb BIOS". Take note of
the chip's orientation, remove it from its socket, and flash it with
an external programmer. For reference, the notch in the chip should be
facing towards the bottom of the board.

## Known issues

- The VGA port doesn't work. Discrete graphic card is used as primary
  device for display output (if CONFIG_ONBOARD_VGA_IS_PRIMARY is not
  set). Dynamic switching between iGPU and PEG is not yet supported.

- SuperIO GPIO pin is used to reset Realtek chip. However, since the
  Logical Device 7 (GPIO6, GPIO7, GPIO8) is not initialized, the network
  chip is in a reset state all the time.

## Untested

- parallel port
- PS/2 keyboard
- PS/2 mouse
- EHCI debug
- TPM
- infrared module
- chassis intrusion header
- chassis speaker header

## Working

- integrated graphics init with libgfxinit (see [Known issues](#known-issues))
- PCIe x1
- PEG x16 Gen3
- SATA
- USB
- serial port
- onboard audio
- using `me_cleaner`
- using `flashrom`

## TODO

- NCT6791D GPIOs
- onboard network (see [Known issues](#known-issues))
- S3 suspend/resume
- Wake-on-LAN
- hardware monitor

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| CPU              | Intel Skylake/Kaby Lake (LGA1151)                |
+------------------+--------------------------------------------------+
| PCH              | Intel Sunrise Point H110                         |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6791D                                 |
+------------------+--------------------------------------------------+
| EC               | None                                             |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

[ASRock H110M-DVS]: https://www.asrock.com/mb/Intel/H110M-DVS%20R2.0/
[MX25L6473E]: http://www.macronix.com/Lists/Datasheet/Attachments/7380/MX25L6473E,%203V,%2064Mb,%20v1.4.pdf
[flashrom]: https://flashrom.org/Flashrom
[H110M-DVS manual]: https://web.archive.org/web/20191023230631/http://asrock.pc.cdn.bitgravity.com/Manual/H110M-DVS%20R2.0.pdf
