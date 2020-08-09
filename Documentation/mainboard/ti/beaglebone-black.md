# Beaglebone Black
This page gives some details about the [BeagleBone Black] coreboot port and
describes how to build and run it.

The port currently only supports booting coreboot from a micro SD card and has
some other limitations listed below.

## Supported Boards
The Beaglebone port supports the following boards:

- Beaglebone Black
- Beaglebone Black Wireless
- Beaglebone Pocket (untested, may need tweaking)
- Beaglebone Blue (untested, may need tweaking)
- Beaglebone Original (untested, may need tweaking)

## Use Cases
This port was primarily developed as a learning exercise and there is
potentially little reason to use it compared to the defacto bootloader choice of
U-Boot. However, it does have some interesting practical use cases compared to
U-Boot:

1. Choosing coreboot as a lightweight alternative to U-Boot. In this case,
   coreboot is used to do the absolute minimum necessary to boot Linux, forgoing
   some U-Boot features and functionality. Complex boot logic can then instead
   be moved into Linux where it can be more flexibly and safely executed. This
   is essentially the LinuxBoot philosophy. [U-Boot Falcon mode] has similar
   goals to this as well.
2. Facilitating experimenting with coreboot on real hardware. The Beaglebone
   Black is widely available at a low pricepoint (~$65) making it a great way to
   experiment with coreboot on real ARMv7 hardware. It also works well as a
   development platform as it has exposed pads for JTAG and, due to the way it
   boots, is effectively impossible to brick.
3. The Beaglebone Black is often used as a external flasher and EHCI debug
   gadget in the coreboot community, so many members have access to it and can
   use it as a reference platform.

## Quickstart
1. Run `make menuconfig` and select _TI_/_Beaglebone_ in the _Mainboard_ menu.
2. Add a payload as normal.
3. Run `make`.
4. Copy the resulting `build/MLO` file to the micro SD card at offset 128k - ie
   `dd if=build/MLO of=/dev/sdcard seek=1 bs=128k`.

**NOTE**: By default, the Beaglebone is configured to try to boot first from
eMMC before booting from SD card. To ensure that the Beaglebone boots from SD,
either erase the internal eMMC or hold the _S2_ button while powering on (note
that this has to be while powering on - ie when plugging in the USB or DC barrel
jack - the boot order doesn't change on reset) to prioritize SD in the boot
order.

## Serial Console
By default, coreboot uses UART0 as the serial console. UART0 is available
through the J1 header on both the Beaglebone Black and Beaglebone Black
Wireless. The serial runs at 3.3V and 115200 8n1.

The pin mapping is shown below for J1.

    ```eval_rst
    +----------------------------+------------+
    | Pin number                 | Function   |
    +============================+============+
    | 1 (Closest to barrel jack) | GND        |
    +----------------------------+------------+
    | 4                          | RX         |
    +----------------------------+------------+
    | 5                          | TX         |
    +----------------------------+------------+
    ```

## Boot Process
The AM335x contains ROM code to allow booting in a number of different
configurations. More information about the boot ROM code can be found in the
AM335x technical reference manual (_SPRUH73Q_) in the _Initialization_ section.

This coreboot port is currently configured to boot in "SD Raw Mode" where the
boot binary, with header ("Table of Contents" in TI's nomenclature), is placed
at the offset of 0x20000 (128KB) on the SD card. The boot ROM loads the coreboot
bootblock stage into SRAM and executes it.

The bootblock and subsequent romstage and ramstage coreboot stages expect that
the coreboot image, containing the CBFS, is located at 0x20000 on the SD card.
All stages directly read from the SD card in order to load the next stage in
sequence.

## Clock Initialization and PMIC
To simplify the port, the TPS65217C Power Management IC (PMIC) on the Beaglebone
Black is not configured by coreboot. By default, the PMIC reset values for
VDD_MPU (1.1V) and VDD_CORE (1.8V) are within the Operating Performance Point
(OPP) for the MPU PLL configuration set by the boot ROM of 500 MHz.

When using Linux as a payload, the kernel will appropriately scale the core
voltages for the desired MPU clock frequency as defined in the device tree.

One significant difference because of this to the U-Boot port is that the DCDC1
rail that powers the DDR3 RAM will be 1.5V by default. The Micron DDR3 supports
both 1.35V and 1.5V and U-Boot makes use of this by setting it to 1.35V to
conserve power. Fortunately, Linux is again able to configure this rail but it
involves adding an entry to the device tree:

    &dcdc1_reg {
    regulator-name = "vdd_ddr3";
    regulator-min-microvolt = <1350000>;
    regulator-max-microvolt = <1350000>;
    regulator-boot-on;
    regulator-always-on;
    };

If this port was to be extended to work with boards or SoCs with different
requirements for the MPU clock frequency or different Operating Performance
Points, then the port may need to be extended to set the core voltages and MPU
PLL within coreboot, prior to loading a payload. Extending coreboot so that it
can configure the PMIC would also be necessary if there was a requirement for
coreboot to run at a different MPU frequency than the 500 MHz set by the boot
ROM.

# Todo
- Allow coreboot to run from the Beaglebone Black's internal eMMC. This would
  require updating the `mmc.c` driver to support running from both SD and eMMC.
- Support the boot ROMs *FAT mode* so that the coreboot binary can be placed on
  a FAT partition.
- Increase the MMC read speed, it currently takes ~15s to read ~20MB which is a
  bit slow. To do this, it should be possible to update the MMC driver to:
    - Increase the supported blocksize (currently is always set to 1)
    - Support 4-bit data width (currently only supports 1-bit data width)
- Convert the while loops in the MMC driver to timeout so that coreboot does not
  hang on a bad SD card or when the SD card is removed during boot.


[Beaglebone Black]: https://beagleboard.org/black [U-Boot Falcon mode]:
https://elixir.bootlin.com/u-boot/v2020.07/source/doc/README.falcon