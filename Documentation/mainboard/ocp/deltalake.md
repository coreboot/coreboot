# OCP Delta Lake

This page describes coreboot support status for the [OCP] (Open Compute Project)
Delta Lake server platform. This page is updated following each 4-weeks
build/test/release cycle.

## Introduction

OCP Delta Lake server platform is a component of multi-host server system
Yosemite-V3. Both were announced by Facebook and Intel in [OCP virtual summit 2020].

Delta Lake server is a single socket Cooper Lake Scalable Processor server.

Yosemite-V3 has multiple configurations. Depending on configurations, it may
host up to 4 Delta Lake servers in one sled.

The Yosemite-V3 program has reached DVT exit. Facebook, Intel and partners
jointly develop FSP/coreboot/LinuxBoot stack on Delta Lake as an alternative
solution. This development is moving toward EVT exit equivalent status.

## Required blobs

This board currently requires:
- FSP blob: The blob (Intel Cooper Lake Scalable Processor Firmware Support Package)
  is not yet available to the public. It will be made public some time after the MP
  (Mass Production) of CooperLake Scalable Processor when the FSP is mature.
- Microcode: Available through github.com:otcshare/Intel-Generic-Microcode.git.
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
    - Type 7 -- Cache Information
    - Type 8 -- Port Connector Information
    - Type 9 -- PCI Slot Information
    - Type 11 -- OEM String
    - Type 32 -- System Boot Information
    - Type 38 -- IPMI Device Information
    - Type 127 -- End-of-Table
- BMC integration:
    - BMC readiness check
    - IPMI commands
    - watchdog timer
    - POST complete pin acknowledgement
    - Check BMC version: ipmidump -device
- SEL record generation
- Early serial output
- port 80h direct to GPIO
- ACPI tables: APIC/DMAR/DSDT/FACP/FACS/HPET/MCFG/SPMI/SRAT/SLIT/SSDT
- Skipping memory training upon subsequent reboots by using MRC cache
- BMC crash dump
- Error injection through ITP
- Versions
    - Check FSP version: cbmem | grep LB_TAG_PLATFORM_BLOB_VERSION
    - Check Microcode version: cat /proc/cpuinfo | grep microcode
- Devices:
    - Boot drive
    - NIC card
    - All 5 data drives
- Power button
- localboot
- netboot from IPv6
- TPM

## Stress/performance tests passed
- OS warm reboot (300 cycles)
- DC reboot (300 cycles)
- AC reboot (300 cycle)
- Mprime test (6 hours)
- StressAppTest (6 hours)
- Ptugen (6 hours)
- MLC (Intel Memory Latency Check)
- Linkpack
- Iperf(IPv6)
- FIO

## Firmware configurations
[ChromeOS VPD] is used to store most of the firmware configurations.
RO_VPD region holds default values, while RW_VPD region holds customized
values.

VPD variables supported are:
- firmware_version: This variable holds overall firmware version. coreboot
  uses that value to populate smbios type 1 version field.
- DeltaLake specific VPDs: check mb/ocp/deltalake/vpd.h.

## Known issues
- spsInfoLinux64 command fail to return ME version.
- fwts test failures related to mtrr.
- kernel error message related to SleepButton ACPI event.

## Feature gaps
- SMBIOS:
    - Type 16 -- Physical Memory Array
    - Type 17 -- Memory Device
    - Type 19 -- Memory Array Mapped Address
    - Type 41 -- Onboard Devices Extended Information
- Verified measurement through CBnT
- Boot guard of CBnT
- RO_VPD region as well as other RO regions are not write protected.

## Technology

```eval_rst
+------------------------+---------------------------------------------+
| Processor (1 socket)   | Intel Cooper Lake Scalable Processor        |
+------------------------+---------------------------------------------+
| BMC                    | Aspeed AST 2500                             |
+------------------------+---------------------------------------------+
| PCH                    | Intel Lewisburg C620 Series                 |
+------------------------+---------------------------------------------+
```

[OCP]: https://www.opencompute.org
[OCP virtual summit 2020]: https://www.opencompute.org/summit/virtual-summit/schedule
[flashrom]: https://flashrom.org/Flashrom
[All about u-root]: https://github.com/linuxboot/book/tree/master/u-root
[u-root]: https://u-root.org/
[ChromeOS VPD]: https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/README.md
