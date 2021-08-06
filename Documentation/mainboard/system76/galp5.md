# System76 Galago Pro 5 (galp5)

## Specs

- CPU
  - Intel Core i7-1165G7
  - Intel Core i5-1135G7
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - dGPU options
    - NVIDIA GeForce 1650
    - NVIDIA GeForce 1650 Ti
  - eDP 14.1" 1920x1080@60Hz LCD (BOE NV140FHM-N62)
  - 1x HDMI
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64 (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - with Intel iGPU only
    - 65W (19V, 3.42A) AC barrel adapter
    - USB-C charging compatible with 65W+ charger
  - with NVIDIA dGPU
    - 90W (19V, 4.74A) AC barrel adapter
    - USB-C charging compatible with 90W+ charger
- Sound
  - Realtek ALC293D codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - 1x M.2 PCIe NVMe Gen 4 SSD
  - SD card reader
- USB
  - 2x USB 3.2 (Gen 1) Type-A
  - 1x USB 3.2 (Gen 2) Type-C
  - 1x USB Type-C with Thunderbolt 4
- Dimensions
  - 32.49cm x 22.5cm x 1.75cm, 1.41kg

## Flashing coreboot

```eval_rst
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | GigaDevice          |
+---------------------+---------------------+
| Model               | GD25B127D           |
+---------------------+---------------------+
| Size                | 16 MiB              |
+---------------------+---------------------+
| Package             | SOIC-8              |
+---------------------+---------------------+
| Internal flashing   | yes                 |
+---------------------+---------------------+
| External flashing   | yes                 |
+---------------------+---------------------+
```

The flash chip (U33) is next to the M.2 WiFi card.
