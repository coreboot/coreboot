# System76 Galago Pro 7 (galp7)

## Specs

- CPU
  - Intel Core i5-13500H
  - Intel Core i7-13700H
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 14.1" 1920x1080@144Hz LCD (Sharp LQ140M1JW49)
  - 1x HDMI 2.1
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX210/211)
- Power
  - 90W (19V, 4.74A) AC barrel adapter (Chicony A16-090P1A)
  - USB-C charging, compatible with 90W+ chargers
  - 53Wh 4-cell Lithium-ion battery
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
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
