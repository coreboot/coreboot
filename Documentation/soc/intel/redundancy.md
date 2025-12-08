# Intel Top Swap based A and B redundancy

This document describes a simple firmware A and B redundancy scheme based on
Intel PCH Top Swap. The scheme maintains two firmware slots and uses the Top
Swap control bit to choose which bootblock runs on boot. Each bootblock then
continues booting from a matching CBFS region, so switching the Top Swap state
switches the active slot.

The Intel Top Swap feature allows the PCH to take two physically topmost chunks
of the BIOS flash chip, and decide in which order to map them - effectively
allowing to swap the two chunks, and deciding which of them lands at the reset
vector.

For background on the hardware mechanism, consult the Intel documentation
for your PCH and platform and search for sections named similarly to "Top
Swap", "Top Swap Block Size (TSBS)", or "Boot Block Update Scheme". The exact
document title and section naming varies between generations. The Alder Lake
implementation for example has been developed basing mainly on ADL-P EDS Vol
1&2, sections:

* `3.2.1 Boot Block Update Scheme`,
* `9.70 PCH Descriptor Record 69 (Flash Descriptor Records)`,
* `31.3.2 Backed Up Control (BUC)`,
* `2.1.17 BIOS Control (ESPI_BC)`

## Flash layout and CBFS regions

The implementation assumes four FMAP regions that are CBFS formatted and that
use the following names.

The bootblocks live in `BOOTBLOCK` and `TOPSWAP`. Each of these is a relatively
small CBFS region that contains a bootblock image and any additional files that
must reside next to it for early boot on a given platform. These regions must
exist in the board's `.fmd` file and must be sized with headroom for growth
- keeping in mind the FIT table and Boot Guard ACM's must reside in the same
region as the bootblock.

The main firmware CBFS regions are `COREBOOT` and `COREBOOT_TS`. `COREBOOT` is
typically the base slot and is often placed under write protection, along with
`BOOTBLOCK`. `COREBOOT_TS` is the alternate slot and is typically left writable
so it can be replaced by an update mechanism, along with `TOPSWAP`.

