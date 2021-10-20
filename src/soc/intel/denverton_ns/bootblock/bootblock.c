/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <bootblock_common.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <FsptUpd.h>
#include <intelblocks/fast_spi.h>
#include <soc/bootblock.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <spi-generic.h>
#include <stdint.h>
#include <console/console.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
			.Signature = 0x545F445055564E44ULL,
			.Revision = 1,
			.Reserved = {0},
	},
	.FsptCoreUpd = {
			/*
			 * It is a requirement for firmware to have Firmware Interface Table
			 * (FIT), which contains pointers to each microcode update.
			 * The microcode update is loaded for all logical processors before
			 * cpu reset vector.
			 *
			 * All SoC since Gen-4 has above mechanism in place to load microcode
			 * even before hitting CPU reset vector. Hence skipping FSP-T loading
			 * microcode after CPU reset by passing '0' value to
			 * FSPT_UPD.MicrocodeRegionBase and FSPT_UPD.MicrocodeRegionLength.
			 */
			.MicrocodeRegionBase = 0,
			.MicrocodeRegionLength = 0,
			.CodeRegionBase =
				(UINT32)(0x100000000ULL - CONFIG_ROM_SIZE),
			.CodeRegionLength = (UINT32)CONFIG_ROM_SIZE,
			.Reserved1 = {0},
	},
	.FsptConfig = {
			.PcdFsptPort80RouteDisable = 0,
			.ReservedTempRamInitUpd = {0},
	},
	.UnusedUpdSpace0 = {0},
	.UpdTerminator = 0x55AA,
};

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
};

static void sanity_check_pci_mmconf(void)
{
	u32 pciexbar, base = 0, length = 0;

	pciexbar = pci_io_read_config32(PCH_SA_DEV, PCIEXBAR);
	assert(pciexbar & (1 << 0));

	switch (pciexbar & MASK_PCIEXBAR_LENGTH) {
	case MASK_PCIEXBAR_LENGTH_256M:
		base = pciexbar & MASK_PCIEXBAR_256M;
		length = 256;
		break;
	case MASK_PCIEXBAR_LENGTH_128M:
		base = pciexbar & MASK_PCIEXBAR_128M;
		length = 128;
		break;
	case MASK_PCIEXBAR_LENGTH_64M:
		base = pciexbar & MASK_PCIEXBAR_64M;
		length = 64;
		break;
	}

	assert(base == CONFIG_ECAM_MMCONF_BASE_ADDRESS);
	assert(length == CONFIG_ECAM_MMCONF_BUS_NUMBER);
}

void bootblock_soc_early_init(void)
{

#if (CONFIG(CONSOLE_SERIAL))
	early_uart_init();
#endif
};

void bootblock_soc_init(void)
{
	sanity_check_pci_mmconf();

	if (CONFIG(BOOTBLOCK_CONSOLE))
		printk(BIOS_DEBUG, "FSP TempRamInit successful...\n");
};
