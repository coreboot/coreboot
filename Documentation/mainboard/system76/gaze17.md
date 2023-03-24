# System76 Gazelle 17 (gaze17)

The gaze17 comes in 2 variants: gaze17-3050 and gaze17-3060-b.

## Specs

- CPU
  - Intel Core i5-12500H
  - Intel Core i7-12700H
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - dGPU options
    - NVIDIA GeForce RTX 3050
    - NVIDIA GeForce RTX 3050 Ti
    - NVIDIA GeForce RTX 3060
- Memory
  - Up to 64GB (2x32GB) dual-channel DDR4 SO-DIMMs @ 3200 MT/s
- Networking
  - Gigabit Ethernet
    - 3050: Realtek RTL8111H controller
    - 3060: Onboard Intel I219-V
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX201
- Power
  - 3050: 150W (20V, 7.5A) AC barrel adapter
  - 3060: 180W (20V, 9A) AC barrel adapter
      - Lite-On PA-1181-76, using a C5 power cord
  - 54Wh 4-cell Li-ion battery (NP50BAT-4-54)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5mm headphone/microphone jack
  - Dedicated 3.5mm microphone jack
- Storage
  - 1x M.2 PCIe NVMe Gen 4 SSD
  - 1x M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - MicroSD card reader (Realtek RTS5227S/OZ711LV2)

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

The position of the flash chip depends on the variant:

- 3050: U24, below the bottom DIMM slot.
- 3060: U55, left of the PCIe 4.0 M.2 slot.
