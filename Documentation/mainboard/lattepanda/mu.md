# LattePanda Mu

This page describes how to run coreboot on the [LattePanda Mu], an
Intel Alder Lake-N system-on-module (SoM).

## Overview

The LattePanda Mu is a compute module: the N100/N305 SoC, LPDDR5 memory
and the primary power sequencing live on a small SO-DIMM-style module,
while all user-facing I/O is broken out by a separate carrier board.
Because of this split, the coreboot port is organized as a common
baseboard (the SoM) plus one variant per carrier:

```{eval-rst}
+-----------------------------------+-----------------+-----------------------------+
| Board                             | Variant dir     | Description                 |
+===================================+=================+=============================+
| ``BOARD_LATTEPANDA_MU``           | ``baseboard``   | SoM only, no carrier        |
+-----------------------------------+-----------------+-----------------------------+
| ``BOARD_LATTEPANDA_MU_DFR1141``   | ``dfr1141``     | Full "Lite Carrier" eval    |
|                                   |                 | board (DFR1141)             |
+-----------------------------------+-----------------+-----------------------------+
| ``BOARD_LATTEPANDA_MU_DFR1142``   | ``dfr1142``     | Reduced carrier (DFR1142)   |
+-----------------------------------+-----------------+-----------------------------+
```

The baseboard `devicetree.cb` describes only what is fixed on the module
(SoC, memory, IT8613E Super I/O, eSPI, CRB TPM). Each carrier variant adds an
`overridetree.cb` and its own GPIO table for the peripherals wired on that
carrier. This makes the port a convenient starting point for **custom
carrier modules**: copy one of the `variants/dfr114x` directories, adjust
the GPIO table, `overridetree.cb` and any carrier-specific HSIO/PCIe
mapping, and add a matching `BOARD_LATTEPANDA_MU_*` Kconfig entry.

## Required proprietary blobs

```{eval-rst}
+---------------+---------------------------------+----------------------+
| Binary file   | Apply                           | Required / Optional  |
+===============+=================================+======================+
| FSP-M & FSP-S | Intel Firmware Support Package  | Required             |
+---------------+---------------------------------+----------------------+
| ME            | Intel Management Engine         | Required             |
+---------------+---------------------------------+----------------------+
| IFD           | Intel Flash Descriptor          | Required (see below) |
+---------------+---------------------------------+----------------------+
| VBT           | Video BIOS Table                | Optional (display)   |
+---------------+---------------------------------+----------------------+
```

FSP is pulled automatically from the `3rdparty/fsp` submodule
(Alder Lake-N / IoT package). The ME and IFD regions must be preserved
from, or extracted from, the stock firmware (see below).

## DFR1141 HSIO configuration and the Flash Descriptor

On the DFR1141 carrier, SoC HSIO lanes 10 and 11 are multiplexed between
the x4 slot connector and a SATA port. Which function is active is
determined by **soft straps in the Intel Flash Descriptor**, so coreboot
cannot switch it at runtime — the descriptor built into the image must
match the desired configuration. `BOARD_LATTEPANDA_MU_DFR1141` therefore
exposes an HSIO choice in Kconfig:

```{eval-rst}
+------------------------------+------------------------------------------------+
| Kconfig choice               | Behavior                                       |
+==============================+================================================+
| ``DFR1141_HSIO_PCIE_X4``     | All four lanes routed to the x4 slot. SATA is  |
|                              | disabled in ramstage. Uses the vendor "SR-B"   |
|                              | descriptor straps.                             |
+------------------------------+------------------------------------------------+
| ``DFR1141_HSIO_PCIE_X2_SATA``| HSIO10+11 routed to SATA; the slot runs x2.    |
|                              | Uses the vendor "SR-B-SATA" descriptor straps. |
+------------------------------+------------------------------------------------+
```

The HSIO Kconfig choice only controls the ramstage side of the mux (it
disables SATA for the x4 option). It does **not** automatically select the
matching Flash Descriptor (as coreboot does not bundle vendor descriptors),
so you must supply one and point coreboot at it yourself.

The descriptor is extracted from the corresponding stock firmware image,
which is published by the vendor on GitHub in the
[LattePanda Mu BIOS repository]. The `Default` (PCIe) branch provides the
straps for the x4 configuration and the `SATA` branch provides the straps
for the x2 + SATA configuration. Extract the descriptor region from the
matching stock image with `ifdtool`:

```text
ifdtool -p adl -x S70NC1R200-16G-A.bin  # or the -SATA image for x2 + SATA
```

This writes `flashregion_0_flashdescriptor.bin`. Store it wherever you
like (outside the tree is fine) and tell coreboot to use it by setting
`CONFIG_IFD_BIN_PATH` in your `.config`, e.g.:

```text
CONFIG_IFD_BIN_PATH="../lattepanda-blobs/flashdescriptor_pcie_x4.bin"
```

Re-run `make` after editing `.config` so the new value takes effect
(alternatively set it interactively under
`Chipset -> Add Intel descriptor.bin file`).

Because the HSIO mux lives in the descriptor, **using a descriptor that
does not match the selected HSIO Kconfig option will result in a
non-functional slot and/or SATA port.**

## Building coreboot

Select the board matching your hardware:

- `LattePanda Mu (SOM only)` — module on a custom/unknown carrier
- `LattePanda Mu with DFR1141 full eval carrier`
- `LattePanda Mu with DFR1142 lite carrier`

Also select the soldered memory density under
`Mainboard -> On-board LP5X memory size` (8 GB or 16 GB); the matching
SPD image is included in CBFS.

For DFR1141, pick the HSIO configuration under
`Mainboard -> HSIO configuration (x4 slot mux)` and set
`CONFIG_IFD_BIN_PATH` to the matching descriptor as described above.

## Flashing coreboot

The stock firmware does not lock the flash regions, so coreboot can be
flashed internally with [flashprog] (or [flashrom]). To update only the
BIOS region and leave the descriptor and ME intact:

```text
flashprog -p internal --ifd -i bios -w ./build/coreboot.rom -N
```

The SoM's SPI flash can also be programmed externally with an SPI
programmer (3.3V) if the system will not boot.

## Specification

```{eval-rst}
+------------------+--------------------------------------------------+
| SoC              | Intel Alder Lake-N (N100 / N305)                 |
+------------------+--------------------------------------------------+
| Memory           | Soldered LPDDR5x on module (8 GB or 16 GB)       |
+------------------+--------------------------------------------------+
| Super I/O / EC   | ITE IT8613E (power sequencing, fan control)      |
+------------------+--------------------------------------------------+
| TPM              | Intel PTT (fTPM 2.0) via CRB                     |
+------------------+--------------------------------------------------+
| SPI flash        | 16 MiB on module                                 |
+------------------+--------------------------------------------------+
| Super I/O UART   | Serial console on IT8613E COM1                   |
+------------------+--------------------------------------------------+
| Payload          | EDK2                                             |
+------------------+--------------------------------------------------+
```

[LattePanda Mu]: https://www.lattepanda.com/lattepanda-mu
[LattePanda Mu BIOS repository]: https://github.com/LattePandaTeam/LattePanda-Mu/tree/main/Softwares/BIOS
[flashprog]: https://flashprog.org/wiki/Flashprog
[flashrom]: https://flashrom.org/
