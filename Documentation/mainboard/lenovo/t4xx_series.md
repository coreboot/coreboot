# Lenovo T4xx series disassembly instructions

A skilled engineer takes around 40 minutes to disassemble, flash and reassemble
the whole device.

Read their [Hardware Maintenance Manual](thinkpad_hmm.md) for detailed steps.

## Steps to access the flash IC

* Unplug the main battery
* Remove the harddisk, CDROM, ExpressCard, SIM-card, SDcard, SmartCard, ...
* Open the bottom flap and remove the keyboard screw
* Remove the keyboard
* Remove the screen
* Remove the top enclosure
* Remove the CMOS battery
* Remove the speakers
* Remove WWAN and WIFI card
* Remove the CPU fan
* Unplug the power cable
* Remove the bottom enclosure
* Flip the mainboard and remove the main frame

## Docking stations
The following docking stations are supported by coreboot:
* Type 2505
  * VGA, Ethernet, Modem, PS2, 4 USB Ports
  * Dock ID on pc87382 reads as: 2
* Type 2504
  * Serial, LPT, LEDs, Audio, DVI, VGA, Ethernet, Modem, PS2, 4 USB Ports
  * Dock ID on pc87382 reads as: 1
  * PNP IO address of SuperIO pc87384: 0x2e

There's no hotplug support for LPT and Serial on Type 2504.

The Dock ID reads as 7 if no dock is connected.
