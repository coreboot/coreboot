/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Advanced Micro Devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/northbridge.h>
#include <soc/pci_devs.h>

uint32_t nb_ioapic_read(unsigned int index)
{
	pci_write_config32(SOC_GNB_DEV, NB_IOAPIC_INDEX, index);
	return pci_read_config32(SOC_GNB_DEV, NB_IOAPIC_DATA);
}

void nb_ioapic_write(unsigned int index, uint32_t value)
{
	pci_write_config32(SOC_GNB_DEV, NB_IOAPIC_INDEX, index);
	pci_write_config32(SOC_GNB_DEV, NB_IOAPIC_DATA, value);
}

void *get_ap_entry_ptr(void)
{
	return (void *)nb_ioapic_read(AP_SCRATCH_REG);
}

void set_ap_entry_ptr(void *entry)
{
	nb_ioapic_write(AP_SCRATCH_REG, (uintptr_t)entry);
}
