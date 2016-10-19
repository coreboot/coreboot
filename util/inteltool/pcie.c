/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 * Copyright (C) 2012 Anton Kochkov
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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "inteltool.h"

/* 320766 */
static const io_register_t nehalem_dmi_registers[] = {
	{ 0x00, 4, "DMIVCH" },		// DMI Virtual Channel Capability Header
	{ 0x04, 4, "DMIVCCAP1" },	// DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIVCCAP2" },	// DMI Port VC Capability Register 2
	{ 0x0C, 4, "DMIVCCTL" },	// DMI Port VC Control
	{ 0x10, 4, "DMIVC0RCAP" },	// DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" },	// DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" },	// DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" },	// DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL" },	// DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMIVC1RSTS" },	// DMI VC1 Resource Status
/*	... - Reserved */
	{ 0x84, 4, "DMILCAP" },		// DMI Link Capabilities
	{ 0x88, 2, "DMILCTL" },		// DMI Link Control
	{ 0x8A, 2, "DMILSTS" },		// DMI Link Status
/*	... - Reserved */
};

/* 322812 */
static const io_register_t westmere_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH" },	// DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" },	// DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" },	// DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMIPVCCTL" },	// DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMIVC0RCAP" },	// DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" },	// DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" },	// DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" },	// DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL1" },	// DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" },	// Reserved */
	{ 0x26, 2, "DMIC1RSTS" },	// DMI VC1 Resource Status
/*	... - Reserved */
	{ 0x84, 4, "DMILCAP" },		// DMI Link Capabilities
	{ 0x88, 2, "DMILCTL" },		// DMI Link Control
	{ 0x8A, 2, "DMILSTS" },		// DMI Link Status
/*	... - Reserved */
};

static const io_register_t sandybridge_dmi_registers[] = {
	{ 0x00, 4, "DMI VCECH" }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMI PVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMI PVVAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMI PVCCTL" }, // DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMI VC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMI VC0RCTL" }, // DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMI VC0RSTS" }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMI VC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMI VC1RCTL" }, // DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMI VC1RSTS" }, // DMI VC1 Resource Status
	{ 0x28, 4, "DMI VCPRCAP" }, // DMI VCp Resource Capability
	{ 0x2C, 4, "DMI VCPRCTL" }, // DMI VCp Resource Control
/*	{ 0x30, 2, "RSVD" }, // Reserved */
	{ 0x32, 2, "DMI VCPRSTS" }, // DMI VCp Resource Status
	{ 0x34, 4, "DMI VCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMI VCMRCTL" }, // DMI VCm Resource Control
/*	{ 0x3C, 2, "RSVD" }, // Reserved */
	{ 0x3E, 2, "DMI VCMRSTS" }, // DMI VCm Resource Status
/*	{ 0x40, 4, "RSVD" }, // Reserved */
	{ 0x44, 4, "DMI ESC" }, // DMI Element Self Description
/*	{ 0x48, 8, "RSVD" }, // Reserved */
	{ 0x50, 4, "DMI LE1D" }, // DMI Link Entry 1 Description
/*	{ 0x54, 4, "RSVD" }, // Reserved */
	{ 0x58, 4, "DMI LE1A" }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMI LUE1A" }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMI LE2D" }, // DMI Link Entry 2 Description
/*	{ 0x64, 4, "RSVD" }, // Reserved */
	{ 0x68, 4, "DMI LE2A" }, // DMI Link Entry 2 Address
/*	{ 0x6C, 4, "RSVD" }, // Reserved
	{ 0x70, 8, "RSVD" }, // Reserved
	{ 0x78, 8, "RSVD" }, // Reserved
	{ 0x80, 4, "RSVD" }, // Reserved */
	{ 0x84, 4, "LCAP" }, // Link Capabilities
	{ 0x88, 2, "LCTL" }, // Link Control
	{ 0x8A, 2, "LSTS" }, // Link Status
/*	{ 0x8C, 4, "RSVD" }, // Reserved
	{ 0x90, 4, "RSVD" }, // Reserved
	{ 0x94, 4, "RSVD" }, // Reserved */
	{ 0x98, 2, "LCTL2" }, // Link Control 2
	{ 0x9A, 2, "LSTS2" }, // Link Status 2
/*	... - Reserved */
	{ 0xBC0, 4, "AFE_BMUF0" }, // AFE BMU Configuration Function 0
	{ 0xBC4, 4, "RSVD" }, // Reserved
	{ 0xBC8, 4, "RSVD" }, // Reserved
	{ 0xBCC, 4, "AFE_BMUT0" }, // AFE BMU Configuration Test 0
