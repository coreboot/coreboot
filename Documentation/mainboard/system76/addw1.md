# System76 Adder Workstation 1 (addw1)

## Specs

- CPU
  - Intel Core i7-9750H
  - Intel Core i9-9980HK
- Chipset
  - Intel HM370
- EC
  - ITE IT8587E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel UHD Graphics 630
  - NVIDIA GeForce RTX 2070
  - eDP 15.6" 3840x2160 OLED (Samsung ATNA56WR06)
  - 1x HDMI
  - 1x Mini DisplayPort 1.3
  - 1x DisplayPort 1.3 over USB-C
- Memory:
  - Up to 64GB dual-channel DDR4 @ 2666 MHz, or
  - Up to 32GB dual-channel DDR4 @ 3000 MHz
- Networking
  - Gigabit Ethernet
  - Intel Wireless-AC
- Power
  - 230W (19.5V, 11.8A) AC adapter
  - Removable 62Wh 6-cell battery
- Sound
  - Realtek ALC1220 codec
  - TAS5825MRHBR smart AMP
  - Internal speakers and microphone
  - Combined headphone and microphone 3.5mm jack
  - Combined microphone and S/PDIF 3.5mm jack
  - HDMI, Mini DisplayPort, USB-C DP audio
- Storage
  - M.2 PCIe/SATA SSD1
  - M.2 PCIe/SATA SSD2
  - 2.5" SATA HDD/SSD
  - RTS5250 SD card reader
- USB
  - 1x USB Type-C with Thunderbolt 3
  - 1x USB 3.1 Gen2 Type-C
  - 3x USB 3.1 Gen1 Type-A

## Flashing coreboot

```eval_rst
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | Macronix            |
+---------------------+---------------------+
| Model               | MX25L12873F         |
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

The flash chip (U61) is next to the battery connector.
