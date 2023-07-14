# Syste76 Darter Pro 9 (darp9)

## Specs

- CPU
  - Intel Core i5-1340P
  - Intel Core i7-1360P
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 15.6" 1920x1080@60Hz LCD
  - 1x HDMI
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR5 SO-DIMMs @ 5600 MHz
- Networking
  - Gigabit Ethernet (Realtek RTL8111H)
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6E AX210/211)
- Power
  - 90W (19V, 4.74A) AC barrel adapter
  - USB-C charging, compatible with 65W+ chargers
  - 73Wh 4-cell Lithium-ion battery (L140BAT-4)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - 2x M.2 PCIe NVMe Gen 4 SSDs
  - MicroSD card reader (OZ711LV2)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 Gen 2 Type-C
  - 1x USB 3.2 Gen 2 Type-A
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

The flash chip (U22) is above the left DIMM slot.
