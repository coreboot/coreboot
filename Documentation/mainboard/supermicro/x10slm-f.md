# Supermicro X10SLM+-F

This section details how to run coreboot on the [Supermicro X10SLM+-F].

## Required proprietary blobs

```eval_rst
Please see :doc:`../../northbridge/intel/haswell/mrc.bin`.
```

## Building coreboot

```eval_rst
If you haven't already, build the coreboot toolchain as described in
:doc:`../../tutorial/part1`.
```

A fully working image should be possible so long as you have the
Haswell `mrc.bin` file. You can set the basic config with the following
commands. However, it is strongly advised to use `make menuconfig`
afterwards (or instead), so that you can see all of the settings.

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_SUPERMICRO
./util/scripts/config --enable BOARD_SUPERMICRO_X10SLM_PLUS_F
./util/scripts/config --enable HAVE_MRC
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

```eval_rst
In addition to the information here, please see the
:doc:`../../tutorial/flashing_firmware/index`.
```

### Internal programming

Under the vendor firmware, the BIOS region of the flash chip is
write-protected. Additionally, the vendor flashing tool does not work
with a coreboot image. So, [external programming](#external-programming)
needs to be used when first installing coreboot. By default, coreboot is
not configured to write-protect the BIOS region, so internal programming
can be used thereafter.

[flashrom] may be used to flash coreboot internally:

```bash
sudo flashrom -p internal --ifd -i bios --noverify-all -w coreboot.rom
```

The use of `--noverify-all` is required since the Management Engine
region is not readable even by the host.

### External programming

The main firmware flash chip is an SOIC-8 package located near the CMOS
battery and SATA ports. It should come with a sticker attached that
states the firmware revision (e.g. "X10SLH 4.424"). The chip model is
an N25Q128A ([datasheet][N25Q128A]).

As with [internal programming](#internal-programming), [flashrom] works
reliably:

```bash
flashrom -p <your-programmer> --ifd -i bios -w coreboot.rom
```

For flashing to work, power to the board should be disconnected (ACPI
G3), and power should be supplied from the external programmer. There is
a diode attached to Vcc, so such flashing should not damage the board.
During testing, a single X10SLM+-F has been flashed dozens of times this
way without issue.

## BMC (IPMI)

This board has an ASPEED [AST2400], which has BMC functionality. The
BMC firmware resides in a 32 MiB SOIC-16 chip just above the [AST2400].
This chip is an MX25L25635F ([datasheet][MX25L25635F]).

### Removing the BMC functionality

The BMC functionality on this board can be removed. If you do not need
its features, removing the BMC functionality might increase security.
This topic has not been widely explored, and you should only **undertake
this process at your own risk.**

There is a jumper labelled `JPB1` on the board that states the ability
to disable the BMC. Though, pins 1 and 2 are fixed together, keeping
the BMC enabled. It might be possible to disable the BMC by cutting the
connection between pins 1 and 2 (and then connecting pins 2 and 3). This
has not been tested so far.

Another approach is to erase the entire BMC firmware chip. However, if
this is done, and the board's power cycled, the voltage changes on some
pins of the flash chip, **so it will be harder to flash it again!**

To remove the firmware, connect an external programmer to the BMC
firmware chip. Vcc should **not** be connected via the external
programmer. The system should be turned off, but the power still
connected (ACPI S5). Then, erase the chip with [flashrom]. Power cycle
the board, and the BMC should no longer be active.

If you erase the BMC firmware while using the **vendor BIOS**, you
will need to cut the connection between pins 1 and 2 of `JPB1`. The
system will stall for two minutes each time when booting, but it will
eventually start. There is no such delay when running coreboot.

## ECC DRAM

```eval_rst
ECC DRAM seems to work, but please see
:doc:`../../northbridge/intel/haswell/mrc.bin`
for caveats.
```

## Known issues

- Broadwell CPUs are not supported. They might work with minimal changes
  to the code, but this has not been tested.

- The PCH thermal sensor doesn't yet have a driver in coreboot, so it
  can't be used for temperature readings.

- There is no automatic, OS-independent fan control. This is because
  the Super I/O hardware monitor can only obtain valid CPU temperature
  readings from the PECI agent, but the required driver doesn't exist
  in coreboot. The `coretemp` driver can still be used for accurate CPU
  temperature readings from an OS, and hence the OS can do fan control.

```eval_rst
Please also see :doc:`../../northbridge/intel/haswell/known-issues`.
```

## Untested

- TPM
- PCIe (likely to work, but maybe not at Gen 3 speeds)
- BMC (IPMI) functionality
- internal serial port
- chassis intrusion header
- SATA DOM header
- standby power header
- serial GPIO headers
- power supply SMBus header
- jumpers not otherwise mentioned
- LEDs

## Working

- USB
- S3 suspend/resume
- Gigabit Ethernet
- SATA
- external serial port
- VGA graphics
- disabling VGA graphics using the jumper
- hiding the AST2400 using the CMOS setting
- Super I/O hardware monitor (see [Known issues](#known-issues))
- initialisation with Haswell MRC version 1.6.1 build 2
- flashrom under coreboot
- Wake-on-LAN
- front panel header
- internal buzzer

## Technology

```eval_rst
+------------------+--------------------------------------------------+
| CPU              | :doc:`../../northbridge/intel/haswell/index`     |
+------------------+--------------------------------------------------+
| PCH              | Intel Lynx Point (C224)                          |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6776                                  |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel SPS (server version of the ME)             |
+------------------+--------------------------------------------------+
| Coprocessor      | ASPEED AST2400                                   |
+------------------+--------------------------------------------------+
```

## Extra links

- [Board manual]

[AST2400]: https://www.aspeedtech.com/products.php?fPath=20&rId=376
[Board manual]: https://www.supermicro.com/manuals/motherboard/C224/MNL-1500.pdf
[flashrom]: https://flashrom.org/Flashrom
[MX25L25635F]: https://media.digikey.com/pdf/Data%20Sheets/Macronix/MX25L25635F.pdf
[N25Q128A]: https://www.micron.com/~/media/Documents/Products/Data%20Sheet/NOR%20Flash/Serial%20NOR/N25Q/n25q_128mb_3v_65nm.pdf
[Supermicro X10SLM+-F]: https://www.supermicro.com/products/motherboard/xeon/c220/x10slm_-f.cfm
