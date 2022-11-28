# System76 Galago Pro 6 (galp6)

## Specs

- CPU
  - Intel Core i5-1240P
  - Intel Core i7-1260P
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 14.1" 1920x1080@60Hz LCD (Panda LM140LF2L02)
  - 1x HDMI 2.1
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX200/201)
- Power
  - 90W (19V, 4.74A) AC barrel adapter (Chicony A16-090P1A)
  - USB-C charging, compatible with 90W+ chargers
  - 53.35Wh 4-cell Lithium-ion battery (NV40BAT-4-53)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 4 SSD
  - MicroSD card reader (OZ711LV2)
- USB
  - 1x USB-C Type-C with Thunderbolt 4
  - 1x USB 3.2 (Gen 2) Type-C
  - 2x USB 3.2 (Gen 1) Type-A
- Dimensions
  - 32.49cm x 22.5cm x 1.82cm, 1.45kg

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

The flash chip (U43) is left of the wireless card.
