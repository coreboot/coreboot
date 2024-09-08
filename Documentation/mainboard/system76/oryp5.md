# System76 Oryx Pro 5 (oryp5)

## Specs

- CPU
  - Intel Core i7-8750H
  - Intel Core i7-9750H
- EC
  - ITE8587E running <https://github.com/system76/ec>
- Graphics
  - Intel UHD Graphics 630
  - NVIDIA GeForce RTX 2080/2070/2060
  - eDP 16.1" or 17.3" 1920x1080 @ 144 Hz LCD
  - HDMI, Mini DisplayPort 1.3, and DisplayPort 1.3 over USB-C
- Memory
  - Channel 0: 8-GB/16-GB/32-GB DDR4 SO-DIMM
  - Channel 1: 8-GB/16-GB/32-GB DDR4 SO-DIMM
- Networking
  - Gigabit Ethernet
  - Intel Dual Band Wireless-AC 9560 Wireless LAN (802.11ac) + Bluetooth
- Power
  - 180W (19.5V, 9.23A) AC adapter
  - 62Wh 4-cell battery
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
  - 2x USB 3.1 Gen2 Type-C
  - 2x USB 3.1 Gen1 Type-A

## Building coreboot

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.system76_oryp5
make
```

## Flashing coreboot

```{eval-rst}
+---------------------+---------------------+
| Type                | Value               |
+=====================+=====================+
| Socketed flash      | no                  |
+---------------------+---------------------+
| Vendor              | GigaDevice          |
+---------------------+---------------------+
| Model               | GD25Q127C/GD25Q128C |
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
The IC is located under the touchpad.
