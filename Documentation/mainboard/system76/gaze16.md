# System76 Gazelle 16 (gaze16)

## Specs

- CPU
  - Intel Core i7-11800H
- Chipset
  - Intel HM570
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options
    - NVIDIA GeForce RTX 3050
    - NVIDIA GeForce RTX 3050 Ti
    - NVIDIA GeForce RTX 3060
  - eDP displays
    - 15.6" 1920x1080@144Hz LCD (AUO B156HAN08.4)
    - 17.3" 1920x1080@144Hz LCD (LG LP173WFG-SPB3)
  - External outputs
    - RTX 3050/3050 Ti
      - 1x HDMI
      - 1x Mini DisplayPort 1.4
    - RTX 3060
      - 1x HDMI
      - 1x Mini DisplayPort 1.2
      - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
    - Either onboard Intel I219-V or Realtek RTL8111H controller
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - RTX 3050/3050 Ti
    - 150W AC barrel adapter
      - Included: Chicony A17-150P2A, using a C5 power cord
  - RTX 3060
    - 180W AC barrel adapter
      - Included: Chicony A17-180P4A, using a C5 power cord
  - 48.96Wh 4-cell battery
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - Dedicated 3.5mm microphone jack
  - HDMI, mDP, USB-C DP audio
- Storage
  - 1x M.2 PCIe NVMe Gen 4 SSD
  - 1x M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - SD card reader
    - Realtek RTS5227S on RTX 3050/3050 Ti models
    - Realtek OZ711LV2 on RTX 3060 models
- USB
  - 1x USB 3.2 Gen 2 Type-C
    - Supports DisplayPort over USB-C on RTX 3060 models only
    - Does not support USB-C charging (USB-PD) or Thunderbolt
  - 1x USB 3.2 Gen 2 Type-A
  - 1x USB 3.2 Gen 1 Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 15": 35.75cm x 23.8cm x 1.98cm, 1.99kg
  - 17": 39.59cm x 26.495cm x 1.99cm, 2.3kg

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

The flash chip (U51 on 3050 variant, U52 on 3060 variant) is left of the top DIMM slot.
