/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

#include <cpu/x86/mtrr.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <AGESA.h>
#include "amdlib.h"

UINT64
MsrRead (
	IN       UINT32 MsrAddress
	);

VOID
MsrWrite (
	IN       UINT32 MsrAddress,
	IN       UINT64 Value
	);


UINT64
MsrRead (
	IN       UINT32 MsrAddress
	)
{
	return __readmsr (MsrAddress);
}

VOID
MsrWrite (
	IN       UINT32 MsrAddress,
	IN       UINT64 Value
	)
{
	__writemsr (MsrAddress, Value);
}

void amd_initcpuio(void)
{
	UINT32			PciData;
	PCI_ADDR		PciAddress;
	AMD_CONFIG_PARAMS	StdHeader;
	UINT32			nodes;
	UINT32			node;
	UINT32			sblink;
	UINT32			i;
	UINT32			TOM;

	/* get the number of coherent nodes in the system */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x60);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	nodes = ((PciData >> 4) & 7) + 1; //NodeCnt[2:0]

	/* Find out the Link ID of Node0 that connects to the
	 * Southbridge (system IO hub). e.g. family10 MCM Processor,
	 * sbLink is Processor0 Link2, internal Node0 Link3
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x64);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	sblink = (PciData >> 8) & 3; //assume ganged

	/* Enable MMIO on AMD CPU Address Map Controller for all nodes */
	for (node = 0; node < nodes; node++) {
		/* clear all MMIO Mapped Base/Limit Registers */
		for (i = 0; i < 8; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* clear all IO Space Base/Limit Registers */
		for (i = 0; i < 4; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* Set VGA Ram MMIO 0000A0000-0000BFFFF to Node0 sbLink */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84);
		PciData = 0x00000B00;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80);
		PciData = 0x00000A03;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set TOM1-FFFFFFFF to Node0 sbLink. */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x8C);
		PciData = 0x00FFFF00;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		TOM = (UINT32)MsrRead(TOP_MEM);
		PciData = (TOM >> 8) | 0x03;
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x88);
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set MMCONF space to Node0 sbLink with NP set.
		 * default E0000000-EFFFFFFF
		 * Just have all mmio set to non-posted,
		 * coreboot not implemente the range by range setting yet.
		 */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xBC);
		PciData = CONFIG_MMCONF_BASE_ADDRESS + (CONFIG_MMCONF_BUS_NUMBER * 0x100000) - 1;//1MB each bus
		PciData = (PciData >> 8) & 0xFFFFFF00;
		PciData |= 0x80; //NP
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xB8);
		PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) | 0x03;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);


		/* Set PCIO: 0x0 - 0xFFF000 to Node0 sbLink  and enabled VGA IO*/
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4);
		PciData = 0x00FFF000;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0);
		PciData = 0x00000033;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	}
}

void amd_initmmio(void)
{
	UINT64                        MsrReg;
	AMD_CONFIG_PARAMS             StdHeader;

	/*
	 * Set the MMIO Configuration Base Address and Bus Range onto
	 * MMIO configuration base Address MSR register.
	 */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (LibAmdBitScanReverse(CONFIG_MMCONF_BUS_NUMBER) << 2) | 1;
	LibAmdMsrWrite(0xC0010058, &MsrReg, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000 - CACHE_ROM_SIZE) | 5;
	LibAmdMsrWrite (0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20D, &MsrReg, &StdHeader);
}

#if 0
#include <cpuFamilyTranslation.h>

void cpu_show_tsc(void)
{
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **) & FamilySpecificServices,
				    &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);
}
#endif
