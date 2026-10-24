# Framework Laptop 13 12th Gen Intel Core (hx30)

## Support Status

This port is currently not officially supported by Framework.
It is a proof of concept for internal evaluation of the technical feasibility
of coreboot at Framework.

All mainboards sold to customers ship with BootGuard enabled. While it is
possible for end users to build the image with publicly available source and
binaries, flashing will not result in a bootable system.

Alderlake and Raptorlake are pin-compatible, so in theory this port can run on
both. So far it has only been tested on Alderlake systems.

Everything listed under Specs has been tested and works with coreboot, unless
marked as untested or listed below.

## Not working

- Needs MEC1521 EC enablement
  - Suspend / resume (S3 / S0ix), causes kernel panic
  - I2C HID EC device media keys and airplane mode button

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
    - 12th Gen Intel Core (Alder Lake-P), one of:
      - i5-1240P, i7-1260P, i7-1280P
- EC
    - Microchip MEC1521 with Framework forked Chrome EC firmware
    - Backlit keyboard, with standard PS/2 keycodes and I2C HID hotkeys
    - Battery
    - Lid switch
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - USB-C DisplayPort video (HDMI/DP through Expansion Cards)
- Memory
    - 2x SODIMM slots for up to 32GB DDR4-3200 each
- Networking
    - AX210 M.2 2230 PCIe Gen3 x1 WiFi / Bluetooth
- Sound
    - IDT 92HD95 HDA Codec
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - USB-C DisplayPort/HDMI audio
- Built-in devices
    - I2C HID PTP Touchpad
    - I2C HID Touchscreen (compatible with Laptop 13 Pro touch panel, untested)
    - I2C HID EC device: media keys, airplane mode button and ALS sensor
    - Intel RAPL power reporting
    - USB Camera
    - USB Fingerprint reader
    - eSPI PS2 Keyboard
    - eSPI PS2 emulated mouse/touchpad (if OS has no I2C driver)
    - Nuvoton discrete TPM 2.0
- Storage
    - M.2 2280 PCIe Gen4 x4 SSD, on the CPU-attached root port
- 4 Type-C Ports
    - USB 3.2 Gen 2
    - Thunderbolt 4, one JHL8040R Burnside Bridge retimer per port
      (host-to-host 40 Gbit/s link tested, PCIe tunnelling to docks untested)
    - 100W USB PD 3.0 Charging
    - DisplayPort 1.4 Alt-Mode

### Build

The following commands will build a working image:

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_FRAMEWORK
./util/scripts/config --enable BOARD_FRAMEWORK_HX30
make olddefconfig
make -j$(nproc)
```

If you don’t plan on using coreboot’s serial console to collect logs, you might want to disable it at this point (./util/scripts/config --disable CONSOLE_SERIAL). It should reduce the boot time by several seconds.

If you would like to boot your existing SSD with UEFI compatible Linux/Windows installation, enable EDK2 Payload (./util/scripts/config --enable PAYLOAD_EDK2).

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Size                | 32 MiB     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```

Flashing example with BusPirate 5 connected to the ROM:

```
flashrom --ifd -i bios --noverify-all -w build/coreboot.rom --progress -p buspirate_spi:dev=/dev/ttyACM1,serialspeed=115200
```

## Serial Console

Serial console is available on UART0 (GPP_H10/GPP_H11) through the debug pads
on the mainboard.

See [FrameworkDebugger](https://github.com/frameworkComputer/FrameworkDebugger) for details.
