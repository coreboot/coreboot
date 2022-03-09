# ASRock H81M-HDS

This page describes how to run coreboot on the [ASRock H81M-HDS].

## Required proprietary blobs

```eval_rst
Please see :doc:`../../northbridge/intel/haswell/mrc.bin`.
```

## Building coreboot

A fully working image should be possible just by setting your MAC
address and obtaining the Haswell mrc. You can set the basic config
with the following commands. However, it is strongly advised to use
`make menuconfig` afterwards (or instead), so that you can see all of
the settings.

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_ASROCK
./util/scripts/config --enable BOARD_ASROCK_H81M_HDS
./util/scripts/config --enable HAVE_MRC
./util/scripts/config --set-str REALTEK_8168_MACADDRESS "xx:xx:xx:xx:xx:xx" # Fill this in!
make olddefconfig
```

If you don't plan on using coreboot's serial console to collect logs,
you might want to disable it at this point (`./util/scripts/config
--disable CONSOLE_SERIAL`). It should reduce the boot time by several
seconds. However, a more flexible method is to change the console log
level from within an OS using `util/nvramtool`, or with the `nvramcui`
payload.

Now, run `make` to build the coreboot image.

## Flashing coreboot

### Internal programming

The main SPI flash can be accessed using [flashrom]. By default, only
the BIOS region of the flash is writable. If you wish to change any
other region, such as the Management Engine or firmware descriptor, then
an external programmer is required (unless you find a clever way around
the flash protection).

The following command may be used to flash coreboot:

```bash
sudo flashrom -p internal --ifd -i bios --noverify-all -w coreboot.rom
```

The use of `--noverify-all` is required since the Management Engine
region is not readable even by the host.

### External programming

The flash chip is a 4 MiB socketed DIP-8 chip. Specifically, it's a
Winbond W25Q32FV, whose datasheet can be found [here][W25Q32FV].
The chip is located to the bottom right-hand side of the board. For
a precise location, refer to section 1.4 (Motherboard Layout) of the
[board manual], where the chip is labelled "32Mb BIOS". Take note of
the chip's orientation, remove it from its socket, and flash it with
an external programmer. For reference, the notch in the chip should be
facing towards the bottom of the board.

## Known issues

- The VGA port doesn't work until the OS reinitialises the display.

- There is no automatic, OS-independent fan control. This is because
  the Super I/O hardware monitor can only obtain valid CPU temperature
  readings from the PECI agent, but the required driver doesn't exist
  in coreboot. The `coretemp` driver can still be used for accurate CPU
  temperature readings from an OS.

```eval_rst
Please also see :doc:`../../northbridge/intel/haswell/known-issues`.
```

## Untested

- parallel port
- PS/2 keyboard
- EHCI debug
- TPM
- infrared module
- chassis intrusion header
- chassis speaker header

## Working

- USB
- S3 suspend/resume
- Gigabit Ethernet
- integrated graphics
- PCIe
- SATA
- PS/2 mouse
- serial port
- hardware monitor (see [Known issues](#known-issues))
- onboard audio
- front panel audio
- initialisation with Haswell mrc version 1.6.1 build 2
- graphics init with libgfxinit (see [Known issues](#known-issues))
- flashrom under the vendor firmware
- flashrom under coreboot
- Wake-on-LAN
- Using `me_cleaner`

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/haswell/index`     |
+------------------+--------------------------------------------------+
| Southbridge      | Intel Lynx Point (H81)                           |
+------------------+--------------------------------------------------+
| CPU              | Intel Haswell (LGA1150)                          |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6776                                  |
+------------------+--------------------------------------------------+
| EC               | None                                             |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

[ASRock H81M-HDS]: https://www.asrock.com/mb/Intel/H81M-HDS/
[W25Q32FV]: https://www.winbond.com/resource-files/w25q32fv%20revi%2010202015.pdf
[flashrom]: https://flashrom.org/Flashrom
[Board manual]: https://web.archive.org/web/20191231093418/http://asrock.pc.cdn.bitgravity.com/Manual/H81M-HDS.pdf
