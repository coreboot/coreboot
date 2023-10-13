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
	const union df_mmio_addr_ext ext_reg = {
		.raw = data_fabric_broadcast_read32(DF_MMIO_ADDR_EXT(reg))
	};
	/* The raw register values in the base and limit registers are bits 47..16  of the
	   actual address. The MMIO address extension register contains the extended MMIO base
	   and limit bits starting with bit 48 of the actual address. */
	*mmio_base = (resource_t)ext_reg.base_ext << DF_MMIO_EXT_ADDR_SHIFT |
			(resource_t)base_reg << DF_MMIO_SHIFT;
	*mmio_limit = ((resource_t)ext_reg.limit_ext << DF_MMIO_EXT_ADDR_SHIFT |
			(((resource_t)limit_reg + 1) << DF_MMIO_SHIFT)) - 1;
}
