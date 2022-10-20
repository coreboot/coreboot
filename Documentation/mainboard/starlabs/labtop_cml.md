# Star LabTop Mk IV

## Specs

- CPU (full processor specs available at https://ark.intel.com)
    - Intel i7-10710U (Comet Lake)
    - Intel i3-10110U (Comet Lake)
- EC
    - ITE IT8987E
    - Backlit Keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - Charger, using AC adapter or USB-C PD
    - Suspend / resume
- GPU
    - Intel UHD Graphics 620
    - GOP driver is recommended, VBT is provided
    - eDP 13-inch 1920x1080 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 16GB on-board *1
- Networking
    - AX201 CNVi WiFi / Bluetooth soldered to PCBA
- Sound
    - Realtek ALC256
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe SSD
    - RTS5129 MicroSD card reader
- USB
    - 1280x720 CCD camera
    - USB 3.1 Gen 2 Type-C (left)
    - USB 3.1 Gen 2 Type-A (left)
    - USB 3.1 Gen 1 Type-A (right)

[^1] The Comet Lake PCB supports multiple memory variations that are based on hardware configuration resistors see `src/mainboard/starlabs/labtop/variants/cml/romstage.c`

## Building coreboot

Please follow the [Star Labs build instructions](../common/building.md) to build coreboot, using `config.starlabs_labtop_cml` as config file.

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | 25Q128JVSQ |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+

Please see [here](../common/flashing.md) for instructions on how to flash with fwupd.
