# StarBook Mk V

## Specs

- CPU (full processor specs available at https://ark.intel.com)
    - Intel i7-1165G7 (Tiger Lake)
    - Intel i3-1110G4 (Tiger Lake)
- EC
    - ITE IT5570E
    - Backlit Keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - Charger, using AC adapter or USB-C PD
    - Suspend / resume
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - eDP 14-inch 1920x1080 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 2 x DDR4 SODIMM
- Networking
    - AX201 2230 WiFi / Bluetooth
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
    - Thunderbolt 4.0 (left)
    - USB 3.1 Gen 2 Type-A (left)
    - USB 3.1 Gen 1 Type-A (right)
    - USB 2.0 Type-A (right)

## Building coreboot

Please follow the [Star Labs build instructions](../common/building.md) to build coreboot, using `config.starlabs_starbook_tgl` as config file.

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
