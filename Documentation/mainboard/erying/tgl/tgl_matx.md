# Erying Polestar G613 Pro

This page describes how to run coreboot on the [Erying Polestar G613 Pro].

## Required proprietary blobs

To build full working image of coreboot, the following blobs are required:

```{eval-rst}
+-----------------+---------------------------------+----------------------+
| Binary file     | Apply                           | Required / Optional  |
+=================+=================================+======================+
| Microcode       | CPU Microcode                   | Required (see notes) |
+-----------------+---------------------------------+----------------------+
| FSP-M & FSP-S   | Intel Firmware Support Package  | Required             |
+-----------------+---------------------------------+----------------------+
| ME              | Intel Management Engine         | Required (see notes) |
+-----------------+---------------------------------+----------------------+
| FD              | Intel Flash Descriptor          | Required (see notes) |
+-----------------+---------------------------------+----------------------+
```

Microcode for D0 stepping cannot be generated from the tree.
Therefore, including external microcode is **mandatory** for boards sold with
D0 stepping (Engineering Sample).

Maintainer of this port had included publicly-available [microcodes] in
`3rdparty/blobs` coreboot repository, which are being pulled as submodule.

Microcode binary for D0 stepping is called
`cpu806D0_platC2_ver00000054_2021-05-07_PRD_B0F9E245.bin`.

It should be selected by going to:
`Chipset -> Include CPU microcode in CBFS (Include external microcode binary)`.

Failure to choose an appropriate microcode will result in:
- MCE (Machine Check Exception) errors.
- Issues with APIC, resulting in random hangs.
- Unstable system RAM, leading to bit flips and data corruption.

There are no extra steps required for FSP.
Both SKUs work perfectly with FSP Intel publishes in their public repository.
coreboot automatically pulls FSP binary from `3rdparty/fsp` submodule
at build time, which points to TigerLakeFspBinPkg/Client package in
official Intel FSP repository.

## Flashing coreboot

### Internally

Vendor of this motherboard hasn't locked any flash regions, resulting in
[flashrom] having full access to the SPI chip.
Assuming that user had booted Linux with `iomem=relaxed`, they can:
- Flash coreboot from stock firmware
- Flash stock firmware from coreboot
- Update coreboot build to a newer version

Without opening the case and connecting the SPI flasher.
Please note that you will need to use [flashrom] v1.3.0 or newer,
as older versions won't detect the chipset.

If you're using [flashrom] or [flashprog] (fork of flashrom), you can
skip extracting `SI_BIOS` and `SI_ME` regions from your ROM, and flash
coreboot to `SI_BIOS` region by issuing the following command:
`flashrom -p internal --ifd -i SI_BIOS -w ./build/coreboot.rom`

### Externally

SPI chip on this mainboard is located right underneath the PCH heatsink.
Due to it's location, it's likely that you will need to move (or remove)
the PCH (Platform Controller Hub) heatsink in order to clip it properly.

Vendor populated this board with Winbond W25Q128FV chip in SOIC-8 package.
Although the stock chip is 16MiB, it is possible to replace it with 32MiB
equivalent if user desires to use LinuxBoot payload instead of EDK2.

Please note that SPI voltage on this board is standard 3.3V,
despite using mobile SoC and PCH.

## Tested and working

- RS232 serial output from IT8613E for debugging (cbmem, Linux)
- Fan control from userspace (IT8613E Environment Controller)
- USB2.0 and 3.0
- HDMI, DisplayPort (iGPU, including audio)
- Realtek RTL8111 (GbE NIC)
- Realtek ALC897 (integrated audio)
- PCIe x16 4.0 (SoC)
- PCIe x1 3.0 (PCH)
- M.2 x4 4.0 (SoC)
- M.2 x4 3.0 (PCH)
- M.2 NGFF (WiFi)
- 4x SATA3 ports
- IOMMU/VT-x (PCIe passthrough)
- PCIe ReBAR (Resizable BAR)
- Intel PTT (fTPM 2.0)

## Work in progress, broken, issues

- XMP Profiles (some people reported issues, despite successful tests).
You can enable it by setting `SpdProfileSelected` in `romstage_fsp_params.c`.
See [FSP XMP flags] for configuration options, proceed with caution.
- Sleep states (which were broken on stock as well, RAM loses power in S3).
- Automatic fan control (fans will always spin at 50% - see below).
- PCIe ASPM (results in AER spam in dmesg).

Please ensure to:
- Disable sleep state in your OS.
- Configure automatic fan control using pwmconfig
(`modprobe it87 force_id=0x8603`).
- Append `pcie_aspm=off` to your kernel commandline to avoid dmesg spam.

## Notes

1. Required blobs, if flashing the entire flash chip.
   They can be skipped safely if you are planning on flashing
   only the `SI_BIOS` region.
    - Intel Flash Descriptor (IFD): `fd.bin`
    - Intel Management Engine (ME): `me.bin`

    Both blobs included in `3rdparty/blobs` repository were extracted
    from vendor's firmware (available to download on their website).
    IFD region has been modified using `ifdtool` to set
    `MeAltDisable` flag.

2. Modifications

    It is possible to replace Winbond 16MB chip with 32MB equivalent,
    which would allow you to use LinuxBoot or implement
    RO + A/B VBOOT update scheme.

3. Payload and pre-OS display output

    If you are using an external graphics card (AMD, Nvidia, Intel Arc),
    you will see output in your OS as soon as kernel initializes the
    card (called "modprobing" in Linux) regardless of payload you chose.

    This board was tested with following payloads:
    - EDK2
    - U-Boot
    - LinuxBoot (U-Root + Linux kernel)

If you're planning to primarly use an external card, disable iGPU by
enabling `Chipset -> Disable Integrated GFX Controller (0:2:0)`
and use [MrChromebox's EDK2] tree.

In order to enable loading Option ROMs from PCIe devices, enable:
`Payload -> Load and Execute OpROMs on PCIe devices`

This functionality has been tested with following graphics cards,
with following results:
- Nvidia GeForce RTX3080, RTX3090: Works perfectly.
- AMD Radeon RX6600XT, RX7800XT: Works, but requires
`Extend resource window for PCIe devices above 4G` if `Support PCIe
Resizable BARs` is enabled.
- Intel Arc A580: Works, but option
`Extend resource window for PCIe devices above 4G` is **mandatory**.

## Specification

```{eval-rst}
+------------------+------------------------------+
| SoC              | Intel TigerLake Halo         |
+------------------+------------------------------+
| PCH              | Intel HM570                  |
+------------------+------------------------------+
| Super I/O        | ITE IT8613E                  |
+------------------+------------------------------+
| SPI              | Winbond W25Q128FV 16MiB 3.3V |
+------------------+------------------------------+
| NIC              | Realtek RTL8111              |
+------------------+------------------------------+
| Audio            | Realtek ALC897               |
+------------------+------------------------------+
```

[Erying Polestar G613 Pro]: https://www.erying.cc/products-detail/id-97.html
[microcodes]: https://github.com/platomav/CPUMicrocodes/tree/master/Intel
[FSP XMP Flags]: https://github.com/intel/FSP/blob/master/TigerLakeFspBinPkg/Client/Include/FspmUpd.h#L586-L591
[MrChromebox's EDK2]: https://github.com/MrChromebox/edk2
[flashrom]: https://flashrom.org/
[flashprog]: https://flashprog.org/wiki/Flashprog
