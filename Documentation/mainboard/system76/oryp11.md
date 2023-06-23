# System76 Oryx Pro 11 (oryp11)

## Specs

- CPU
  - Intel Core i9-13900H
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options:
    - NVIDIA GeForce RTX 4050
    - NVIDIA GeForce RTX 4060
    - NVIDIA GeForce RTX 4070
  - 16" 1920x1200@165Hz LCD
  - External outputs:
    - 1x HDMI 2.1
    - 1x Mini DisplayPort 1.4
    - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR5 SO-DIMMs @ 5600 MHz
- Networking
  - Realtek RTL8125BG-CG 2.5G Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX210/211)
- Power
  - 180W (20V, 9A) AC barrel adapter (Lite-On PA-1181-86)
  - 73Wh 4-cell Lithium-ion battery (NV40BAT-4-73)
- Sound
  - Realtek ALC1220 codec
  - Realtek ALC1318 smart amp
  - Internal speakers and microphone
  - Combined 3.5mm headphone & microphone jack
  - Combined 3.5mm microphone & S/PDIF jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 2x M.2 PCIe NVMe Gen 4 SSD
  - MicroSD card reader (Realtek RTS5227S)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 Gen 2 Type-C
  - 2x USB 2.1 Gen 1 Type-A
- Dimensions
  - 35.95cm x 27.3cm x 1.99cm, 2.7kg

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

The flash chip (U61) is left of the memory slots.
