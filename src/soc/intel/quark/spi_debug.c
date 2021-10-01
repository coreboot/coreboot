/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/spi.h>

const char *spi_opcode_name(int opcode)
{
	const char *op_name;

	switch (opcode) {
	default:
		op_name = "Unknown";
		break;
	case 1:
		op_name = "Write Status";
		break;
	case 2:
		op_name = "Page Program";
		break;
	case 3:
		op_name = "Read Data";
		break;
	case 5:
		op_name = "Read Status";
		break;
	case 6:
		op_name = "Write Data Enable";
		break;
	case 0x0b:
		op_name = "Fast Read";
		break;
	case 0x20:
		op_name = "Erase 4 KiB";
		break;
	case 0x50:
		op_name = "Write Status Enable";
		break;
	case 0x9f:
		op_name = "Read ID";
		break;
	case 0xd8:
		op_name = "Erase 64 KiB";
		break;
	}
	return op_name;
}

void spi_display(volatile struct flash_ctrlr *ctrlr)
{
	int index;
	int opcode;
	const char *op_name;
	int prefix;
	int status;
	int type;

	/* Display the prefixes */
	printk(BIOS_DEBUG, "Prefix Table\n");
	for (index = 0; index < 2; index++) {
		prefix = ctrlr->prefix[index];
		op_name = spi_opcode_name(prefix);
		printk(BIOS_DEBUG, "  %d: 0x%02x (%s)\n", index, prefix,
			op_name);
	}

	/* Display the opcodes */
	printk(BIOS_DEBUG, "Opcode Menu\n");
	for (index = 0; index < 8; index++) {
		opcode = ctrlr->opmenu[index];
		type = (ctrlr->type >> (index << 1)) & 3;
		op_name = spi_opcode_name(opcode);
		printk(BIOS_DEBUG, "  %d: 0x%02x (%s), %s%s\n", index, opcode,
			op_name,
			(type & SPITYPE_PREFIX) ? "Write" : "Read",
			(type & SPITYPE_ADDRESS) ? ", w/3 byte address" : "");
	}

	/* Display the BIOS base address */
	printk(BIOS_DEBUG, "0x%08x: BIOS Base Address\n", ctrlr->bbar);

	/* Display the protection ranges */
	printk(BIOS_DEBUG, "BIOS Protected Range Registers\n");
	for (index = 0; index < ARRAY_SIZE(ctrlr->pbr); index++) {
		status = ctrlr->pbr[index];
		printk(BIOS_DEBUG, "  %d: 0x%08x: 0x%08x - 0x%08x %s\n",
			index, status,
			0xff000000 | (0x1000000 - CONFIG_ROM_SIZE)
				| ((status & SPIPBR_PRB) << SPIPBR_PRB_SHIFT),
			0xff800fff | (0x1000000 - CONFIG_ROM_SIZE)
				| (status & SPIPBR_PRL),
			(status & SPIPBR_WPE) ? "Protected" : "Unprotected");
	}

	/* Display locked status */
	status = ctrlr->status;
	printk(BIOS_DEBUG, "0x%04x: SPISTS, Tables %s\n", status,
		(status & SPISTS_CLD) ? "Locked" : "Unlocked");
}
