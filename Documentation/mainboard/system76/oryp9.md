# System76 Oryx Pro 9 (oryp9)

## Specs

- CPU
  - Intel Core i7-12700H
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options:
    - NVIDIA GeForce RTX 3070 Ti (Max-Q)
    - NVIDIA GeForce RTX 3080 Ti (Max-Q)
  - eDP options:
    - 15.6" 1920x1080@144Hz LCD (BOE NV156FHM-NY5)
    - 17.3" 1920x1080@144Hz LCD (BOE NV173FHM-NY1)
  - 1x HDMI 2.1
  - 1x Mini DisplayPort 1.4
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX200/201)
- Power
  - 230W (20V, 11.5A)  AC barrel adapter (Lite-On PA-1231-26)
  - 80Wh 6-cell Lithium-ion battery
- Sound
  - Realtek ALC1220 codec
  - TI TAS5825M smart amp
  - Internal speakers and microphone
  - Combined 3.5mm headphone & microphone jack
  - Combined 3.5mm microphone & S/PDIF jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 2x M.2 PCIe NVMe Gen 4 SSD
  - MicroSD card reader (RTS5227S)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 (Gen 2) Type-C
  - 2x USB 3.2 (Gen 1) Type-A
- Dimensions
  - 15": 35.814cm x 24.003cm x 2.489cm, 1.99kg
  - 17": 39.599cm x 26.213cm x 2.489cm, 2.3kg

## Flashing coreboot

```eval_rst
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | Macronix            |
+---------------------+---------------------+
| Model               | MX25L25673G         |
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

The flash chip (U61) is left of the DIMM slots.
