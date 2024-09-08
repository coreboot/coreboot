# System76 Oryx Pro (oryp6)

## Specs

- CPU
  - Intel i7-10875H
- Chipset
  - Intel HM470
- EC
  - ITE IT5570E running <https://github.com/system76/ec>
- GPU
  - NVIDIA GeForce RTX 2080 Super (Max-Q)
  - or NVIDIA GeForce RTX 2070 (Max-Q)
  - or NVIDIA GeForce RTX 2060
  - eDP 15.6" or 17.3" 1920x1080@144Hz LCD
  - HDMI, Mini DisplayPort 1.4, and DisplayPort over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - 180W (19.5V, 9.23A) AC adapter
  - 73Wh 3-cell battery
- Sound
  - Internal speakers and microphone
  - Combined headphone and microphone 3.5mm jack
  - Combined microphone and S/PDIF (optical) 3.5mm jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- USB
  - 3x USB 3.2 Gen 1 Type-A
  - 1x USB Type-C with Thunderbolt 3
- Dimensions
  - 15": 35.75cm x 23.8cm x 1.98cm, 1.99kg
  - 17": 39.59cm x 26.495cm x 1.99cm, 2.3kg

## Flashing coreboot

```{eval-rst}
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

The flash chip (U53) is above the M.2 SSD connectors.
