/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 * Copyright (C) 2019 3mdeb
 * Copyright (C) 2019 Eltan B.V.
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

#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <device/pci_def.h>
#include <device/pci_type.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <soc/smbus.h>

int smbus_i2c_block_write(u8 addr, u8 bytes, u8 *buf)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev = PCI_DEV(0, SMBUS_DEV, SMBUS_FUNC);
#else
	struct device *dev = pcidev_on_root(SMBUS_DEV, SMBUS_FUNC);
#endif
	u32 smbase;
	u32 smb_ctrl_reg;
	int status;

	/* SMBus I/O BAR */
	smbase = pci_read_config32(dev, PCI_BASE_ADDRESS_4) & 0xFFFFFFFE;

	/* Enable I2C_EN bit in HOSTC register */
	smb_ctrl_reg = pci_read_config32(dev, HOSTC);
	pci_write_config32(dev, HOSTC, smb_ctrl_reg | HOSTC_I2C_EN);

	status = do_i2c_block_write(smbase, addr, bytes, buf);

	/* Restore I2C_EN bit */
	pci_write_config32(dev, HOSTC, smb_ctrl_reg);

	return status;
}
