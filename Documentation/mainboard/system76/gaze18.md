# System76 Gazelle 18 (gaze18)

## Specs

- CPU
  - Intel i9-13900H
- Chipset
  - Intel HM770
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - NVIDIA GeForce RTX 3050 (70W TDP)
  - Intel Irix Xe Graphics
  - eDP displays
    - 17.3" 1920x1080@144Hz LCD
    - 15.6" 1920x1080@144Hz LCD
  - External outputs
    - 1x HDMI 2.1
    - 1x Mini DisplayPort 1.4
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Realtek RTL8111H gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6E AX210/AX211
- Power
  - 150W AC barrel adapter
    - Included: LiteOn PA-1151-76, using a C5 power cord
  - 54Wh 4-cell battery (NP50BAT-4-54)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - Dedicated 3.5mm microphone jack
  - HDMI, mDP audio
- Storage
  - 1x M.2 (PCIe NVMe Gen 4)
  - 1x M.2 (PCIe NVMe Gen 3)
  - MicroSD card reader
    - Realtek RTS5227S
- USB
  - 2x USB 3.2 Gen 2 Type-C
    - Does not support USB-C charging (USB-PD) or Thunderbolt
  - 1x USB 3.2 Gen 2 Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 15": 35.95cm x 23.8cm x 2.27cm, 1.99kg
  - 17": 39.69cm x 26.2cm x 2.5cm, 2.41kg

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

The flash chip (U24) is right of the M.2 SSD connectors.
