/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/pci_devs.h>
#include <device/pci_ops.h>
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
	if (instance_id == BROADCAST_FABRIC_ID)
		/* No bit masking required. Macros will apply mask to values. */
		return pci_read_config32(_SOC_DEV(DF_DEV, function), reg);

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(function, reg, instance_id);
	return pci_read_config32(SOC_DF_F4_DEV, DF_FICAD_LO);
}