The `BOOTBLOCK` and `TOPSWAP` regions are expected to be placed in the
flash area covered by the Top Swap configuration. The sizes of the regions
must match and be equal to the corresponding field in the IFD. coreboot
can adjust the The Top Swap size (also called Top Swap Block Size in
Intel's documentation) field in the descriptor during the build. This
is controlled by `CONFIG_INTEL_IFD_SET_TOP_SWAP_BOOTBLOCK_SIZE` and uses
`CONFIG_INTEL_TOP_SWAP_BOOTBLOCK_SIZE` as the requested size.

## Kconfig options

The option `CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK` creates two copies of the
bootblock.

The option `CONFIG_INTEL_TOP_SWAP_SEPARATE_REGIONS` changes where the bootblocks
are stored. When it is disabled, both are stored in the primary CBFS region as
on existing platforms. When it is enabled, they are placed in the `BOOTBLOCK`
`TOPSWAP` FMAP regions. It also places copies of the following stages and
required files from the `COREBOOT` region in the `COREBOOT_TS` region.

If CBFS verification is enabled, the image build checks verification status for
all main CBFS regions that are part of the redundancy configuration, not only
for `COREBOOT`.

The option `CONFIG_INTEL_TOP_SWAP_OPTION_CONTROL` enables runtime control of the
Top Swap control bit based on a CMOS option. In tree, this option is constrained
to specific Intel SoCs. For a board to use it, the SoC bootblock must apply the
setting early enough for writes to the Top Swap control bit to take effect.

To reduce per board configuration burden, `CONFIG_TOP_SWAP_REDUNDANCY` exists
as a convenience option that selects the relevant pieces for Top Swap based
redundancy. Even with this convenience option enabled, a board still must
provide an appropriate `.fmd` layout and a `cmos.layout` entry as described
below.

Boards that use Top Swap based redundancy must enable CMOS option support
and ensure the option backend can read options at boot time. The symbol
`MAINBOARD_NEEDS_CMOS_OPTIONS` exists to indicate this dependency. Please
note that not every SoC supports this functionality.

## CMOS option and when it is applied

Runtime Top Swap selection is controlled by a CMOS option named
`attempt_slot_b`. The name is defined as `TOP_SWAP_ENABLE_CMOS_OPTION` in
`src/soc/intel/common/block/include/intelblocks/rtc.h`. A board enabling
`CONFIG_INTEL_TOP_SWAP_OPTION_CONTROL` must provide a CMOS entry with this exact
name in its `cmos.layout` file.

The function `sync_rtc_buc_top_swap()` in `src/soc/intel/common/block/rtc/rtc.c`
implements the synchronization logic. It reads the CMOS option via
`get_uint_option()`, reads the current Top Swap state from the RTC BUC register,
compares the two, and if they differ it programs the new state and resets the
platform. The reset is intentional and ensures the new Top Swap state is in
effect from the beginning of the subsequent boot.

The SoC bootblock must call `sync_rtc_buc_top_swap()` early in
`bootblock_soc_init()` when `CONFIG_INTEL_TOP_SWAP_OPTION_CONTROL` is enabled.
This must happen before the platform locks down the relevant interfaces such
that Top Swap state changes no longer take effect.

Because the CMOS option is applied in bootblock, a mismatch between the requested
slot and the current Top Swap state causes a very early reset. The next boot
then starts directly from the selected slot.

## CBFS region selection

The choice of the main CBFS region is made in `cbfs_get_boot_device()` in
`src/lib/cbfs.c`. Instead of always locating `COREBOOT`, the code calls
`cbfs_fmap_region_hint()` to obtain the FMAP region name and then locates that
region.

The default `cbfs_fmap_region_hint()` implementation is a weak symbol that
returns `COREBOOT`, so platforms that do not override it retain the existing
behavior.

Intel Top Swap platforms provide a strong definition of
`cbfs_fmap_region_hint()` in `src/soc/intel/common/block/rtc/rtc.c`. When
`CONFIG_INTEL_TOP_SWAP_OPTION_CONTROL` is enabled and the Top Swap control
bit is set, it returns `COREBOOT_TS`. Otherwise it returns `COREBOOT`.
`cbfs_get_boot_device()` logs the selected region and stops with an error if the
region cannot be found in FMAP.

This is deliberately based on the actual hardware Top Swap state rather than on
reading CMOS again. At the point where `cbfs_fmap_region_hint()` is used, the
option table is not reliably available, because the `cmos_layout.bin` backing
the option table is itself stored in CBFS.

For the override to apply in a given stage, the RTC block code must be linked
into that stage. The common RTC block is built into multiple stages and is
also built into postcar to ensure the hint function is available when CBFS is
accessed there.

## Boot flow summary

A typical update and rollback sequence is as follows.

The platform starts in slot A with Top Swap disabled. The hardware boots from
the `BOOTBLOCK` and coreboot continues from the `COREBOOT` CBFS region.

An update writes into the `TOPSWAP` and `COREBOOT_TS` regions, then sets the
`attempt_slot_b` CMOS option.

On the next boot, `sync_rtc_buc_top_swap()` observes that the CMOS request
differs from the current hardware Top Swap state, programs the new Top Swap
state, and resets the platform.

After the reset, the hardware starts from the bootblock corresponding to
the new Top Swap state. When coreboot later initializes its boot device,
`cbfs_fmap_region_hint()` sees the Top Swap state and selects `COREBOOT_TS`, so
the remainder of the boot uses the updated slot.

Clearing CMOS or resetting `attempt_slot_b` triggers the same sequence in the
opposite direction, returning the platform to `COREBOOT` without requiring
external flashing.

## Troubleshooting notes

If the platform does not switch slots as expected, confirm that the
`attempt_slot_b` entry exists in `cmos.layout` and that the option table backend
is in use so `get_uint_option()` can read it in bootblock.

If the platform resets but still boots from `COREBOOT`, confirm that the RTC
block implementation providing `cbfs_fmap_region_hint()` is linked into the
stage that performs CBFS initialization on your platform.

If the build fails to boot after enabling separate regions, confirm that the
`.fmd` file defines `BOOTBLOCK`, `TOPSWAP`, `COREBOOT`, and `COREBOOT_TS` as
CBFS regions and that the platform Top Swap configuration matches the reserved
bootblock and Top Swap region placement and sizing.

Serial logs should show the CMOS request and the RTC BUC control bit values
during bootblock, followed by a log line indicating which CBFS region is being
used.
