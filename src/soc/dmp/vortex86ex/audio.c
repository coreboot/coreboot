/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

#include <device/azalia_device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

/* RDC HD audio controller */
static const struct pci_driver rdc_audio __pci_driver = {
	.ops	= &default_azalia_audio_ops,
	.vendor	= PCI_VENDOR_ID_RDC,
	.device	= 0x3010,
};