/*	... - Reserved */
};

/*
 * All Haswell DMI Registers per
 *
 * Mobile 4th Generation Intel Core TM Processor Family, Mobile Intel Pentium Processor Family,
 * and Mobile Intel Celeron Processor Family
 * Datasheet Volume 2
 * 329002-002
 */
static const io_register_t haswell_ult_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH" }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMI PVCCTL" }, // DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMIVC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" }, // DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL" }, // DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMIVC1RSTS" }, // DMI VC1 Resource Status
	{ 0x28, 4, "DMIVCPRCAP" }, // DMI VCp Resource Capability
	{ 0x2C, 4, "DMIVCPRCTL" }, // DMI VCp Resource Control
/*	{ 0x30, 2, "RSVD" }, // Reserved */
	{ 0x32, 2, "DMIVCPRSTS" }, // DMI VCp Resource Status
	{ 0x34, 4, "DMIVCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMIVCMRCTL" }, // DMI VCm Resource Control
/*	{ 0x3C, 2, "RSVD" }, // Reserved */
	{ 0x3E, 2, "DMIVCMRSTS" }, // DMI VCm Resource Status
	{ 0x40, 4, "DMIRCLDECH" }, // DMI Root Complex Link Declaration */
	{ 0x44, 4, "DMIESD" }, // DMI Element Self Description
/*	{ 0x48, 4, "RSVD" }, // Reserved */
/*	{ 0x4C, 4, "RSVD" }, // Reserved */
	{ 0x50, 4, "DMILE1D" }, // DMI Link Entry 1 Description
/*	{ 0x54, 4, "RSVD" }, // Reserved */
	{ 0x58, 4, "DMILE1A" }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMILUE1A" }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMILE2D" }, // DMI Link Entry 2 Description
/*	{ 0x64, 4, "RSVD" }, // Reserved */
	{ 0x68, 4, "DMILE2A" }, // DMI Link Entry 2 Address
/*	{ 0x6C, 4, "RSVD" }, // Reserved */
/*	{ 0x70, 4, "RSVD" }, // Reserved */
/*	{ 0x74, 4, "RSVD" }, // Reserved */
/*	{ 0x78, 4, "RSVD" }, // Reserved */
/*	{ 0x7C, 4, "RSVD" }, // Reserved */
/*	{ 0x80, 4, "RSVD" }, // Reserved */
/*	{ 0x84, 4, "RSVD" }, // Reserved */
	{ 0x88, 2, "LCTL" }, // Link Control
	/*  ... - Reserved */
	{ 0x1C4, 4, "DMIUESTS" }, // DMI Uncorrectable Error Status
	{ 0x1C8, 4, "DMIUEMSK" }, // DMI Uncorrectable Error Mask
	{ 0x1D0, 4, "DMICESTS" }, // DMI Correctable Error Status
	{ 0x1D4, 4, "DMICEMSK" }, // DMI Correctable Error Mask
/*  ... - Reserved */
};

/*
 * Egress Port Root Complex MMIO configuration space
 */
int print_epbar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *epbar;
	uint64_t epbar_phys;

	printf("\n============= EPBAR =============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82975X:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		epbar_phys |= ((uint64_t)pci_read_long(nb, 0x44)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82830M:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have EPBAR.\n");
		return 1;
	default:
		printf("Error: Dumping EPBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	epbar = map_physical(epbar_phys, size);

	if (epbar == NULL) {
		perror("Error mapping EPBAR");
		exit(1);
	}

	printf("EPBAR = 0x%08" PRIx64 " (MEM)\n\n", epbar_phys);
	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(epbar + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(epbar+i));
	}

	unmap_physical((void *)epbar, size);
	return 0;
}

/*
 * MCH-ICH Serial Interconnect Ingress Root Complex MMIO configuration space
 */
