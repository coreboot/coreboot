/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/gpmr.h>
#include <intelblocks/ioc.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <types.h>

/* GPMR Register read given offset */
uint32_t gpmr_read32(uint16_t offset)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_IOC))
		return ioc_reg_read32(offset);
	else
		return pcr_read32(PID_DMI, offset);
}

/* GPMR Register write given offset and val */
void gpmr_write32(uint16_t offset, uint32_t val)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_IOC))
		return ioc_reg_write32(offset, val);
	else
		return pcr_write32(PID_DMI, offset, val);
}

void gpmr_or32(uint16_t offset, uint32_t ordata)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_IOC))
		return ioc_reg_or32(offset, ordata);
	else
		return pcr_or32(PID_DMI, offset, ordata);
}

/* Check for available free gpmr */
static int get_available_gpmr(void)
{
	int i;
	uint32_t val;

	for (i = 0; i < MAX_GPMR_REGS; i++) {
		val = gpmr_read32(GPMR_DID_OFFSET(i));
		if (!(val & GPMR_EN))
			return i;
	}
	printk(BIOS_ERR, "%s: No available free gpmr found\n", __func__);
	return CB_ERR;
}

/* Configure GPMR for the given base and size of extended BIOS Region */
enum cb_err enable_gpmr(uint32_t base, uint32_t size, uint32_t dest_id)
{
	int gpmr_num;
	uint32_t limit;

	if (base & ~(GPMR_BASE_MASK << GPMR_BASE_SHIFT)) {
		printk(BIOS_ERR, "base is not 64-KiB aligned!\n");
		return CB_ERR;
	}

	limit = base + (size - 1);

	if (limit < base) {
		printk(BIOS_ERR, "Invalid limit: limit cannot be less than base!\n");
		return CB_ERR;
	}

	if ((limit & ~GPMR_LIMIT_MASK) != 0xffff) {
		printk(BIOS_ERR, "limit does not end on a 64-KiB boundary!\n");
		return CB_ERR;
	}

	/* Get available free GPMR */
	gpmr_num = get_available_gpmr();
	if (gpmr_num == CB_ERR)
		return CB_ERR;

	/* Program Range for the given decode window */
	gpmr_write32(GPMR_OFFSET(gpmr_num), (limit & GPMR_LIMIT_MASK) |
		((base >> GPMR_BASE_SHIFT) & GPMR_BASE_MASK));

	/* Program source decode enable bit and the Destination ID */
	gpmr_write32(GPMR_DID_OFFSET(gpmr_num), dest_id | GPMR_EN);

	return CB_SUCCESS;
}
