# Syste76 Darter Pro 8 (darp8)

## Specs

- CPU
  - Intel Core i5-1240P
  - Intel Core i7-1260P
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 15.6" 1920x1080@60Hz LCD
  - 1x HDMI
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX200/201)
- Power
  - 90W (19V, 4.74A) AC barrel adapter (Chicony A16-090P1A)
  - USB-C charging, compatible with 65W+ chargers
  - 73Wh 4-cell Lithium-ion battery (L140BAT-4)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 4 SSD
  - M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - MicroSD card reader (OZ711LV2)
- USB
  - 1x USB-C Type-C with Thunderbolt 4
  - 1x USB 3.2 (Gen 2) Type-C
  - 1x USB 3.2 (Gen 2) Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 35.7cm x 22.05cm x 1.99cm, 1.74kg

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
```eval_rst
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | Winbond             |
+---------------------+---------------------+
| Model               | W25Q256.V           |
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

The flash chip (U19) is above the left DIMM slot.
