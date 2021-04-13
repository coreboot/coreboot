# System76 Lemur Pro 10 (lemp10)

## Specs

- CPU
  - Intel Core i7-1165G7
  - Intel Core i5-1135G7
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 14.0" 1920x1080@60Hz LCD
  - 1x HDMI
  - 1x DisplayPort 1.4 over USB-C
- Memory
 - Channel 0: 8-GB on-board DDR4 (Samsung K4AAG165WA-BCWE x 8)
 - Channel 1: 8-GB/16-GB/32-GB DDR4 SO-DIMM @ 3200 MHz
- Networking
  - M.2 PCIe/CNVi WiFi/Bluetooth
    - Intel Wi-Fi 6 AX200/AX201
- Power
  - 65W (19V, 3.42A) AC barrel adapter
    - Included: AcBel ADA012, using a C7 power cord
  - USB-C charging compatible with 65W+ charger
  - 73Wh 4-cell Lithium-Ion battery
  - TI BQ24780S battery charge controller
- Sound
  - Realtek ALC293 codec
  - Internal speakers and microphone
  - Combined 3.5 mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 4 SSD
  - M.2 PCIe NVMe Gen 3 or SATA SSD
  - MicroSD card reader (RTS5227S)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.1 (3.1 Gen 2) Type-A
  - 1x USB 3.0 (3.2 Gen 1) Type-A

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

The flash chip (U33) is left of the DIMM slot.
