# System76 Bonobo Workstation 14 (bonw14)

## Specs

- CPU
  - Intel Core i5-10600K
  - Intel Core i7-10700K
  - Intel Core i9-10900K
- Chipset
  - Intel Z490
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Grahpics
  - dGPU options
    - NVIDIA GeForce RTX 2080 Super
    - NVIDIA GeForce RTX 2070 Super
    - NVIDIA GeForce RTX 2060
  - eDP options
    - 17.3" 1920x1080x144Hz LCD (LG LP173WFG-SPB1)
    - 17.3" 3840x2160@60Hz LCD (AUO B173ZAN03.0)
  - 1x HDMI
  - 2x Mini DisplayPort 1.4
  - 2x DisplayPort 1.4 over USB-C
- Memory
  - Up to 128GB (4x32GB) quad-channel DDR4 SO-DIMMs @ 3200 MHz
- Networking
  - Gigabit Ethernet
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - 2x 280W (20V 14A) AC barrel adapter
    - Chicony A18-280P1A, using a C13 power cord
  - 97Wh 8-cell Lithium-Ion battery
- Sound
  - Realtek ALC1220 codec
  - 2x TI TAS5825MRHBR smart AMPs
  - Internal speakers (stereo + subwoofer) and microphone
  - Combined 3.5mm headphone/microphone jack
  - Combined 3.5mm microphone and S/PDIF jack
  - HDMI, Mini DisplayPort, USB-C DisplayPort audio
- Storage
  - 2x M.2 PCIe NVMe or SATA SSD
  - 1x M.2 PCIe NVMe-only SSD
  - 1x M.2 SATA-only SSD
  - SD card reader (RTS5260)
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 1x USB 3.2 Gen 2 Type-C with DisplayPort 1.4
  - 1x USB 3.2 Gen 2x2 Type-C
  - 3x USB 3.2 Gen 2 Type-A
- Dimensions
  - 4.343cm x 39.903cm x 31.902cm, 3.80kg

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

External flashing via ISP requires removing the board from the chassis.
The flash chip (U16) is next to the PCH.
