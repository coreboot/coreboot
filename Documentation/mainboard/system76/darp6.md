# System76 Darter Pro 6 (darp6)

## Specs

- CPU
  - Intel Core i7-10510U
  - Intel Core i5-10210U
- EC
  - ITE IT8587E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel UHD Graphics 620
  - eDP 15.6" 1920x1080@60Hz LCD (LG LP156WFC-SPD3)
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 2666 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
    - or Intel Wireless-AC 9560
- Power
  - 65W (19V, 3.42A) AC barrel adapter
    - Chicony A12-065N2A, using a C5 power cord
  - 54.5Wh 4-cell Lithium-Ion battery
- Sound
  - Realtek ALC293 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone and microphone jack
  - Combined 3.5mm microphone and S/PDIF jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- Storage
  - 1x M.2 PCIe NVMe or SATA SSD
  - SD card reader
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 2x USB 3.0 Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 1.98cm x 36.04cm x 24.46cm, 1.6kg


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

The flash chip (U26) is right of the DIMM slots.
