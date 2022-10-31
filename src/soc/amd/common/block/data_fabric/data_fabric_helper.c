/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/pci_devs.h>
#include <arch/hpet.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <types.h>
#include "data_fabric_def.h"

static void data_fabric_set_indirect_address(uint8_t func, uint16_t reg, uint8_t instance_id)
{
	uint32_t fabric_indirect_access_reg = DF_IND_CFG_INST_ACC_EN;
	/* Register offset field [10:2] in this register corresponds to [10:2] of the
	   requested offset. */
	fabric_indirect_access_reg |= reg & DF_IND_CFG_ACC_REG_MASK;
	fabric_indirect_access_reg |=
		(func << DF_IND_CFG_ACC_FUN_SHIFT) & DF_IND_CFG_ACC_FUN_MASK;
	fabric_indirect_access_reg |= instance_id << DF_IND_CFG_INST_ID_SHIFT;
	pci_write_config32(SOC_DF_F4_DEV, DF_FICAA_BIOS, fabric_indirect_access_reg);
}

uint32_t data_fabric_read32(uint8_t function, uint16_t reg, uint8_t instance_id)
{
	/* Broadcast reads might return unexpected results when a register has different
	   contents in the different instances. */
	if (instance_id == BROADCAST_FABRIC_ID)
		return data_fabric_broadcast_read32(function, reg);

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(function, reg, instance_id);
	return pci_read_config32(SOC_DF_F4_DEV, DF_FICAD_LO);
}

void data_fabric_write32(uint8_t function, uint16_t reg, uint8_t instance_id, uint32_t data)
{
	if (instance_id == BROADCAST_FABRIC_ID) {
		data_fabric_broadcast_write32(function, reg, data);
		return;
	}

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(function, reg, instance_id);
	pci_write_config32(SOC_DF_F4_DEV, DF_FICAD_LO, data);
}

void data_fabric_print_mmio_conf(void)
{
	printk(BIOS_SPEW,
		"=== Data Fabric MMIO configuration registers ===\n"
		"Addresses are shifted to the right by 16 bits.\n"
		"idx  control     base    limit\n");
	for (unsigned int i = 0; i < NUM_NB_MMIO_REGS; i++) {
		printk(BIOS_SPEW, " %2u %8x %8x %8x\n",
			i,
			data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(i)),
			data_fabric_broadcast_read32(0, NB_MMIO_BASE(i)),
			data_fabric_broadcast_read32(0, NB_MMIO_LIMIT(i)));
	}
}

void data_fabric_disable_mmio_reg(unsigned int reg)
{
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg),
		IOMS0_FABRIC_ID << DF_MMIO_DST_FABRIC_ID_SHIFT);
	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), 0);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), 0);
}

static bool is_mmio_reg_disabled(unsigned int reg)
{
	uint32_t val = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(reg));
	return !(val & (DF_MMIO_WE | DF_MMIO_RE));
}

int data_fabric_find_unused_mmio_reg(void)
{
	for (unsigned int i = 0; i < NUM_NB_MMIO_REGS; i++) {
		if (is_mmio_reg_disabled(i))
			return i;
	}
	return -1;
}

void data_fabric_set_mmio_np(void)
{
	/*
	 * Mark region from HPET-LAPIC or 0xfed00000-0xfee00000-1 as NP.
	 *
	 * AGESA has already programmed the NB MMIO routing, however nothing
	 * is yet marked as non-posted.
	 *
	 * If there exists an overlapping routing base/limit pair, trim its
	 * base or limit to avoid the new NP region.  If any pair exists
	 * completely within HPET-LAPIC range, remove it.  If any pair surrounds
	 * HPET-LAPIC, it must be split into two regions.
	 *
	 * TODO(b/156296146): Remove the settings from AGESA and allow coreboot
	 * to own everything.  If not practical, consider erasing all settings
	 * and have coreboot reprogram them.  At that time, make the source
	 * below more flexible.
	 *   * Note that the code relies on the granularity of the HPET and
	 *     LAPIC addresses being sufficiently large that the shifted limits
	 *     +/-1 are always equivalent to the non-shifted values +/-1.
	 */

	unsigned int i;
	int reg;
	uint32_t base, limit, ctrl;
	const uint32_t np_bot = HPET_BASE_ADDRESS >> D18F0_MMIO_SHIFT;
	const uint32_t np_top = (LAPIC_DEFAULT_BASE - 1) >> D18F0_MMIO_SHIFT;

	data_fabric_print_mmio_conf();

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		/* Adjust all registers that overlap */
		ctrl = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(i));
		if (!(ctrl & (DF_MMIO_WE | DF_MMIO_RE)))
			continue; /* not enabled */

		base = data_fabric_broadcast_read32(0, NB_MMIO_BASE(i));
		limit = data_fabric_broadcast_read32(0, NB_MMIO_LIMIT(i));

		if (base > np_top || limit < np_bot)
			continue; /* no overlap at all */

		if (base >= np_bot && limit <= np_top) {
			data_fabric_disable_mmio_reg(i); /* 100% within, so remove */
			continue;
		}

		if (base < np_bot && limit > np_top) {
			/* Split the configured region */
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(i), np_bot - 1);
			reg = data_fabric_find_unused_mmio_reg();
			if (reg < 0) {
				/* Although a pair could be freed later, this condition is
				 * very unusual and deserves analysis.  Flag an error and
				 * leave the topmost part unconfigured. */
				printk(BIOS_ERR, "Not enough NB MMIO routing registers\n");
				continue;
			}
			data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), np_top + 1);
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), limit);
			data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg), ctrl);
			continue;
		}

		/* If still here, adjust only the base or limit */
		if (base <= np_bot)
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(i), np_bot - 1);
		else
			data_fabric_broadcast_write32(0, NB_MMIO_BASE(i), np_top + 1);
	}

	reg = data_fabric_find_unused_mmio_reg();
	if (reg < 0) {
		printk(BIOS_ERR, "cannot configure region as NP\n");
		return;
	}

	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), np_bot);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), np_top);
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg),
			   (IOMS0_FABRIC_ID << DF_MMIO_DST_FABRIC_ID_SHIFT) | DF_MMIO_NP
				   | DF_MMIO_WE | DF_MMIO_RE);

	data_fabric_print_mmio_conf();
}
