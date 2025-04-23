# ASUS P8Z77-V LE PLUS

This page describes how to run coreboot on the [ASUS P8Z77-V LE PLUS].

## Flashing coreboot

```{eval-rst}
+---------------------+----------------+
| Type                | Value          |
+=====================+================+
| Socketed flash      | yes            |
+---------------------+----------------+
| Model               | W25Q64FVA1Q    |
+---------------------+----------------+
| Size                | 8 MiB          |
+---------------------+----------------+
| Package             | DIP-8          |
+---------------------+----------------+
| Write protection    | yes            |
+---------------------+----------------+
| Dual BIOS feature   | no             |
+---------------------+----------------+
| Internal flashing   | yes            |
+---------------------+----------------+
```

### How to flash

The main SPI flash cannot be written because the vendor firmware disables BIOSWE
and enables BLE/SMM_BWP flags in BIOS_CNTL for their latest BIOSes. An external
programmer is required. You must flash standalone, flashing in-circuit doesn't
work. The flash chip is socketed, so it's easy to remove and reflash.

See page 2-2 of user's manual for flash chip location.

### Extra preparations for changing PCIe slot configuration

On vendor firmware, the black PCIEX16_3 slot can be configured as x2 or x4.
If set for x4, PCIEX1_1 and PCIEX1_2 are disabled.

Before flashing coreboot for the first time, decide how you want to use the PCIe slots.
If you want to be able to choose between using the two PCIEX1 slots and the PCIEX16_3 slot at
x4 bandwidth, you need to do some preparation, namely make two backups of the whole flash
chip, specifically the flash descriptor under both configurations.

Enter vendor UEFI setup and check the PCIEX16_3 (black) slot bandwidth setting. You'll back up
under this setting first. Once one backup is made, come back and change the setting
from x2 to x4 (or vice versa) and reboot once, then make the other backup.

With PCIEX16_3 (black) slot bandwidth at x2, run these commands:
```bash
flashrom -p internal -r pciex163_x2.bin
dd if=pciex163_x2.bin of=ifd-pciex163_x2.bin bs=4096 count=1
```

With PCIEX16_3 (black) slot bandwidth at x4, run these commands:
```bash
flashrom -p internal -r pciex163_x4.bin
dd if=pciex163_x4.bin of=ifd-pciex163_x4.bin bs=4096 count=1
```
(`dd` needs not be run as root.)

Save the shortened `ifd-pciex163_*.bin` files for when you want to change the configuration.
Keep one of the full backups as well.

See "PCIe config" section below for more details.

## Working

