/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <device/device.h>
#include <types.h>

/* Read the registers and return normalized values */
void data_fabric_get_mmio_base_size(unsigned int reg, resource_t *mmio_base,
				    resource_t *mmio_limit)
{
	const uint32_t base_reg = data_fabric_broadcast_read32(DF_MMIO_BASE(reg));
	const uint32_t limit_reg = data_fabric_broadcast_read32(DF_MMIO_LIMIT(reg));
	/* The raw register values are bits 47..16  of the actual address */
	*mmio_base = (resource_t)base_reg << DF_MMIO_SHIFT;
	*mmio_limit = (((resource_t)limit_reg + 1) << DF_MMIO_SHIFT) - 1;
}
