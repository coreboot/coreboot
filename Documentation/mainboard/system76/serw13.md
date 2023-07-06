# System76 Serval Workstation 13 (serw13)

## Specs

- CPU
  - Intel Core i9-13900HX
- Chipset
  - Intel HM770
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options:
    - NVIDIA GeForce RTX 4060
    - NVIDIA GeForce RTX 4070
  - eDP options:
    - 15.6" 1920x1080@165Hz LCD (BOE NV156FHM-NY8)
    - 17.6" 3840x2160@144Hz LCD (AUO B173ZAN03.0)
  - 1x HDMI 2.1
  - 1x Mini DisplayPort 1.4
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR5 SO-DIMMs @ 5200 Mhz
- Networking
  - Realtek RTL8125BG-CG 2.5G Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX210/211)
- Power
  - 280W (20V, 14A) AC adapter (Chicony A18-280P1A)
  - 80Wh 6-cell Lithium-ion battery ()
- Sound
  - Realtek ALC1220 codec
  - Realtek ALC1318 smart amp
  - Internal speakers and microphone
  - Combined 3.5mm headphone & microphone jack
  - Combined 3.5mm microphone & S/PDIF jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 2x M.2 PCIe NVMe Gen 4 SSDs
  - MicroSD card reader (Realtek RTS5227S)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 Gen 2 Type-C
  - 2x USB 3.2 Gen 1 Type-A
- Dimensions
  - 15": 2.49cm x 35.8cm x 24.0cm, 2.4kg
  - 17": 2.49cm x 39.6cm x 26.2cm, 2.8kg

## Flashing coreboot

```eval_rst
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | GigaDevice          |
+---------------------+---------------------+
| Model               | GD25B256E           |
+---------------------+---------------------+
| Size                | 32 MiB              |
+---------------------+---------------------+
| Package             | WSON-8              |
+---------------------+---------------------+
| Internal flashing   | yes                 |
+---------------------+---------------------+
| External flashing   | yes                 |
+---------------------+---------------------+
```

The flash chip (U46) is left of the memory slots.
