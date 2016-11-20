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
#include <northbridge/amd/agesa/agesawrapper.h>
#include "amdlib.h"

/* Define AMD Ontario APPU SSID/SVID */
#define AMD_APU_SVID		0x1022
#define AMD_APU_SSID		0x1234

void amd_initcpuio(void)
{
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xF4);
	PciData = 1;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* The platform BIOS needs to ensure the memory ranges of SB800 legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions.
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x84);
	PciData = 0x00FEDF00;	// last address before processor local APIC at FEE00000
	PciData |= 1 << 7;	// set NP (non-posted) bit
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x80);
	PciData = (0xFED00000 >> 8) | 3;	// lowest NP address is HPET at FED00000
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Map the remaining PCI hole as posted MMIO */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x8C);
	PciData = 0x00FECF00;	// last address before non-posted range
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	LibAmdMsrRead(0xC001001A, &MsrReg, &StdHeader);
	MsrReg = (MsrReg >> 8) | 3;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x88);
	PciData = (UINT32) MsrReg;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Send all IO (0000-FFFF) to southbridge. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC4);
	PciData = 0x0000F000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC0);
	PciData = 0x00000003;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
}

void amd_initmmio(void)
{
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;

	/*
	   Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	   Address MSR register.
	 */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (LibAmdBitScanReverse(CONFIG_MMCONF_BUS_NUMBER) << 2) | 1;
	LibAmdMsrWrite(0xC0010058, &MsrReg, &StdHeader);

	/*
	   Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	 */
	LibAmdMsrRead(0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | 0x0000400000000000ull;
	LibAmdMsrWrite(0xC001001F, &MsrReg, &StdHeader);

	/* Set Ontario Link Data */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0xE0);
	PciData = 0x01308002;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0xE4);
	PciData = (AMD_APU_SSID << 0x10) | AMD_APU_SVID;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000ull - CACHE_ROM_SIZE) | MTRR_TYPE_WRPROT;
	LibAmdMsrWrite (0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | MTRR_PHYS_MASK_VALID;
	LibAmdMsrWrite (0x20D, &MsrReg, &StdHeader);

	/* Set P-state 0 (1600 MHz) early to save a few ms of boot time */
	MsrReg = 0;
	LibAmdMsrWrite (0xC0010062, &MsrReg, &StdHeader);
}

void amd_initenv(void)
{
	AMD_INTERFACE_PARAMS AmdParamStruct;
	PCI_ADDR PciAddress;
	UINT32 PciValue;

	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;
	/* Write to D1F0x18 */
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x00010100;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Legacy Bridge Mode
	 *      Modify B1D5F0x18
	 */
	PciAddress.Address.Bus = 1;
	PciAddress.Address.Device = 5;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Legacy Bridge Mode
	 * Modify B1D5F0x10
	 */
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Pcie Mode
	 *      Modify B0D1F0x18
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Pcie Mode
	 *      Modify B0D1F0x10
	 */
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Base and Limit Address
	 *      Modify B0D1F0x20
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x20;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96009600;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Prefetchable Memory Limit and Base
	 *      Modify B0D1F0x24
	 */
	PciAddress.Address.Register = 0x24;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x8FF18001;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
}
