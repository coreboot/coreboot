# Kontron mAL10 Computer-on-Modules platform

The Kontron [mAL10] COMe is a credit card sized Computer-on-Modules
platform based on the Intel Atom E3900 Series, Pentium and Celeron
processors.

## Technology

```eval_rst
+------------------+----------------------------------+
| COMe Type        | mini pin-out type 10             |
+------------------+----------------------------------+
| SoC              | Intel Atom x5-E3940 (4 core)     |
+------------------+----------------------------------+
| GPU              | Intel HD Graphics 500            |
+------------------+----------------------------------+
| Coprocessor      | Intel TXE 3.0                    |
+------------------+----------------------------------+
| RAM              | 8GB DDR3L                        |
+------------------+----------------------------------+
| eMMC Flash       | 32GB eMMC pSLC                   |
+------------------+----------------------------------+
| USB3             | x2                               |
+------------------+----------------------------------+
| USB2             | x6                               |
+------------------+----------------------------------+
| SATA             | x2                               |
+------------------+----------------------------------+
| LAN              | Intel I210IT, I211AT             |
+------------------+----------------------------------+
| Super IO/EC      | Kontron CPLD/EC                  |
+------------------+----------------------------------+
| HWM              | NCT7802                          |
+------------------+----------------------------------+
```

## Building coreboot

The following commands will build a working image:

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.kontron_mal10
make
```
## Payloads
- SeaBIOS
- edk2
- Linux as payload

## Flashing coreboot

The SPI flash can be accessed internally using [flashrom].
The following command is used to flash BIOS region.

```bash
$ flashrom -p internal --ifd -i bios -w coreboot.rom --noverify-all
```

## Hardware Monitor

The Nuvoton [NCT7802Y] is a hardware monitoring IC, capable of monitor critical
system parameters including power supply voltages, fan speeds, and temperatures.
The remote inputs can be connected to CPU/GPU thermal diode or any thermal diode
sensors and thermistor.

- 6 temperature sensors;
- 5 voltage sensors;
- 3 fan speed sensors;
- 4 sets of temperature setting points.

PECI is not supported by Apollo Lake Pentium/Celeron/Atom processors and the CPU
temperature value is taken from a thermal resistor (NTC) that is placed very
close to the CPU.

## Untested

- IGD/LVDS
- SDIO

## Tested and working

- Kontron CPLD/EC (Serial ports, I2C port, GPIOs)
- NCT7802 [HWM](#Hardware Monitor)
- USB2/3
- Gigabit Ethernet ports
- eMMC
- SATA
- PCIe ports
- IGD/DP

## TODO
- Onboard audio (codec IDT 92HD73C1X5, currently disabled)
- S3 suspend/resume

[mAL10]: https://www.kontron.com/products/iot/iot-industry-4.0/iot-ready-boards-and-modules/com-express/com-express-mini/come-mal10-e2-.html
[W25Q128FV]: https://www.winbond.com/resource-files/w25q128fv%20rev.m%2005132016%20kms.pdf
[flashrom]: https://flashrom.org/Flashrom
[NCT7802Y]: https://www.nuvoton.com/products/cloud-computing/hardware-monitors/desktop-server-series/nct7802y/?__locale=en
[crashes]: https://pastebin.com/cpCfrPCL
