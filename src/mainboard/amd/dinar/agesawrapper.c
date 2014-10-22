/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <cpu/x86/mtrr.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "cpuRegisters.h"
#include "cpuCacheInit.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "Dispatcher.h"
#include "cpuCacheInit.h"
#include "amdlib.h"
#include "heapManager.h"
#include "Filecode.h"
#include <arch/io.h>

#include <southbridge/amd/cimx/sb700/gpio_oem.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/*Get the Bus Number from CONFIG_MMCONF_BUS_NUMBER, Please reference AMD BIOS BKDG docuemt about it*/
/*
BusRange: bus range identifier. Read-write. Reset: X. This specifies the number of buses in the
MMIO configuration space range. The size of the MMIO configuration space range varies with this
field as follows: the size is 1 Mbyte times the number of buses. This field is encoded as follows:
Bits    Buses  Bits    Buses
0h      1       5h      32
1h      2       6h      64
2h      4       7h      128
3h      8       8h      256
4h      16      Fh-9h   Reserved
*/
STATIC UINT8 GetEndBusNum(VOID)
{
	UINT64 BusNum;
	UINT8 Index;
	for (Index = 1; Index <= 8; Index++) {
		BusNum = CONFIG_MMCONF_BUS_NUMBER >> Index;
		if (BusNum == 1) {
			break;
		}
	}
	return Index;
}

AGESA_STATUS agesawrapper_amdinitcpuio(void)
{
	AGESA_STATUS Status;
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;
	UINT32 TopMem;
	UINT32 nodes;
	UINT32 node;
	UINT32 SbLink;
	UINT32 i;

	/* get the number of coherent nodes in the system */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 0, 0x60);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	nodes = ((PciData >> 4) & 7) + 1;	//nodes[6:4]
	/* Find out the Link ID of Node0 that connects to the
	 * Southbridge (system IO hub). e.g. family10 MCM Processor,
	 * SbLink is Processor0 Link2, internal Node0 Link3
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 0, 0x64);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	SbLink = (PciData >> 8) & 3;	//assume ganged
	/* Enable MMIO on AMD CPU Address Map Controller for all nodes */
	for (node = 0; node < nodes; node++) {
		/* clear all MMIO Mapped Base/Limit Registers */
		for (i = 0; i < 8; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}
		/* clear all IO Space Base/Limit Registers */
		for (i = 0; i < 4; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* Enable MMIO on AMD CPU Address Map Controller */

		/* Set VGA Ram MMIO 0000A0000-0000BFFFF to Node0 sbLink */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80);
		PciData = (0xA0000 >> 8) | 3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84);
		PciData = 0xB0000 >> 8;
		PciData &= (~0xFF);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set UMA MMIO. */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x88);
		LibAmdMsrRead(0xC001001A, &MsrReg, &StdHeader);
		TopMem = (UINT32) MsrReg;
		MsrReg = (MsrReg >> 8) | 3;
		PciData = (UINT32) MsrReg;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x8c);
		if (TopMem <= CONFIG_MMCONF_BASE_ADDRESS) {
			PciData = (CONFIG_MMCONF_BASE_ADDRESS - 1) >> 8;
		} else {
			PciData = (0x100000000ull - 1) >> 8;
		}
		PciData &= (~0xFF);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set PCIE MMIO. */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x90);
		PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) | 3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x94);
		PciData = ((CONFIG_MMCONF_BASE_ADDRESS + CONFIG_MMCONF_BUS_NUMBER * 4096 * 256 - 1) >> 8) & (~0xFF);
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set XAPIC MMIO. 24K */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x98);
		PciData = (0xFEC00000 >> 8) | 3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x9c);
		PciData = ((0xFEC00000 + 6 * 4096 - 1) >> 8);
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set Local APIC MMIO. 4K*4= 16K, Llano CPU are 4 cores */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xA0);
		PciData = (0xFEE00000 >> 8) | 3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xA8);
		PciData = (0xFEE00000 + 4 * 4096 - 1) >> 8;
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set PCIO: 0x0 - 0xFFF000  and enabled VGA IO */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0);
		PciData = 0x13;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4);
		PciData = 0x00FFF000;
		PciData &= (~0x7F);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	}
	Status = AGESA_SUCCESS;
	return Status;
}

AGESA_STATUS agesawrapper_amdinitmmio(void)
{
	AGESA_STATUS Status;
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;

	/*
	   Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	   Address MSR register.
	 */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (GetEndBusNum() << 2) | 1;
	LibAmdMsrWrite(0xC0010058, &MsrReg, &StdHeader);

	/*
	   Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	 */
	LibAmdMsrRead(0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | BIT46;
	LibAmdMsrWrite(0xC001001F, &MsrReg, &StdHeader);

	/* Set PCIE MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x90);
	PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) | 3;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x94);
	PciData = ((CONFIG_MMCONF_BASE_ADDRESS + CONFIG_MMCONF_BUS_NUMBER * 4096 * 256 - 1) >> 8) | MMIO_NP_BIT;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Enable memory access */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0x04);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);
	PciData |= BIT1;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0x04);
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000 - CACHE_ROM_SIZE) | 5;
	LibAmdMsrWrite(0x20E, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20F, &MsrReg, &StdHeader);

	Status = AGESA_SUCCESS;
	return Status;
}
