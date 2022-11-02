# OCP Delta Lake

This page describes coreboot support status for the [OCP] (Open Compute Project)
Delta Lake server platform.

## Introduction

OCP Delta Lake server platform is a component of multi-host server system
Yosemite-V3. Both [Delta Lake server design spec] and [Yosemite-V3 design
spec] were [OCP] accepted.

On the other hand, Wiwynn's Yosemite-V3 system and Delta Lake server product
along with its OSF implementation, which is based on FSP/coreboot/LinuxBoot
stack, was [OCP] accepted; For details, check:
- [The OCP blog]
- [The Wiwynn Press Release]
- [The Wiwynn's Yosemite-V3 product in OCP market place]
Wiwynn and 9Elements formed a partnership to offer the Wiwynn's Yosemite-V3
product and OSF for it.

Delta Lake server is a single socket Cooper Lake Scalable Processor (CPX-SP) server.
Intel Cooper Lake Scalable Processor was launched in Q2 2020.

Yosemite-V3 has multiple configurations. Depending on configurations, it may
host up to 4 Delta Lake servers (blades) in one sled.

The Yosemite-V3 system is in mass production. Meta, Intel and partners
jointly develop Open System Firmware (OSF) solution on Delta Lake as an alternative
solution. The OSF solution reached production quality for some use cases
in July, 2021.

## How to build

OSF code base is publicly available at
https://github.com/opencomputeproject/OpenSystemFirmware

Run following commands to build Delta Lake OSF image from scratch:
    git clone https://github.com/opencomputeproject/OpenSystemFirmware.git
    cd OpenSystemFirmware/Wiwynn/deltalake && ./download_and_build.sh

The Delta Lake OSF code base leverages [osf-builder] to sync down coreboot,
Linux kernel and u-root code from their upstream repo, and sync down needed
binary blobs. [osf-builder] also provides the top level build system.

Besides coreboot, the Delta Lake OSF solution includes following components:
- FSP blob: The blobs (Intel Cooper Lake Scalable Processor Firmware Support Package)
  is downloaded from https://github.com/intel/FSP/tree/master/CedarIslandFspBinPkg.
- Microcode: downloaded from github.com/intel/Intel-Linux-Processor-Microcode-Data-Files.
- ME ignition binary: downloaded from
	https://github.com/tianocore/edk2-non-osi/tree/master/Silicon/Intel/PurleySiliconBinPkg/MeFirmware
- ACM binaries: only required for CBnT enablement. Available under NDA with Intel.
- Payload: LinuxBoot is necessary when LinuxBoot is used as the coreboot payload.
  U-root as initramfs, is used in the joint development. It is built
  following [All about u-root].

The Delta Lake OSF solution is updated periodically to newer versions of
upstream coreboot code base and other components.

## How to verify Delta Lake OSF image

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

## How to work on coreboot for Delta Lake
After the OSF image for Delta Lake is built and verified, under
OpenSystemFirmware/Wiwynn/deltalake directory:
    cd src/osf-builder/projects/craterlake/coreboot

Run "git remote -v" to confirm the origin is from coreboot upstream repo.

Run "git branch -v" to know the confirmed working coreboot commit ID for the
Delta Lake OSF solution.

Fetch down the tip of coreboot upstream repo, run "make" to build a new OSF
image for Delta Lake, verify that it works.

Now you are in a familiar coreboot environment, happy coding!

## Firmware configurations
[ChromeOS VPD] is used to store most of the firmware configurations.
RO_VPD region holds default values, while RW_VPD region holds customized
values.

VPD variables supported are:
- firmware_version: This variable holds overall firmware version. coreboot
  uses that value to populate smbios type 1 version field.
- bmc_bootorder_override: When it's set to 1 IPMI OEM command can override boot
  order. The boot order override is done in the u-root LinuxBoot payload.
- systemboot_log_level: u-root package systemboot log levels, would be mapped to
  quiet/verbose in systemboot as that is all we have for now. 5 to 8 would be
  mapped to verbose, 0 to 4 and 9 would be mapped to quiet.
- VPDs affecting coreboot are listed/documented in [src/mainboard/ocp/deltalake/vpd.h].

## Features
The solution is developed using LinuxBoot payload with Linux kernel 5.2.9,
and [u-root] as initramfs.
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
    - Type 16 -- Physical Memory Array
    - Type 17 -- Memory Device
    - Type 19 -- Memory Array Mapped Address
    - Type 32 -- System Boot Information
    - Type 38 -- IPMI Device Information
    - Type 41 -- Onboard Devices Extended Information
    - Type 127 -- End-of-Table
- BMC integration:
    - BMC readiness check
    - IPMI commands
    - watchdog timer
    - POST complete pin acknowledgement
    - Check BMC version: ipmidump -device
- SEL record generation
- Converged Bootguard and TXT (CBnT)
    - TPM
    - Bootguard profile 0T
    - TXT
    - SRTM
    - DRTM (verified through tboot)
		- unsigned KM/BPM generation
		- KM/BPM signing
		- memory secret clearance upon ungraceful shutdown
- Early serial output
- port 80h direct to GPIO
- ACPI tables: APIC/DMAR/DSDT/EINJ/FACP/FACS/HEST/HPET/MCFG/SPMI/SRAT/SLIT/SSDT
- Skipping memory training upon subsequent reboots by using MRC cache
- BMC crash dump
- Error injection through ITP
- Versions
    - Check FSP version: cbmem | grep LB_TAG_PLATFORM_BLOB_VERSION
    - Check Microcode version: cat /proc/cpuinfo | grep microcode
- Devices:
    - Boot drive
    - All 5 data drives
    - NIC card
- Power button
- localboot
- netboot from IPv6
- RAS (SMI handlers not upstreamed)
    - EINJ/HEST
    - error injection through ITP
    - memory error handling
    - PCIe error handling
    - PCIe live error recovery (LER)

## Stress/performance tests passed
- OS warm reboot (1000 cycles)
- DC reboot (1000 cycles)
- AC reboot (1000 cycle)
- Mprime test (6 hours)
- StressAppTest (6 hours)
- Ptugen (6 hours)

## Performance on par with traditional firmware
- coremark
- FIO
- Iperf(IPv6)
- Linpack
- Intel MLC (memory latency and bandwidth)
- SpecCPU
- stream

## Other tests passed
- Power
- Thermal
- coreboot address sanitizer (both romstage and ramstage)
- Intel selftest tool (all errors analyzed; applicable errors clean)

## Known issues
- HECI access at OS run time:
    - spsInfoLinux64 command fail to return ME version
    - ptugen command fail to get memory power
- CLTT (Closed Loop Thermal Throttling, eg. thermal protection for DIMMs)
- ProcHot (thermal protection for processors)

## Feature gaps
- flashrom command not able to update ME region
- ACPI BERT table
- PCIe hotplug through VPP (Virtual Pin Ports)
- RO_VPD region as well as other RO regions are not write protected
- Not able to selectively enable/disable core

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
[Delta Lake server design spec]: https://www.opencompute.org/documents/delta-lake-1s-server-design-specification-1v05-pdf
[Yosemite-V3 design spec]: https://www.opencompute.org/documents/ocp-yosemite-v3-platform-design-specification-1v16-pdf
[The OCP blog]: https://www.opencompute.org/blog/open-system-firmware-for-ocp-server-deltalake-is-published
[The Wiwynn Press Release]: https://www.prnewswire.com/news-releases/wiwynn-successfully-implemented-open-system-firmware-on-its-ocp-yosemite-v3-server-301417374.html?tc=eml_cleartime
[The Wiwynn's Yosemite-V3 product in OCP market place]: https://www.opencompute.org/products/423/wiwynn-yosemite-v3-server
[osf-builder]: https://github.com/facebookincubator/osf-builder
[OCP virtual summit 2020]: https://www.opencompute.org/summit/virtual-summit/schedule
[flashrom]: https://flashrom.org/Flashrom
[All about u-root]: https://github.com/linuxboot/book/tree/master/u-root
[u-root]: https://u-root.org/
[ChromeOS VPD]: https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/README.md
[src/mainboard/ocp/deltalake/vpd.h]: https://review.coreboot.org/plugins/gitiles/coreboot/+/refs/heads/master/src/mainboard/ocp/deltalake/vpd.h
