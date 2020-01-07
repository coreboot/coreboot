/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <device/smbus_host.h>
#include <intelblocks/smbus.h>
#include <reg_script.h>
#include <soc/pci_devs.h>
#include "smbuslib.h"

static const struct reg_script smbus_init_script[] = {
	/* Set SMBus I/O base address */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_4, SMBUS_IO_BASE),
	/* Set SMBus enable */
	REG_PCI_WRITE8(HOSTC, HST_EN),
	/* Enable I/O access */
	REG_PCI_WRITE16(PCI_COMMAND, PCI_COMMAND_IO),
	/* Disable interrupts */
	REG_IO_WRITE8(SMBUS_IO_BASE + SMBHSTCTL, 0),
	/* Clear errors */
	REG_IO_WRITE8(SMBUS_IO_BASE + SMBHSTSTAT, 0xff),
	/* Indicate the end of this array by REG_SCRIPT_END */
	REG_SCRIPT_END,
};

void smbus_common_init(void)
{
	reg_script_run_on_dev(PCH_DEV_SMBUS, smbus_init_script);
}

uintptr_t smbus_base(void)
{
	return SMBUS_IO_BASE;
}
