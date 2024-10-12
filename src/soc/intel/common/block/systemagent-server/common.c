/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <intelblocks/systemagent_server.h>

static uint32_t pci_moving_config32(pci_devfn_t dev, unsigned int reg)
{
	uint32_t value, ones, zeroes;

	value = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0xffffffff);
	ones = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, 0x00000000);
	zeroes = pci_read_config32(dev, reg);

	pci_write_config32(dev, reg, value);

	return ones ^ zeroes;
}

uint64_t sa_server_read_map_entry(pci_devfn_t dev,
				  const struct sa_server_mem_map_descriptor *entry)
{
	uint64_t value = 0;
	uint32_t alignment = entry->alignment;

	if (entry->is_64_bit) {
		value = pci_s_read_config32(dev, entry->reg_offset + 4);
		value <<= 32;
	}

	value |= pci_s_read_config32(dev, entry->reg_offset);

	/**
	 * Probe the alignment if not provided, promising the lower bits are read only, otherwise
	 * SoC should give a initial value.
	 */
	if (!alignment)
		alignment = ~pci_moving_config32(dev, entry->reg_offset) + 1;

	/**
	 * If register stores the limit address, where lower bits are read as 0s but treated as 1s,
	 * restore the lower bits and align it up.
	 */
	if (entry->is_limit)
		value = ALIGN_UP(value + alignment - 1, alignment);
	else
		value = ALIGN_DOWN(value, alignment);

	return value;
}
