# System76 Lemur Pro 11 (lemp11)

## Specs

- CPU
  - Intel Core i5-1235U
  - Intel Core i7-1255U
- EC
  - ITE IT5570E running [System76 EC](https://github.com/system76/ec)
- Graphics
  - Intel Iris Xe Graphics
  - eDP 14.0" 1920x1080@60Hz LCD
  - 1x HDMI 2.1
  - 1x DisplayPort 1.4 over USB-C
- Memory
  - Channel 0: 8-GB on-board DDR4 (Samsung K4AAG165WA-BCWE)
  - Channel 1: 8-GB/16-GB/32-GB DDR4 SO-DIMM @ 3200 MHz
- Networking
  - M.2 NVMe/CNVi WiFi/Bluetooth (Intel Wi-Fi 6 AX200/201)
- Power
  - 65W (19V, 3.42A) AC barrel adapter (AcBel ADA012)
  - USB-C charging, compatible with 65W+ chargers
  - 73Wh 4-cell Lithium-ion battery (L140BAT-4)
- Sound
  - Realtek ALC256 codec
  - Internal speakers and microphone
  - Combined 3.5 mm headphone/microphone jack
  - HDMI, USB-C DisplayPort audio
- Storage
  - M.2 PCIe NVMe Gen 4 SSD
  - M.2 PCIe NVMe Gen 3 or SATA 3 SSD
  - MicroSD card reader (RTS5227S)
- USB
  - 1x USB Type-C with Thunderbolt 4
  - 1x USB 3.2 (Gen 2) Type-A
  - 1x USB 3.2 (Gen 1) Type-A
- Dimensions
    - 1.65cm x 32.2cm x 21.68cm, 1.15kg

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

The flash chip (U41) is left of the DIMM slot.
