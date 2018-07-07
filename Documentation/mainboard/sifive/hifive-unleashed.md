# SiFive HiFive Unleashed

This page describes how to run coreboot on the [HiFive Unleashed] development
board from [SiFive], the first RISC-V board on the market with enough resources
to run a multiuser operating system.

For general setup instructions, please refer to the [Getting Started Guide].


## TODO

The following things are still missing from this coreboot port:

- Support running romstage from flash (fix stack) to support boot mode 1
- CBMEM support
- FU540 clock configuration
- FU540 RAM init
- Placing the ramstage in DRAM
- Starting the U54 cores
- FU540 PIN configuration and GPIO access macros
- Provide serial number to payload (e.g. in device tree)
- Support for booting Linux on RISC-V


## Configuration

Run `make menuconfig` and select _SiFive_/_HiFive Unleashed_ in the _Mainboard_
menu.


### Boot modes

A total of 16 boot modes can be configured using the switches labeled `MSEL0`
through `MSEL3`. The most important ones are as follows:

- **MSEL=1**: Jump directly into the SPI flash, bypassing ROM1
- **MSEL=11**: Load FSBL from SD-card
- **MSEL=15**: Default boot mode; Load FSBL/coreboot from a GPT partition on
  SPI flash


## Flashing coreboot

The HiFive Unleashed has an 32 MiB SPI flash (**ISSI IS25WP256D**), that can be
programmed from within Linux running on the board, via USB/JTAG, or directly
with an SPI programmer.

### Internal programming

The SPI flash can be accessed as `/dev/mtd0` from Linux.

### USB/JTAG

To program the flash via USB/JTAG, connect the USB port to a computer. If the
board is powered on, two new serial ports, for example `/dev/ttyUSB0` and
`/dev/ttyUSB1` will appear. The first is JTAG, and the second is connected to
the SoC's UART.

- Download and build the [RISC-V fork of OpenOCD].
- Download the [OpenOCD script] for Freedom Unleashed.
- Start OpenOCD with `openocd -f openocd.cfg`
- Connect to OpenOCD's command interface (via telnet) and enter the line
  marked with `> `:
```
> flash write_image erase unlock build/coreboot.rom 0x20000000
auto erase enabled
auto unlock enabled
wrote 33554432 bytes from file build/coreboot.rom in 1524.943848s (21.488 KiB/s)
```
  Note that programming the whole flash with OpenOCD isn't fast. In this
  example it took just over 25 minutes. This process can be sped up
  considerably by building/flashing a smaller image; OpenOCD does not check if
  the image and the flash have the same size.


### External programming

External programming with an SPI adapter and [flashrom] may work, but has not
been tested. Please study the [schematics] before going this route.


## Error codes

The zeroth-stage bootloader (ZSBL) in ROM1 can print error codes on the serial
console in certain situations.

```
// Error codes are formatted as follows:
// [63:60]    [59:56]  [55:0]
// bootstage  trap     errorcode
// If trap == 1, then errorcode is actually the mcause register with the
// interrupt bit shifted to bit 55.
```
(--- from the [SiFive forum](https://forums.sifive.com/t/loading-fsbl-from-sd/1156/4))


[HiFive Unleashed]: https://www.crowdsupply.com/sifive/hifive-unleashed
[SiFive]: https://www.sifive.com/
[Getting Started Guide]: https://www.sifive.com/documentation/boards/hifive-unleashed/hifive-unleashed-getting-started-guide/
[RISC-V fork of OpenOCD]: https://github.com/riscv/riscv-openocd
[OpenOCD script]: https://github.com/sifive/freedom-u-sdk/blob/057a47f657fa33e2c60df7f183884a68e90381cc/bsp/env/freedom-u500-unleashed/openocd.cfg
[flashrom]: https://flashrom.org/Flashrom
[schematics]: https://www.sifive.com/documentation/boards/hifive-unleashed/hifive-unleashed-schematics/
