# System76 Lemur Pro (lemp9)

## Specs

- CPU
    - Intel i7-10510U
    - Intel i5-10210U
- EC
    - ITE IT5570E running <https://github.com/system76/ec>
    - Backlit Keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - Charger, using AC adapter or USB-C PD
    - Suspend/resume
    - Touchpad
- GPU
    - Intel UHD Graphics 620
    - GOP driver is recommended, VBT is provided
    - eDP 14-inch 1920x1080 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - Channel 0: 8-GB on-board DDR4 Samsung K4AAG165WA-BCTD
    - Channel 1: 8-GB/16-GB/32-GB DDR4 SO-DIMM
- Networking
    - M.2 PCIe/CNVi WiFi/Bluetooth
- Sound
    - Realtek ALC293D
    - Internal speaker
    - Internal microphone
    - Combined headphone/microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe/SATA SSD-1
    - M.2 PCIe/SATA SSD-2
    - RTS5227S MicroSD card reader
- USB
    - 1280x720 CCD camera
    - USB 3.1 Gen 2 Type-C (left)
    - USB 3.1 Gen 2 Type-A (left)
    - USB 3.1 Gen 1 Type-A (right)

## Building coreboot

The following commands will build a working image:

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.system76_lemp9
make
```

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | GigaDevice |
+---------------------+------------+
| Model               | GD25Q128C  |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```
