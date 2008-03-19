/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * Seems the link and width of HT link needs to be setup too, you need to
 * generate PCI reset or LDTSTOP to apply.
 */

#include <stdlib.h>

/* AMD K8 LDT0, LDT1, LDT2 Link Control Registers */
static ldtreg[3] = {0x86, 0xa6, 0xc6};

/* This functions sets KT890 link frequency and width to same values as
 * it has been setup on K8 side, by AMD NB init.
 */ 

u8 k8t890_early_setup_ht(void)
{
	u8 awidth, afreq, cldtfreq; 
	u8 cldtwidth_in, cldtwidth_out, vldtwidth_in, vldtwidth_out, ldtnr, width;
	u16 vldtcaps;

	/* check if connected non coherent, initcomplete (find the SB on K8 side) */
	if (0x7 == pci_read_config8(PCI_DEV(0, 0x18, 0), 0x98)) {
		ldtnr = 0;
	} else if (0x7 == pci_read_config8(PCI_DEV(0, 0x18, 0), 0xb8)) {
		ldtnr = 1;
	} else if (0x7 == pci_read_config8(PCI_DEV(0, 0x18, 0), 0xd8)) {
		ldtnr = 2;
	}

	print_debug("K8T890 found at LDT ");
	print_debug_hex8(ldtnr);

	/* get the maximum widths for both sides */
	cldtwidth_in = pci_read_config8(PCI_DEV(0, 0x18, 0), ldtreg[ldtnr]) & 0x7;
	cldtwidth_out = (pci_read_config8(PCI_DEV(0, 0x18, 0), ldtreg[ldtnr]) >> 4) & 0x7;
	vldtwidth_in = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x66) & 0x7;
	vldtwidth_out = (pci_read_config8(PCI_DEV(0, 0x0, 0), 0x66) >> 4) & 0x7;

	width = MIN(MIN(MIN(cldtwidth_out, cldtwidth_in), vldtwidth_out), vldtwidth_in);
	print_debug(" Agreed on width: ");
	print_debug_hex8(width);

	awidth = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x67);

	/* Update the desired HT LNK to match AMD NB max from VIA NB is 0x1 */
	width = (width == 0x01) ? 0x11 : 0x00;

	pci_write_config8(PCI_DEV(0, 0x0, 0), 0x67, width);

	/* Get programmed HT freq at base 0x89 */
	cldtfreq = pci_read_config8(PCI_DEV(0, 0x18, 0), ldtreg[ldtnr] + 3) & 0xf;
	print_debug(" CPU programmed to HT freq: ");
	print_debug_hex8(cldtfreq);

	print_debug(" VIA HT caps: ");
	vldtcaps = pci_read_config16(PCI_DEV(0, 0, 0), 0x6e);
	print_debug_hex16(vldtcaps);

	if (!(vldtcaps & (1 << cldtfreq ))) {
		die("Chipset does not support desired HT frequency\n");
	}

	afreq = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x6d);
	pci_write_config8(PCI_DEV(0, 0x0, 0), 0x6d, cldtfreq);
	print_debug("\n");

	/* no reset needed */
	if ((width == awidth) && (afreq == cldtfreq)) {
		return 0;
	}

	return 1;
}
