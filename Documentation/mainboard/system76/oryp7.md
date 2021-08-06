# System76 Oryx Pro 7 (oryp7)

## Specs

- CPU
  - Intel Core i7-10870H
- Chipset
  - Intel HM470
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options
    - NVIDIA GeForce RTX 3060
    - NVIDIA GeForce RTX 3070 (Max-Q)
    - NVIDIA GeForce RTX 3080 (Max-Q)
  - eDP display options
    - 15.6" 1920x1080@144Hz LCD (LG LP156WFG-SPB3)
    - 15.6" 1920x1080@60Hz OLED (Samsung ATNA56WR06)
    - 17.3" 1920x1080@144Hz LCD (LG LP173WFG-SPB1)
  - 1x HDMI 2.1
  - 1x Mini DisplayPort 1.4
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 2933 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel WiFi 6 AX200/AX201
- Power
  - 180W (19.5V, 9.23A) AC barrel adapter
    - Chicony A17-180P4A, using a C5 power cord
  - 73Wh 3-cell battery
- Sound
  - Internal speakers and microphone
  - Combined 3.5mm headphone and microphone jack
  - Combined 3.5mm microphone and S/PDIF jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- Storage
  - 1x M.2 PCIe NVMe or SATA SSD
  - 1x M.2 PCIe NVME SSD
  - MicroSD card reader
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 3x USB 3.2 Gen 1 Type-A
- Dimensions
  - 15": 35.75cm x 23.8cm x 1.98cm, 1.99kg
  - 17": 39.59cm x 26.495cm x 1.99cm, 2.3kg

## Flashing coreboot

```eval_rst
+---------------------+-----------------+
| Type                | Value           |
+=====================+=================+
| Socketed flash      | no              |
+---------------------+-----------------+
| Vendor              | Macronix        |
+---------------------+-----------------+
| Model               | MX25L12873F     |
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

The flash chip (U66) is above the M.2 SSD connectors.
