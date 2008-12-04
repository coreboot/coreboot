/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH 
 *  written by Stefan Reinauer <stepan@coresystems.de> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include "inteltool.h"

static const io_register_t ich7_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter C2/C3/C4 state, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile/Ultra Mobile)" },
	{ 0x15, 1, "LV3 (Mobile/Ultra Mobile)" },
	{ 0x16, 1, "LV4 (Mobile/Ultra Mobile)" },
#endif
	{ 0x17, 1, "RESERVED" },
	{ 0x18, 4, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 1, "PM2_CNT (Mobile/Ultra Mobile)" },
	{ 0x21, 1, "RESERVED" },
	{ 0x22, 2, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "SS_CNT (Mobile/Ultra Mobile)" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile/Ultra Mobile)" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t ich8_pm_registers[] = {
	{ 0x00, 2, "PM1_STS" },
	{ 0x02, 2, "PM1_EN" },
	{ 0x04, 4, "PM1_CNT" },
	{ 0x08, 4, "PM1_TMR" },
	{ 0x0c, 4, "RESERVED" },
	{ 0x10, 4, "PROC_CNT" },
#if DANGEROUS_REGISTERS
	/* These registers return 0 on read, but reading them may cause
	 * the system to enter Cx states, which might hang the system.
	 */
	{ 0x14, 1, "LV2 (Mobile)" },
	{ 0x15, 1, "LV3 (Mobile)" },
	{ 0x16, 1, "LV4 (Mobile)" },
	{ 0x17, 1, "LV5 (Mobile)" },
	{ 0x18, 1, "LV6 (Mobile)" },
#endif
	{ 0x19, 1, "RESERVED" },
	{ 0x1a, 2, "RESERVED" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 1, "PM2_CNT (Mobile)" },
	{ 0x21, 1, "RESERVED" },
	{ 0x22, 2, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "GPE0_STS" },
	{ 0x2C, 4, "GPE0_EN" },
	{ 0x30, 4, "SMI_EN" },
	{ 0x34, 4, "SMI_STS" },
	{ 0x38, 2, "ALT_GP_SMI_EN" },
	{ 0x3a, 2, "ALT_GP_SMI_STS" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 2, "RESERVED" },
	{ 0x42, 1, "GPE_CNTL" },
	{ 0x43, 1, "RESERVED" },
	{ 0x44, 2, "DEVACT_STS" },
	{ 0x46, 2, "RESERVED" },
	{ 0x48, 4, "RESERVED" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 1, "SS_CNT (Mobile)" },
	{ 0x51, 1, "RESERVED" },
	{ 0x52, 2, "RESERVED" },
	{ 0x54, 4, "C3_RES (Mobile)" },
	{ 0x58, 4, "C5_RES (Mobile)" },
	{ 0x5c, 4, "RESERVED" },
	/* Here start the TCO registers */
	{ 0x60, 2, "TCO_RLD" },
	{ 0x62, 1, "TCO_DAT_IN" },
	{ 0x63, 1, "TCO_DAT_OUT" },
	{ 0x64, 2, "TCO1_STS" },
	{ 0x66, 2, "TCO2_STS" },
	{ 0x68, 2, "TCO1_CNT" },
	{ 0x6a, 2, "TCO2_CNT" },
	{ 0x6c, 2, "TCO_MESSAGE" },
	{ 0x6e, 1, "TCO_WDCNT" },
	{ 0x6f, 1, "RESERVED" },
	{ 0x70, 1, "SW_IRQ_GEN" },
	{ 0x71, 1, "RESERVED" },
	{ 0x72, 2, "TCO_TMR" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

int print_pmbase(struct pci_dev *sb)
{
	int i, size;
	uint16_t pmbase;
	const io_register_t *pm_registers;

	printf("\n============= PMBASE ============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich7_pm_registers;
		size = ARRAY_SIZE(ich7_pm_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH8M:
		pmbase = pci_read_word(sb, 0x40) & 0xfffc;
		pm_registers = ich8_pm_registers;
		size = ARRAY_SIZE(ich8_pm_registers);
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("This southbridge does not have PMBASE.\n");
		return 1;
	default:
		printf("Error: Dumping PMBASE on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("PMBASE = 0x%04x (IO)\n\n", pmbase);

	for (i = 0; i < size; i++) {
		switch (pm_registers[i].size) {
		case 4:
			printf("pmbase+0x%04x: 0x%08x (%s)\n",
				pm_registers[i].addr,
				inl(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		case 2:
			printf("pmbase+0x%04x: 0x%04x     (%s)\n",
				pm_registers[i].addr,
				inw(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		case 1:
			printf("pmbase+0x%04x: 0x%02x       (%s)\n",
				pm_registers[i].addr,
				inb(pmbase+pm_registers[i].addr),
				pm_registers[i].name);
			break;
		}
	}

	return 0;
}

