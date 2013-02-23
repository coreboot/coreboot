#include <stdio.h>
#include <stdlib.h>
#include "pirq_routing.h"

static char *preamble[] = {

	"/*\n",
	" * This file is part of the coreboot project.\n",
	" *\n",
	" * Copyright (C) 200x TODO <TODO@TODO>\n",
	" *\n",
	" * This program is free software; you can redistribute it and/or modify\n",
	" * it under the terms of the GNU General Public License as published by\n",
	" * the Free Software Foundation; either version 2 of the License, or\n",
	" * (at your option) any later version.\n",
	" *\n",
	" * This program is distributed in the hope that it will be useful,\n",
	" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n",
	" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n",
	" * GNU General Public License for more details.\n",
	" *\n",
	" * You should have received a copy of the GNU General Public License\n",
	" * along with this program; if not, write to the Free Software\n",
	" * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA\n",
	" */\n\n",
	"#ifdef GETPIR			/* TODO: Drop this when copying to coreboot. */\n",
	"#include \"pirq_routing.h\"	/* TODO: Drop this when copying to coreboot. */\n",
	"#else				/* TODO: Drop this when copying to coreboot. */\n"
	"#include <arch/pirq_routing.h>\n",
	"#endif				/* TODO: Drop this when copying to coreboot. */\n\n"
	"const struct irq_routing_table intel_irq_routing_table = {\n",
	"\tPIRQ_SIGNATURE,		/* u32 signature */\n",
	"\tPIRQ_VERSION,		/* u16 version */\n",
	0
};

void code_gen(char *filename, struct irq_routing_table *rt)
{
	char **code = preamble;
	struct irq_info *se_arr = (struct irq_info *) ((char *) rt + 32);
	int i, ts = (rt->size - 32) / 16;
	FILE *fpir;

	if ((fpir = fopen(filename, "w")) == NULL) {
		printf("Failed creating file!\n");
		exit(2);
	}

	while (*code)
		fprintf(fpir, "%s", *code++);

	fprintf(fpir, "\t32 + 16 * %d,		/* Max. number of devices on the bus */\n",
		ts);
	fprintf(fpir, "\t0x%02x,			/* Interrupt router bus */\n",
		rt->rtr_bus);
	fprintf(fpir, "\t(0x%02x << 3) | 0x%01x,	/* Interrupt router dev */\n",
		rt->rtr_devfn >> 3, rt->rtr_devfn & 7);
	fprintf(fpir, "\t%#x,			/* IRQs devoted exclusively to PCI usage */\n",
		rt->exclusive_irqs);
	fprintf(fpir, "\t%#x,			/* Vendor */\n", rt->rtr_vendor);
	fprintf(fpir, "\t%#x,			/* Device */\n", rt->rtr_device);
	fprintf(fpir, "\t%#x,			/* Miniport */\n",
		rt->miniport_data);
	fprintf(fpir, "\t{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */\n");
	fprintf(fpir, "\t%#x,			/* Checksum (has to be set to some value that\n				 * would give 0 after the sum of all bytes\n				 * for this structure (including checksum).\n                                 */\n",
		rt->checksum);
	fprintf(fpir, "\t{\n");
	fprintf(fpir, "\t\t/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */\n");
	for (i = 0; i < ts; i++) {
		fprintf(fpir, "\t\t{0x%02x, (0x%02x << 3) | 0x%01x, {{0x%02x, 0x%04x}, {0x%02x, 0x%04x}, {0x%02x, 0x%04x}, {0x%02x, 0x%04x}}, 0x%x, 0x%x},\n",
			(se_arr+i)->bus, (se_arr+i)->devfn >> 3,
			(se_arr+i)->devfn & 7, (se_arr+i)->irq[0].link,
			(se_arr+i)->irq[0].bitmap, (se_arr+i)->irq[1].link,
			(se_arr+i)->irq[1].bitmap, (se_arr+i)->irq[2].link,
			(se_arr+i)->irq[2].bitmap, (se_arr+i)->irq[3].link,
			(se_arr+i)->irq[3].bitmap, (se_arr+i)->slot,
			(se_arr+i)->rfu);
	}
	fprintf(fpir, "\t}\n");
	fprintf(fpir, "};\n");

	fprintf(fpir, "\nunsigned long write_pirq_routing_table(unsigned long addr)\n");
	fprintf(fpir, "{\n");
	fprintf(fpir, "\treturn copy_pirq_routing_table(addr);\n");
	fprintf(fpir, "}\n");

	fclose(fpir);
}
