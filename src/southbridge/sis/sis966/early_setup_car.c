/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

void sis966_early_pcie_setup(unsigned busnx, unsigned devnx, unsigned anactrl_io_base, unsigned pci_e_x)
{
	uint32_t tgio_ctrl;
	uint32_t pll_ctrl;
	uint32_t dword;
	int i;
	pci_devfn_t dev;
	dev = PCI_DEV(busnx, devnx+1, 1);
	dword = pci_read_config32(dev, 0xe4);
	dword |= 0x3f0; // disable it at first
	pci_write_config32(dev, 0xe4, dword);

	for (i=0; i<3; i++) {
		tgio_ctrl = inl(anactrl_io_base + 0xcc);
		tgio_ctrl &= ~(3<<9);
		tgio_ctrl |= (i<<9);
		outl(tgio_ctrl, anactrl_io_base + 0xcc);
		pll_ctrl = inl(anactrl_io_base + 0x30);
		pll_ctrl |= (1<<31);
		outl(pll_ctrl, anactrl_io_base + 0x30);
		do {
			pll_ctrl = inl(anactrl_io_base + 0x30);
		} while (!(pll_ctrl & 1));
	}
	tgio_ctrl = inl(anactrl_io_base + 0xcc);
	tgio_ctrl &= ~((7<<4)|(1<<8));
	tgio_ctrl |= (pci_e_x<<4)|(1<<8);
	outl(tgio_ctrl, anactrl_io_base + 0xcc);

//	wait 100us
	udelay(100);

	dword = pci_read_config32(dev, 0xe4);
	dword &= ~(0x3f0); // enable
	pci_write_config32(dev, 0xe4, dword);

//	need to wait 100ms
	mdelay(100);
}
