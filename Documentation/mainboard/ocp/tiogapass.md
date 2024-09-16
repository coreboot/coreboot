# OCP Tioga Pass

This page describes coreboot support status for the [OCP] (Open Compute Project)
Tioga Pass server platform.

## Introduction

OCP Tioga Pass server platform was contributed by Facebook, and was accepted
in 2019. The design collateral including datasheet can be found at [OCP Tioga Pass].

Since complete EE design collateral is open sourced, anyone can build server
as-is or a variant based on the original design. It can also be purchased from [OCP Market Place].
An off-the-shelf version is available, as well as rack ready version. With the
off-the-shelf version, the server can be plugged into wall power outlet.

With the off-the-shelf version of Tioga Pass, a complete software solution is
available. [Off-the-shelf Host Firmware] takes the approach of UEFI/Linuxboot.

coreboot as of release 4.13 is a proof-of-concept project between Facebook,
Intel, Wiwynn and Quanta. The context is described at [OCP Tioga Pass POC Blog].

## Required blobs

This board currently requires:
- FSP blob: The blob (Intel Skylake Scalable Processor Firmware Support Package)
  is not yet available to the public. The binary is at POC status, hopefully
  someday an IBV is able to obtain the privilege to maintain it.
- Microcode: `3rdparty/intel-microcode/intel-ucode/06-55-04`
- ME binary: The binary can be extracted from [Off-the-shelf Host Firmware].

## Payload
- Linuxboot: This is necessary only if you use Linuxboot as coreboot payload.
  U-root as initramfs, is used in the POC activity. It can be extracted from
  [Off-the-shelf Host Firmware], or it can be built following [All about u-root].

## Flashing coreboot

To do in-band FW image update, use [flashrom]:
    flashrom -p internal:ich_spi_mode=hwseq -c "Opaque flash chip" --ifd \
			-i bios --noverify-all -w <path to coreboot image>

From OpenBMC, to update FW image:
    fw-util mb --force --update <path to coreboot image>

To power off/on the host:
    power-util mb off
    power-util mb on

To connect to console through SOL (Serial Over Lan):
    sol-util mb

## Known issues / feature gaps
- C6 state is not supported. Workaround is to disable C6 support through
  target OS and Linuxboot kernel parameter, such as "cpuidle.off=1".
- SMI handlers are not implemented.
- xSDT tables are not fully populated, such as processor/socket devices,
  PCIe bridge devices.
- There is boot stability issue. Occasionally the boot hangs at ramstage
  with following message "BIOS PCU Misc Config Read timed out."
- If [CB 40500 patchset] is not merged, when PCIe riser card is used,
  boot fails.
- PCIe devices connected to socket 1 may not work, because FSP
  does not support PCIe topology input for socket 1.k
- SMBIOS type 7 and type 17 are not populated.

## Working
The solution was developed using Linuxboot payload. The Linuxboot
kernel versions tried are 4.16.18 and 5.2.9. The initramfs image is
u-root.
- Most SMBIOS types
- BMC integration:
    - BMC readiness check
    - IPMI commands
    - watchdog timer
    - POST complete pin acknowledgement
- SEL record generation
- Early serial output
- port 80h direct to GPIO
- ACPI tables: APIC/DMAR/DSDT/FACP/FACS/HPET/MCFG/SPMI/SRAT/SLIT/SSDT

## Technology

```{eval-rst}
+------------------------+---------------------------------------------+
| Processor (2 sockets)  | Intel Skylake Scalable Processor LGA3647    |
+------------------------+---------------------------------------------+
| BMC                    | Aspeed AST 2500                             |
+------------------------+---------------------------------------------+
| PCH                    | Intel Lewisburg C621                        |
+------------------------+---------------------------------------------+
```

[flashrom]: https://flashrom.org/Flashrom
[OCP]: https://www.opencompute.org/
[OCP Tioga Pass]: http://files.opencompute.org/oc/public.php?service=files&t=6fc3033e64fb029b0f84be5a8faf47e8
[OCP Market Place]: https://www.opencompute.org/products/109/wiwynn-tioga-pass-advanced-2u-ocp-server-up-to-768gb-12-dimm-slots-4-ssds-for-io-performance
[Off-the-shelf Host Firmware]: https://book.linuxboot.org/case_studies/TiogaPass.html
[OCP Tioga Pass POC Blog]: https://www.opencompute.org/blog/linux-firmware-boots-up-server-powered-by-intelr-xeonr-scalable-processor
[All about u-root]: https://book.linuxboot.org/u-root.html
[CB 40500 patchset]: https://review.coreboot.org/c/coreboot/+/40500
