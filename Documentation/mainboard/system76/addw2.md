# System76 Adder Workstation 2 (addw2)

## Specs

- CPU
  - Intel Core i7-10875H
- Chipset
  - Intel HM470
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - NVIDIA GeForce RTX 2070 Super
  - eDP 15.6" 3840x2160@60Hz OLED (Samsung ATNA56WR06)
  - 1x HDMI
  - 1x Mini DisplayPort 1.4
  - 1x DisplayPort 1.4 over USB-C
- Memory
 - Up to 64 (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - 230W (19.5V, 11.8A) AC barrel adapter
    - Chicony A17-230P1A, using a C5 power cord
  - 62Wh 6-cell Lithium-Ion battery
- Sound
  - Internal speakers and microphone
  - Combined 3.5mm headhpone and microphone jack
  - Combined 3.5mm microphone and S/PDIF jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - M.2 PCIe NVMe Gen 3 SSD
  - 2.5" SATA 3 SSD
  - SD card reader (RTS5250S)
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 1x USB 3.2 Gen 2 Type-C
  - 3x USB 3.2 Gen 1 Type-A
- Dimensions
  - 35.890cm x 25.806cm x 2.997cm, 2.5kg

## Flashing coreboot

```eval_rst
+---------------------+-----------------+
| Type                | Value           |
+=====================+=================+
| Socketed flash      | no              |
+---------------------+-----------------+
| Vendor              | Macronix        |
+---------------------+-----------------+
| Model               | MX25L12872F     |
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

The flash chip (U60) is next to the battery connector.
