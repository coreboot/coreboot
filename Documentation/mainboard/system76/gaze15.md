# System76 Gazelle 15 (gaze15)

## Specs

- CPU
  - Intel Core i7 10750H
- EC
  - ITE5570E running <https://github.com/system76/ec>
- Graphics
  - Intel UHD Graphics
  - NVIDIA GeForce GTX 1650/1650 Ti/1660 Ti
  - eDP 15.6" or 17.3" 1920x1080 @ 120 Hz LCD
  - HDMI, Mini DisplayPort 1.4, and DisplayPort 1.4 over USB-C
- Memory
  - Channel 0: 8-GB/16-GB/32-GB DDR4 SO-DIMM
  - Channel 1: 8-GB/16-GB/32-GB DDR4 SO-DIMM
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi Wifi/Bluetooth
    - Intel Wireless-AC 9560, or
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - 120W AC adapter (GTX 1650 and 1650 Ti)
  - 180W AC adapter (GTX 1660 Ti)
  - 48.96Wh battery
- Sound
  - Realtek ALC293 codec
  - TAS5825MRHBR smart AMP
  - Internal speakers and microphone
  - Combined headphone and microphone 3.5mm jack
  - HDMI, Mini DisplayPort, USB-C DP audio
- Storage
  - M.2 PCIe/SATA SSD-1
  - M.2 PCIe SSD-2
  - 2.5" 7mm drive bay
  - SD card reader
  - Realtek RTL8411B card reader
- USB
  - 1x USB 2.0
  - 1x USB 3.0
  - 1x USB 3.1
  - 1x USB 3.2 Type-C

## Building coreboot

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.system76_gaze15
make
```

## Flashing coreboot

```{eval-rst}
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | GigaDevice          |
+---------------------+---------------------+
| Model               | GD25Q127C/GD25Q128C |
+---------------------+---------------------+
| Size                | 16 MiB              |
+---------------------+---------------------+
| Package             | SOIC-8              |
+---------------------+---------------------+
| Internal flashing   | yes                 |
+---------------------+---------------------+
| External flashing   | yes                 |
+---------------------+---------------------+
```
