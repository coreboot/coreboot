# OCP Delta Lake

This page describes coreboot support status for the [OCP] (Open Compute Project)
Delta Lake server platform.

## Introduction

OCP Delta Lake server platform is a component of multi-host server system
Yosemite-V3. Both were announced by Facebook and Intel in [OCP virtual summit 2020].

Delta Lake server is a single socket Cooper Lake Scalable Processor server.

Yosemite-V3 has multiple configurations. Depending on configurations, it may
host up to 4 Delta Lake servers in one sled.

Yosemite-V3 and Delta Lake are currently in DVT phase. Facebook, Intel and partners
jointly develop FSP/coreboot/LinuxBoot stack on Delta Lake as an alternative solution.

## Required blobs

This board currently requires:
- FSP blob: The blob (Intel Cooper Lake Scalable Processor Firmware Support Package)
  is not yet available to the public. It will be made public some time after the MP
  (Mass Production) of CooperLake Scalable Processor when the FSP is mature.
- Microcode: Not yet available to the public.
- ME binary: Not yet available to the public.

## Payload
- LinuxBoot: This is necessary only if you use LinuxBoot as coreboot payload.
  U-root as initramfs, is used in the joint development. It can be built
  following [All about u-root].

## Flashing coreboot

To do in-band FW image update, use [flashrom]:
    flashrom -p internal:ich_spi_mode=hwseq -c "Opaque flash chip" --ifd \
			-i bios --noverify-all -w <path to coreboot image>

From OpenBMC, to update FW image:
    fw-util slotx --update bios <path to coreboot image>

To power off/on the host:
    power-util slotx off
    power-util slotx on

To connect to console through SOL (Serial Over Lan):
    sol-util slotx

## Working features
The solution is developed using LinuxBoot payload with Linux kernel 5.2.9, and [u-root]
as initramfs.
- SMBIOS:
    - Type 0 -- BIOS Information
    - Type 1 -- System Information
    - Type 2 -- Baseboard Information
    - Type 3 -- System Enclosure or Chassis
    - Type 4 -- Processor Information
    - Type 8 -- Port Connector Information
    - Type 9 -- PCI Slot Information
    - Type 11 -- OEM String
    - Type 13 -- BIOS Language Information
    - Type 16 -- Physical Memory Array
    - Type 19 -- Memory Array Mapped Address
    - Type 127 -- End-of-Table

- BMC integration:
    - BMC readiness check
    - IPMI commands
    - watchdog timer
    - POST complete pin acknowledgement
- SEL record generation
- Early serial output
- port 80h direct to GPIO
- ACPI tables: APIC/DSDT/FACP/FACS/HPET/MCFG/SPMI/SRAT/SLIT/SSDT
- Skipping memory training upon subsequent reboots by using MRC cache
- BMC crash dump
- Error injection through ITP

## Firmware configurations
[ChromeOS VPD] is used to store most of the firmware configurations.
RO_VPD region holds default values, while RW_VPD region holds customized
values.

VPD variables supported are:
- firmware_version: This variable holds overall firmware version. coreboot
  uses that value to populate smbios type 1 version field.

## Known issues
- Even though CPX-SP FSP is based on FSP 2.2 framework, it does not
  support FSP_USES_CB_STACK. An IPS ticket is filed with Intel.
- VT-d is not supported. An IPS ticket is filed with Intel.
- PCIe bifuration is not supported. An IPS ticket is filed with Intel.
- ME based power capping. This is a bug in ME. An IPS ticket is filed
  with Intel.
- RO_VPD region as well as other RO regions are not write protected.
- HECI is not set up correctly, so BMC is not able to get PCH and DIMM
  temperature sensor readings.

## Feature gaps
- Delta Lake DVT is not supported, as we only have Delta Lake EVT servers
  at the moment.
- SMBIOS:
    - Type 7 -- Cache Information
    - Type 17 -- Memory Device
    - Type 38 -- IPMI Device Information
    - Type 41 -- Onboard Devices Extended Information
- ACPI:
    - DMAR
- PFR/CBnT

## Technology

```eval_rst
+------------------------+---------------------------------------------+
| Processor (1 socket)   | Intel Cooper Lake Scalable Processor        |
+------------------------+---------------------------------------------+
| BMC                    | Aspeed AST 2500                             |
+------------------------+---------------------------------------------+
| PCH                    | Intel Lewisburg C621                        |
+------------------------+---------------------------------------------+
```

[OCP]: https://www.opencompute.org
[OCP virtual summit 2020]: https://www.opencompute.org/summit/virtual-summit/schedule
[flashrom]: https://flashrom.org/Flashrom
[All about u-root]: https://github.com/linuxboot/book/tree/master/u-root
[u-root]: https://u-root.org/
[ChromeOS VPD]: https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/README.md
