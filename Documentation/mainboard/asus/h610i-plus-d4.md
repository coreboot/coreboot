# ASUS PRIME H610i-PLUS
This is a Mini-ITX LGA1700 (Alder Lake/Raptor Lake) motherboard, using the H610 chipset and
DDR4 RAM. It's a close relative of the H610M-K, and like it is also sold in DDR4 and DDR5
variants.

## Variants
- *ASUS PRIME H610i-PLUS **D4***: uses DDR4 RAM, supported
- *ASUS PRIME H610i-PLUS* (no "D4"): uses DDR5 RAM, not currently supported by this port

## Flashing
This mainboard uses a standard 3.3V SOIC-8 SPI flash chip. The vendor firmware enables write
protection, thus for initial installation an external programmer is required. Thereafter,
coreboot can be updated internally using `flashrom -p internal`.

An external programmer can be connected using an ordinary chip clip, but for development or
testing, it can be more convenient to flash via the TPM header. A pinout can be found on Page
1-4 of the board's User's Manual - to select the flash chip, connect your CS line to
F_SPI_CS0#_R. An adapter cable can be made using a 2x7-pin 2.0mm female header, or a set of
2.0mm jumper wires. Beware, despite its similar appearance, this TPM header pinout is NOT
compatible with the pinout found on the MSI Z690A and Z790P boards (adapters for flashing those
boards over the SPI TPM header will not work on ASUS boards).

## Feature Support
### Working:
- Console over onboard serial port
- PS/2 keyboard
- Port 80 POST codes over ASUS debug header
- All USB ports, including USB3 working, except front USB2
- All outputs (DP, HDMI, VGA) for iGPU
- M.2 slot
- PCIe WiFi card in WiFi slot
- Onboard Ethernet
- PCIe ASPM and clock power management for all devices
- x16 PCIe slot
- All SATA ports
- Hard drive indicator LED
- All audio including front panel
- Fan control
- ME disable with HAP bit in IFD
- HSPHY-in-FMAP when ME is disabled

### Untested:
- CNVi WiFi card in WiFi slot
- SPI TPM
- Front USB2 ports (did not have an adapter on hand to test)
- Status LEDs in actual error states (they do show a normal status normally)

### Not working:
- S3 sleep