int print_dmibar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *dmibar;
	uint64_t dmibar_phys;
	const io_register_t *dmi_registers = NULL;

	printf("\n============= DMIBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		dmibar_phys = pci_read_long(nb, 0x4c) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
		dmibar_phys = pci_read_long(nb, 0x68) & 0xfffffffe;
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have DMIBAR.\n");
		return 1;
	case PCI_DEVICE_ID_INTEL_82X58:
		dmibar_phys = pci_read_long(nb, 0x50) & 0xfffff000;
		break;
	case PCI_DEVICE_ID_INTEL_CORE_0TH_GEN:
		/* DMIBAR is called DMIRCBAR in Nehalem */
		dmibar_phys = pci_read_long(nb, 0x50) & 0xfffff000; /* 31:12 */
		dmi_registers = nehalem_dmi_registers;
		size = ARRAY_SIZE(nehalem_dmi_registers);
		break;
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN:
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000000ffffff000UL; /* 35:12 */
		dmi_registers = westmere_dmi_registers;
		size = ARRAY_SIZE(westmere_dmi_registers);
		break;
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
		dmi_registers = sandybridge_dmi_registers;
		size = ARRAY_SIZE(sandybridge_dmi_registers);
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D: /* pretty printing not implemented yet */
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
		dmi_registers = haswell_ult_dmi_registers;
		size = ARRAY_SIZE(haswell_ult_dmi_registers);
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;

	default:
		printf("Error: Dumping DMIBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	dmibar = map_physical(dmibar_phys, size);

	if (dmibar == NULL) {
		perror("Error mapping DMIBAR");
		exit(1);
	}

	printf("DMIBAR = 0x%08" PRIx64 " (MEM)\n\n", dmibar_phys);
	if (dmi_registers != NULL) {
		for (i = 0; i < size; i++) {
			switch (dmi_registers[i].size) {
				case 4:
					printf("dmibase+0x%04x: 0x%08x (%s)\n",
						dmi_registers[i].addr,
						*(uint32_t *)(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
				case 2:
					printf("dmibase+0x%04x: 0x%04x     (%s)\n",
						dmi_registers[i].addr,
						*(uint16_t *)(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
				case 1:
					printf("dmibase+0x%04x: 0x%02x       (%s)\n",
						dmi_registers[i].addr,
						*(uint8_t *)(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
			}
		}
	} else {
		for (i = 0; i < size; i += 4) {
			if (*(uint32_t *)(dmibar + i))
				printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(dmibar+i));
		}
	}

	unmap_physical((void *)dmibar, size);
	return 0;
}

/*
 * PCIe MMIO configuration space
 */
int print_pciexbar(struct pci_dev *nb)
{
	uint64_t pciexbar_reg;
	uint64_t pciexbar_phys;
	volatile uint8_t *pciexbar;
	int max_busses, devbase, i;
	int bus, dev, fn;

	printf("========= PCIEXBAR ========\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		pciexbar_reg = pci_read_long(nb, 0x48);
		break;
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
		pciexbar_reg = pci_read_long(nb, 0x60);
		pciexbar_reg |= ((uint64_t)pci_read_long(nb, 0x64)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("Error: This northbridge does not have PCIEXBAR.\n");
		return 1;
	default:
		printf("Error: Dumping PCIEXBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	if (!(pciexbar_reg & (1 << 0))) {
		printf("PCIEXBAR register is disabled.\n");
		return 0;
	}

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		pciexbar_phys = pciexbar_reg & (0xffULL << 28);
		max_busses = 256;
		break;
	case 1: // 128M
		pciexbar_phys = pciexbar_reg & (0x1ffULL << 27);
		max_busses = 128;
		break;
	case 2: // 64M
		pciexbar_phys = pciexbar_reg & (0x3ffULL << 26);
		max_busses = 64;
		break;
	default: // RSVD
		printf("Undefined address base. Bailing out.\n");
		return 1;
	}

	printf("PCIEXBAR: 0x%08" PRIx64 "\n", pciexbar_phys);

	pciexbar = map_physical(pciexbar_phys, (max_busses * 1024 * 1024));

	if (pciexbar == NULL) {
		perror("Error mapping PCIEXBAR");
		exit(1);
	}

	for (bus = 0; bus < max_busses; bus++) {
		for (dev = 0; dev < 32; dev++) {
			for (fn = 0; fn < 8; fn++) {
				devbase = (bus * 1024 * 1024) + (dev * 32 * 1024) + (fn * 4 * 1024);

				if (*(uint16_t *)(pciexbar + devbase) == 0xffff)
					continue;

				/* This is a heuristics. Anyone got a better check? */
				if( (*(uint32_t *)(pciexbar + devbase + 256) == 0xffffffff) &&
					(*(uint32_t *)(pciexbar + devbase + 512) == 0xffffffff) ) {
#if DEBUG
					printf("Skipped non-PCIe device %02x:%02x.%01x\n", bus, dev, fn);
#endif
					continue;
				}

				printf("\nPCIe %02x:%02x.%01x extended config space:", bus, dev, fn);
				for (i = 0; i < 4096; i++) {
					if((i % 0x10) == 0)
						printf("\n%04x:", i);
					printf(" %02x", *(pciexbar+devbase+i));
				}
				printf("\n");
			}
		}
	}

	unmap_physical((void *)pciexbar, (max_busses * 1024 * 1024));

	return 0;
}
