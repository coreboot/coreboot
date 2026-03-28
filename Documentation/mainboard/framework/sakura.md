# Framework Laptop 13 Pro Intel Core Ultra Series 3 (Sakura)

## Support Status

This port is currently not officially supported by Framework.
It is a proof of concept for internal evaluation of the technical feasability
of coreboot at Framework.

All mainboards sold to customers ship with BootGuard enabled. While it is
possible for end users to build the image with publicly available source and
binaries, flashing will not result in a bootable system.

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
    - Intel Core Ultra Series 3 (Panther Lake), one of:
      - Ultra 5, Ultra 7, Ultra 9
- EC
    - Nuvoton NPCX 993 with Framework forked Chrome EC firmware
    - Backlit keyboard, with standard PS/2 keycodes and I2C HID hotkeys
    - Battery
    - Suspend / resume (S0ix, not S3)
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 1x LPCAMM2 slot for up to 96GB LPDDR5
- Networking
    - BE211 M.2 2230 CNVio3 WiFi / Bluetooth
    - Or backwards compatible with PCIe Gen3 x2 WiFi, USB2 Bluetooth
- Sound
    - Realtek ALC285 HDA Codec
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - USB-C DisplayPort/HDMI audio
- Built-in devices
    - I2C HID Touchscreen
    - I2C HID PTP Touchpad
    - I2C HID ALS Sensor
    - I2C HID Mediakeys
    - Intel fTPM
    - Intel RAPL power reporting
    - USB Camera
    - USB Fingerprint reader
    - eSPI PS2 Keyboard
    - eSPI PS2 emulated mouse/touchpad (if OS has no I2C driver)
- Storage
    - M.2 2280 PCIe Gen5 x4 SSD
- 4 Type-C Ports
    - USB 3.2 Gen 2x2
    - Thunderbolt 4
    - 100W USB PD 3.0 Charging
    - DisplayPort 2.0 Alt-Mode

### Build

The following commands will build a working image:

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_FRAMEWORK
./util/scripts/config --enable BOARD_FRAMEWORK_SAKURA
make olddefconfig
make -j$(nproc)
```

If you don’t plan on using coreboot’s serial console to collect logs, you might want to disable it at this point (./util/scripts/config --disable CONSOLE_SERIAL). It should reduce the boot time by several seconds.

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q256JV  |
+---------------------+------------+
| Size                | 32 MiB     |
+---------------------+------------+
| Package             | WSON-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```

Flashing example with BusPirate 5 connected to socketed ROM:

```
flashrom --ifd -i bios --noverify-all -w build/coreboot.rom --progress -p buspirate_spi:dev=/dev/ttyACM1,serialspeed=115200
```

## Serial Console

Serial console is available through JECDB header (unpopulated on MP) or Type-C CCD UART on the front right port.

See [FrameworkDebugger](https://github.com/frameworkComputer/FrameworkDebugger) for details.
