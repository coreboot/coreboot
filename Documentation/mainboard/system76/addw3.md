# System76 Adder Workstation 3 (addw3)

## Specs

- CPU
  - Intel Core i9-13900HX
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options
    - NVIDIA GeForce RTX 4050
    - NVIDIA GeForce RTX 4060
    - NVIDIA GeForce RTX 4070
  - eDP displays
    - 15.6" 1920x1080@144Hz LCD
    - 17.3" 1920x1080@144Hz LCD
  - External outputs
    - 1x HDMI 2.1
    - 1x Mini DisplayPort 1.4
    - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR5 SO-DIMMs @ 4800 MHz
- Networking
  - Intel I219-V gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6E AX210/AX211
- Power
  - 280W (20V, 14A) DC-in port
    - Included: Chicony A18-280P1A
  - 73Wh 4-cell Lithium-Ion battery
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - Dedicated 3.5mm microphone jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 2x M.2 (PCIe NVMe Gen 4) SSDs
  - MicroSD card reader
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 Gen 2 Type-C
  - 1x USB 3.2 Gen 1 Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 15": 2.71cm x 35.95cm x 23.8cm, 2.05kg
  - 17": 2.82cm x 39.69cm x 26.2cm, 2.85kg

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

The flash chip (U65) is above the battery connector.
