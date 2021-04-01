# System76 Galago Pro 4 (galp4)

## Specs

- CPU
  - Intel Core i7-10510U
  - Intel Core i5-10210U
- EC
  - ITE IT8587E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel UHD Graphics 620
  - eDP 14.1" 1920x1080@60Hz LCD (Innolux N140HCA-EAC)
  - 1x HDMI
  - 1x Mini DisplayPort 1.2
  - 1x DisplayPort over USB-C
- Memory
  - Up to 64 (2x32GB) dual-channel DDR4 SO-DIMMs @ 2666 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
    - or Intel Wireless-AC 9560
- Power
  - 40W (19V, 2.1A) AC barrel adapter
    - Chicony A13-040A3A, using a C5 power cord
  - 35.3Wh 3-cell Lithium-Ion battery
- Sound
  - Realtek ALC293 codec
  - Internal speakers and microphone
  - 3.5mm headphone jack
  - 3.5mm microphone jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- Storage
  - 1x M.2 PCIe NVMe or SATA SSD
  - 1x 2.5" SATA SSD
  - SD card reader
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 2x USB 3.1 Gen 1 Type-A
- Dimensions
  - 1.8cm x 33cm x 22.5cm, 1.3kg

## Flashing coreboot

```eval_rst
+---------------------+-----------------+
| Type                | Value           |
+=====================+=================+
| Socketed flash      | no              |
+---------------------+-----------------+
| Vendor              | GigaDevice      |
+---------------------+-----------------+
| Model               | GD25B127D       |
+---------------------+-----------------+
| Size                | 16 MiB          |
+---------------------+-----------------+
| Package             | SOIC-8          |
+---------------------+-----------------+
| Internal flashing   | yes             |
+---------------------+-----------------+
| External flashing   | yes             |
+---------------------+-----------------+
```

The flash chip (U25) is right of the DIMM slots.
