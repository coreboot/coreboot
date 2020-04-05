/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/pci_def.h>
#include <device/smbus_host.h>
#include <reg_script.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/smbus.h>
#include <soc/romstage.h>

static const struct reg_script smbus_init_script[] = {
	/* Set SMBUS I/O base address */
	REG_PCI_WRITE32(SMB_BASE, SMBUS_BASE_ADDRESS | 1),
	/* Set SMBUS enable */
	REG_PCI_WRITE8(HOSTC, HST_EN),
	/* Enable I/O access */
	REG_PCI_WRITE16(PCI_COMMAND, PCI_COMMAND_IO),
	/* Disable interrupts */
	REG_IO_WRITE8(SMBUS_BASE_ADDRESS + SMBHSTCTL, 0),
	/* Clear errors */
	REG_IO_WRITE8(SMBUS_BASE_ADDRESS + SMBHSTSTAT, 0xff),
	/* Indicate the end of this array by REG_SCRIPT_END */
	REG_SCRIPT_END,
};

uintptr_t smbus_base(void)
{
	return SMBUS_BASE_ADDRESS;
}

int smbus_enable_iobar(uintptr_t base)
{
	reg_script_run_on_dev(PCH_DEV_SMBUS, smbus_init_script);
	return 0;
}
