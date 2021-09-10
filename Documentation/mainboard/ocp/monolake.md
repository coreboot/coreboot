# OCP Mono Lake

This page describes coreboot support status for the [OCP] (Open Compute Project)
Mono Lake server platform.

## Introduction

OCP Mono Lake server platform is a component of multi-host server Yosemite-V1.
Facebook [introduced Yosemite] in 2015 and the [Mono Lake was accepted] to OCP in 2019.

Mono Lake server is a single socket BroadwellDE server.

Yosemite-V1 may host up to 4 Mono Lake servers (blades) in one sled.

The Yosemite-V1 system is in mass production. Facebook, Intel, ITRenew and partners
jointly develop Open System Firmware (OSF) solution on Mono Lake as an alternative
solution. The OSF solution is based on coreboot/FSP/LinuxBoot stack. A firmware binary
is available from ITRenew.

coreboot Mono Lake is supported on the coreboot version 4.11 branch.
The Broadwell-DE FSP v1.0 support was dropped after 4.11.


## Required blobs

Mono Lake server OSF solution requires:
- [FSP] blob: SysPro custom FSP is required to support the 16-core Broadwell-DE on Mono Lake
- Microcode: Available through github.com:otcshare/Intel-Generic-Microcode.git
- ME binary: Available under NDA with Intel

## Payload
- LinuxBoot: Linuxboot is the prefered coreboot payload for Mono Lake.
  It can be built following [All about u-root].

  Tested configuration:
    Linux Kernel v5.10.44
    [u-root Mono Lake pull request]
      u-root build command:
	GO111MODULE=off u-root -build=bb -uinitcmd=systemboot \
	 -files="${FLASHROMDIR}/flashrom:bin/flashrom" \
	 -files="${VPDDIR}/vpd:bin/vpd" \
	 core github.com/u-root/u-root/cmds/boot/{systemboot,pxeboot,boot} \
	 github.com/u-root/u-root/cmds/exp/{cbmem,dmidecode,modprobe,ipmidump,netbootxyz} \

    note: flashrom and vpd binaries must be precompiled

## Flashing coreboot

Mono Lake in-band BIOS firmware image update uses [flashrom]:
    flashrom -p internal:ich_spi_mode=hwseq -c "Opaque flash chip" --ifd \
			-i bios --noverify-all -w <path to coreboot image>

Yosemite-V1 OpenBMC BIOS firmware image update uses fw-util:
    fw-util slotx --update bios <path to coreboot image>

## Yosimite-V1 server controls

To power off/on a Mono Lake host:
    power-util slotx off
    power-util slotx on

To connect to coreboot and Linux console through SOL (Serial Over Lan):
    sol-util slotx

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
- VPDs affecting coreboot are in src/mainboard/ocp/monolake/mainboard.c.
  No coreboot VPD are required, uses safe defaults.

## Working features
The solution is developed using LinuxBoot payload with Linux kernel 5.2.9,
and [u-root] as initramfs.
- BMC integration:
    - BMC readiness check
    - IPMI commands
    - watchdog timer
    - POST complete pin acknowledgement
    - Check BMC version: ipmidump -device
- Early serial output
- ACPI tables: DMAR/DSDT/FACP/FACS/HPET/MCFG/SPMI/SRAT/SLIT/SSDT/XSDT
- FSP MRC cache support (Skipping memory training upon subsequent reboots)
- Versions
    - Check FSP version: cbmem | grep LB_TAG_PLATFORM_BLOB_VERSION
    - Check Microcode version: cat /proc/cpuinfo | grep microcode
- Devices:
    - Boot drive
    - All 5 data drives
    - NIC card
- SMBIOS:
    - Type 0 – BIOS Information
    - Type 1 – System Information
    - Type 2 – Baseboard Information
    - Type 3 – System Enclosure or Chassis
    - Type 4 – Processor Information
    - Type 7 – Cache Information
    - Type 16 – Physical Memory Array
    - Type 17 – Memory Device
    - Type 32 – System Boot Information
    - Type 38 – IPMI Device Information
    - Type 41 – Onboard Devices Extended Information
    - Type 127 – End-of-Table
- Power button
- u-root boot
- u-root pxeboot

## Stress and Performance tests passed

## Known issues

### Feature gaps
- flashrom command not able to update ME region
- ACPI BERT table
- PCIe hotplug through VPP (Virtual Pin Ports)
- RO_VPD region as well as other RO regions are not write protected
- Not able to selectively enable/disable core

## Technology

```eval_rst
+------------------------+---------------------------------------------+
| Processor (1 socket)   | Broadwell-DE                                |
+------------------------+---------------------------------------------+
| BMC                    | Aspeed AST 1250                             |
+------------------------+---------------------------------------------+
```

[OCP]: https://www.opencompute.org
[introduced Yosemite]: https://engineering.fb.com/2015/03/10/core-data/introducing-yosemite-the-first-open-source-modular-chassis-for-high-powered-microservers/
[Mono Lake was accepted]: https://www.opencompute.org/contributions?query=Tioga%20Pass%20v1.0
[FSP]: https://doc.coreboot.org/soc/intel/fsp/index.html
[u-root Mono Lake pull request]: https://github.com/u-root/u-root/pull/2045
[flashrom]: https://flashrom.org/Flashrom
[All about u-root]: https://github.com/linuxboot/book/tree/master/u-root
[u-root]: https://u-root.org/
[ChromeOS VPD]: https://chromium.googlesource.com/chromiumos/platform/vpd/+/master/README.md
