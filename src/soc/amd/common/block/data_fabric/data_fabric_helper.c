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
	union df_ficaa ficaa = { .cfg_inst_acc_en = 1 };
	/* convert register address to 32-bit register number */
	ficaa.reg_num = reg >> 2;
	ficaa.func_num = func;
	ficaa.inst_id = instance_id;
	pci_write_config32(SOC_DF_F4_DEV, DF_FICAA_BIOS, ficaa.raw);
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
	union df_mmio_control ctrl = { .fabric_id = IOMS0_FABRIC_ID };
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg), ctrl.raw);
	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), 0);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), 0);
}

static bool is_mmio_reg_disabled(unsigned int reg)
{
	union df_mmio_control ctrl;
	ctrl.raw = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(reg));
	return !(ctrl.we || ctrl.re);
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
	uint32_t base, limit;
	union df_mmio_control ctrl;
	const uint32_t np_bot = HPET_BASE_ADDRESS >> D18F0_MMIO_SHIFT;
	const uint32_t np_top = (LAPIC_DEFAULT_BASE - 1) >> D18F0_MMIO_SHIFT;

	data_fabric_print_mmio_conf();

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		/* Adjust all registers that overlap */
		ctrl.raw = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(i));
		if (!(ctrl.we || ctrl.re))
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
			data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg), ctrl.raw);
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

	union df_mmio_control np_ctrl = { .fabric_id = IOMS0_FABRIC_ID,
					  .np = 1, .we = 1, .re = 1 };
	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), np_bot);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), np_top);
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg), np_ctrl.raw);

	data_fabric_print_mmio_conf();
}
