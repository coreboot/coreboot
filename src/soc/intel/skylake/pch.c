/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>

u8 pch_revision(void)
{
	return pci_read_config8(PCH_DEV_LPC, PCI_REVISION_ID);
}

u16 pch_type(void)
{
	return pci_read_config16(PCH_DEV_LPC, PCI_DEVICE_ID);
}
