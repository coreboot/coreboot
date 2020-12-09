/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <cf9_reset.h>
#include <reset.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/reset.h>

void set_warm_reset_flag(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	htic |= HTIC_COLD_RST_DET;
	pci_write_config32(SOC_HT_DEV, HT_INIT_CONTROL, htic);
}

int is_warm_reset(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	return !!(htic & HTIC_COLD_RST_DET);
}

/* Clear bits 5, 9 & 10, used to signal the reset type */
static void clear_bios_reset(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_DETECT;
	pci_write_config32(SOC_HT_DEV, HT_INIT_CONTROL, htic);
}

void do_cold_reset(void)
{
	clear_bios_reset();

	/* De-assert and then assert all PwrGood signals on CF9 reset. */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) |
		TOGGLE_ALL_PWR_GOOD);
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_warm_reset(void)
{
	set_warm_reset_flag();
	clear_bios_reset();

	/* Assert reset signals only. */
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_board_reset(void)
{
	/* TODO: Would a warm_reset() suffice? */
	do_cold_reset();
}
