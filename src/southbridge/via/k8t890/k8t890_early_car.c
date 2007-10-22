/*
 * This file is part of the LinuxBIOS project.
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

u8 k8t890_early_setup_car(u8 width, u8 speed)
{
	u8 awidth, aspeed;

	print_debug("LDT width and speed for K8T890 was");
	awidth = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x67);
	print_debug_hex8(awidth);

	aspeed = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x6d);
	print_debug_hex8(aspeed);

	if ((aspeed == speed) && (((width == 16) && (awidth == 0x11)) ||
				  ((width == 8) && (awidth == 0x00))))
		return 0;

	/* Update the desired HT LNK capabilities in NB too. */
	pci_write_config8(PCI_DEV(0, 0x0, 0), 0x67,
			  (width == 16) ? 0x11 : 0x00);
	pci_write_config8(PCI_DEV(0, 0x0, 0), 0x6d, speed);

	print_debug(" and will after HT reset: ");

	awidth = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x67);
	print_debug_hex8(awidth);

	aspeed = pci_read_config8(PCI_DEV(0, 0x0, 0), 0x6d);
	print_debug_hex8(aspeed);

	print_debug("\n");

	return 1;
}
