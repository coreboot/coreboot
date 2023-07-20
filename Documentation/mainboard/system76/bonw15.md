# System76 Bonobo Workstation 15 (bonw15)

## Specs

- CPU
  - Intel Core i9-13900HX
- Chipset
  - Intel HM770
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options:
    - NVIDIA GeForce RTX 4080
    - NVIDIA GeForce RTX 4090
  - eDP 17.3" 3840x2160@144Hz LCD (BOE NE173QUM-NY1)
  - 1x HDMI 2.1
  - 1x Mini DisplayPort 1.4
  - 2x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR5 SO-DIMMs @ 5200 Mhz
- Networking
  - Onboard Intel Killer Ethernet E3100X 2.5 GbE
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX210/211)
- Power
  - 330W (19.5V, 16.42A) AC adapter (Chicony A20-330P1A)
    - Rectangular connector; not a barrel connector
  - 99Wh 8-cell Lithium-ion battery
- Sound
  - Realtek ALC1220 codec
  - Realtek ALC1318 smart amp
  - Internal speakers and microphone
  - Combined 3.5mm headphone & microphone jack
  - Combined 3.5mm microphone & S/PDIF jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 3x M.2 PCIe NVMe Gen 4 SSDs
- USB
  - 2x USB Type-C with Thunderbolt 4
  - 2x USB 3.2 Gen 2 Type-A
- Dimensions
  - 2.49cm x 39.6cm x 27.8cm, 3.29kg

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

The flash chip (U58) is next to the left M.2 port.
