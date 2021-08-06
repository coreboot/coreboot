# System76 Darter Pro 7 (darp7)

## Specs

- CPU
  - Intel Core i5-1135G7
  - Intel Core i7-1165G7
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
  - M.2 PCIe/CNVi Wifi/Bluetooth
    - Intel Wi-Fi 6 AX200/201
- Power
  - 65W (19V, 3.42A) AC barrel adapter
    - Included: Chicony A18-065N3A, using a C5 power cord
  - USB-C charging, compatible with 65W+ chargers
  - 73Wh 4-cell Lithium-ion battery
- Sound
  - Realtek ALC293 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 4 SSD
  - M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - MicroSD card reader (RTS5227S)
- USB
  - 1x USB-C Type-C with Thunderbolt 4
  - 1x USB 3.2 (Gen 2) Type-C
  - 1x USB 3.2 (Gen 2) Type-A
  - 1x USB 2.0 Type-A
- Dimensions
  - 1.99cm x 35.70cm x 22.05cm, 1.74kg

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

The flash chip (U12) is above the left DIMM slot.