- Core i5-3570K and i7-3770K CPUs
- Corsair CMZ16GX3M2A1600C10 2x8GB memory kit
- SeaBIOS 1.16.3
- edk2 mrchromebox fork uefipayload_2501
- Kernel 6.12.7
- All USB2 ports (mouse, keyboard)
- All USB3 ports
- Z77 SATA ports (WD Blue SA510, Liteon LH-20A1L)
- nVidia 8800GT GPU in PCIEX16_1 slot running x16
- PCI slots (Sound Blaster Live! Value)
- RTL8111F LAN
- CPU temperature sensors and hardware monitor
  (see [below](#hardware-monitoring-and-fan-speed-control))
- Integrated graphics with libgfxinit and VBT
  (all ports tested and working)
- Both PCIe x1 slots when properly configured
  (see [How to flash](#how-to-flash) above and [PCIe config](#pcie-config);
  Atheros 928x miniPCIe Wifi on adapter & MSI Herald-BE Wifi7 adapter)
- PCIe x4 slot with Intel Octane H10 1TB NVMe at x2 mode
- Serial port
- PS/2 keyboard
- Analog 7.1 audio out the 3.5mm jacks on rear panel
- Front HDA audio panel
- Digital audio out (Optical, internal SPDIF header, HDMI, DisplayPort)

  Although a `spdif_dest` option is provided for feature parity with vendor firmware,
  it doesn't seem to matter and digital audio out is available through all ports.
  It does, however, change how the ports are presented to the OS.

- S3 suspend from Linux

## Known issues

- For 7.1 analog audio to work, at least the front channel (green jack) must be connected.

## Untested

- Hotplug of Z77 SATA ports
- EHCI debugging

## Not working

- Wake-on-LAN
- PS/2 mouse (requires a patch currently under review)
- Asmedia USB 3.0 battery charging support (for USB 3 ports on the LAN stack)
- USB Charger+ (When the bottom USB 3 port on the eSATA stack, also used for BIOS flashback,
  remains powered while the rest of the system is off. Both features are controlled by the same
  AI1314 controller.)
- Marvell SATA ports are brought up in IDE mode, pata_marvell driver is loaded,
  but are effectively unusable.

## PCIe config
See [Extra preparations](#extra-preparations-for-changing-pcie-slot-configuration) section above.

Changing the PCIe slot configuration requires manipulating a PCH GPIO line and a soft strap in
the flash chip's descriptor section, which is read-only at runtime. coreboot programs the GPIO
to match the soft strap, but how it can update the soft strap itself is to be determined. Until
then, to make this change you have to re-flash the descriptor yourself, with one of the two
copies you previously saved per above:
```bash
flashrom -p internal --ifd -i fd -w ifd-pciex163_x2.bin
```

## Hardware monitoring and fan speed control

Although all fan ports are 4-pin for PWM fans, only CPU_FAN has actual PWM control;
all other fan speed control is by voltage only.

Write 1 into `/sys/class/hwmon/hwmon1/pwm1_mode` to enable CHA_FAN1 control, otherwise it
runs at full speed.

`fan5`/`pwm5` is not implemented and should be ignored.

These are the sensors.conf settings for this board:

```
label fan1 "CHA_FAN1"
label fan2 "CPU_FAN"
label fan3 "CHA_FAN2"
label fan4 "CHA_FAN3"
ignore fan5
label in1 "+12V"
label in4 "+5V"
compute in1 @*12, @/12
compute in4 @*5, @/5
set temp1_type 4
set temp2_type 4
```

## Extra onboard switches and LEDs

- `BIOS_FLBK`:
  Vendor firmware uses this button to facilitate a simple update mechanism
  via a USB drive plugged into the bottom USB port of the USB/ESATA6G stack.
  It connects to the proprietary AI1314 controller, along with `FLBK_LED`.

- `MemOK!`:
  OEM firmware uses this button for memory tuning related to overclocking.
  It connects to pin 74 of super I/O.

- `DRAM_LED` lights up when there is a memory problem or when vendor MemOK! feature is
  operating. Connects to GP07 line of super I/O. coreboot lights it up during memory init
  similar to vendor firmware.

- `EPU`: When enabled, lights up `EPU_LED` and takes PCH GPIO44 low.
- `TPU`: When enabled, lights up `TPU_LED` and takes PCH GPIO45 low.

  `EPU` and `TPU` are cues to vendor firmware to enable two embedded controllers for
  overclocking features. coreboot is not yet able to make use of these two signals.

- `SB_PWR` lights up whenever board is receiving power. It's all hardware
  and does not concern coreboot.

- `DRCT` is an undocumented 2-pin header next to the front panel connector block. It
  connects to both the power button circuit and Z77's intruder detection input. Shorting this
  header triggers both. With coreboot it currently works the same as the power button.

## Extra exposed GPIOs at `TB_HEADER`

A number of GPIO lines are broken out to `TB_HEADER` to support the ThunderboltEX adapter,
which never took off. Now they're yours to play with. Additional programming may be required such
as enabling GPIO by I/O for maximum effect.

This may be safely ignored for most normal uses.

**Be careful not to apply more than 3.3v to these pins!** And do not touch the two pins
labeled "NOT A GPIO".

Pinout:
```
 +---+---+---+---+---+
 | 2 | 4 | 5 | 7 | 9 |
 +---+---+---+---+---+
 | 1 | 3 |   | 6 | 8 |
 +---+---+---+---+---+
```

```{eval-rst}
+-----+-----------------------+----------+--------+
| Pin | Name                  | Source   | GPIO # |
+=====+=======================+==========+========+
|  1  | S_DP_DDC_CLK_TO_TB    | **NOT A GPIO**    |
+-----+-----------------------+----------+--------+
|  2  | TB_GPIO_6             | NCT6779D |   14   |
+-----+-----------------------+----------+--------+
|  3  | S_DP_DDC_DATA_TO_TB   | **NOT A GPIO**    |
+-----+-----------------------+----------+--------+
|  4  | TB_GPIO_7             | NCT6779D |   13   |
+-----+-----------------------+----------+--------+
|  5  | TB_FWUPDATE           | NCT6779D |   11   |
+-----+-----------------------+----------+--------+
|  6  | TB_DEV_HPD            | Z77      |    0   |
+-----+-----------------------+----------+--------+
|  7  | TB_GO2SX              | NCT6779D |   17   |
+-----+-----------------------+----------+--------+
|  8  | TB_GO2SX#_ACK         | NCT6779D |   16   |
+-----+-----------------------+----------+--------+
|  9  | Not connected                             |
+-----+-------------------------------------------+
```

Pins 2, 4, 6, 8 have 1M ohm pulldowns.

## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/sandybridge/index` |
+------------------+--------------------------------------------------+
| Southbridge      | bd82x6x                                          |
+------------------+--------------------------------------------------+
| CPU              | model_206ax                                      |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT6779D                                 |
+------------------+--------------------------------------------------+
| EC               | TPU (ENE KB3722), AI1314                         |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

## Extra resources

- [Flash chip datasheet][W25Q64FVA1Q]

[ASUS P8Z77-V LE PLUS]: https://www.asus.com/supportonly/p8z77-v%20le%20plus/helpdesk_manual/
[W25Q64FVA1Q]: https://www.winbond.com/resource-files/w25q64fv%20revs%2007182017.pdf
[flashrom]: https://flashrom.org/Flashrom
[rtnicpg]: https://github.com/redchenjs/rtnicpg
