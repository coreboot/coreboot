/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <stdint.h>

static void fill_x_pm_io(acpi_addr_t *x_pm_blk, int access_size, uint16_t ioport, uint8_t len)
{
	x_pm_blk->space_id = ACPI_ADDRESS_SPACE_IO;
	x_pm_blk->bit_width = len * 8;
	x_pm_blk->bit_offset = 0;
	x_pm_blk->access_size = access_size;
	x_pm_blk->addrl = ioport;
	x_pm_blk->addrh = 0x0;
}

static void do_sanity_error(const char *func, const int line)
{
	printk(BIOS_ERR, "ACPI: FADT error detected, %s line %d.\n", func, line);
}

#define fadt_sanity(x)	do { if (!(x)) do_sanity_error(__func__, __LINE__); } while (0)

void fill_fadt_extended_pm_io(acpi_fadt_t *fadt)
{
	fadt_sanity(fadt->pm1a_evt_blk && (fadt->pm1_evt_len >= 4));

	/* Upper word is reserved and Linux complains about 32 bit. */
	fadt_sanity(fadt->pm1a_cnt_blk && (fadt->pm1_cnt_len == 2));

	fill_x_pm_io(&fadt->x_pm1a_evt_blk, ACPI_ACCESS_SIZE_WORD_ACCESS,
		     fadt->pm1a_evt_blk, fadt->pm1_evt_len);

	fill_x_pm_io(&fadt->x_pm1a_cnt_blk, ACPI_ACCESS_SIZE_WORD_ACCESS,
		     fadt->pm1a_cnt_blk, fadt->pm1_cnt_len);

	if (fadt->pm1b_evt_blk)
		fill_x_pm_io(&fadt->x_pm1b_evt_blk, ACPI_ACCESS_SIZE_WORD_ACCESS,
			     fadt->pm1b_evt_blk, fadt->pm1_evt_len);

	if (fadt->pm1b_cnt_blk)
		fill_x_pm_io(&fadt->x_pm1b_cnt_blk, ACPI_ACCESS_SIZE_WORD_ACCESS,
			     fadt->pm1b_cnt_blk, fadt->pm1_cnt_len);

	if (fadt->pm_tmr_blk) {
		fadt_sanity(fadt->pm_tmr_len == 4);
		fill_x_pm_io(&fadt->x_pm_tmr_blk, ACPI_ACCESS_SIZE_DWORD_ACCESS,
			     fadt->pm_tmr_blk, fadt->pm_tmr_len);
	}

	if (fadt->pm2_cnt_blk)
		fill_x_pm_io(&fadt->x_pm2_cnt_blk, ACPI_ACCESS_SIZE_BYTE_ACCESS,
			     fadt->pm2_cnt_blk, fadt->pm2_cnt_len);

	/*
	 * Windows 10 requires x_gpe0_blk to be set starting with FADT revision 5.
	 * The bit_width field intentionally overflows here.
	 * The OSPM can instead use the values in `fadt->gpe0_blk{,_len}`, which
	 * seems to work fine on Linux 5.0 and Windows 10.
	 *
	 * FIXME: GPE1_BASE is not initialised.
	 *
	 */

	if (fadt->gpe0_blk)
		fill_x_pm_io(&fadt->x_gpe0_blk, ACPI_ACCESS_SIZE_BYTE_ACCESS,
			     fadt->gpe0_blk, fadt->gpe0_blk_len);

	if (fadt->gpe1_blk)
		fill_x_pm_io(&fadt->x_gpe1_blk, ACPI_ACCESS_SIZE_BYTE_ACCESS,
			     fadt->gpe1_blk, fadt->gpe1_blk_len);
}
