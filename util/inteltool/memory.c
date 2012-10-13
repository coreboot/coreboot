/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
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
#include <stdlib.h>
#include <inttypes.h>
#include "inteltool.h"

static const io_register_t sandybridge_mch_registers[] = {
/* Channel 0 */
	{ 0x4000, 4, "TC_DBP_C0" }, // Timing of DDR Bin Parameters
	{ 0x4004, 4, "TC_RAP_C0" }, // Timing of DDR Regular Access Parameters
	{ 0x4028, 4, "SC_IO_LATENCY_C0" }, // IO Latency Configuration
	{ 0x42A4, 4, "TC_SRFTP_C0" }, // Self-Refresh Timing Parameters
	{ 0x40B0, 4, "PM_PDWN_config_C0" }, // Power-down Configuration
	{ 0x4294, 4, "TC_RFP_C0" }, // Refresh Parameters
	{ 0x4298, 4, "TC_RFTP_C0" }, // Refresh Timing Parameters
/* Channel 1 */
	{ 0x4400, 4, "TC_DBP_C1" }, // Timing of DDR Bin Parameters
	{ 0x4404, 4, "TC_RAP_C1" }, // Timing of DDR Regular Access Parameters
	{ 0x4428, 4, "SC_IO_LATENCY_C1" }, // IO Latency Configuration
	{ 0x46A4, 4, "TC_SRFTP_C1" }, // Self-Refresh Timing Parameters
	{ 0x44B0, 4, "PM_PDWN_config_C1" }, // Power-down Configuration
	{ 0x4694, 4, "TC_RFP_C1" }, // Refresh Parameters
	{ 0x4698, 4, "TC_RFTP_C1" }, // Refresh Timing Parameters
/* Integrated Memory Peripheral Hub (IMPH) */
	{ 0x740C, 4, "CRDTCTL3" }, // Credit Control 3
/* Common Registers */
	{ 0x5000, 4, "MAD_CHNL" }, // Address decoder Channel Configuration
	{ 0x5004, 4, "MAD_DIMM_ch0" }, // Address Decode Channel 0
	{ 0x5008, 4, "MAD_DIMM_ch1" }, // Address Decode Channel 1
	{ 0x5060, 4, "PM_SREF_config" }, // Self Refresh Configuration
/* MMIO Registers Broadcast Group */
	{ 0x4CB0, 4, "PM_PDWN_config" }, // Power-down Configuration
	{ 0x4F84, 4, "PM_CMD_PWR" }, // Power Management Command Power
	{ 0x4F88, 4, "PM_BW_LIMIT_config" }, // BW Limit Configuration
	{ 0x4F8C, 4, "RESERVED" }, // Reserved, default value - 0xFF1D1519
/* PCU MCHBAR Registers */
	{ 0x5880, 4, "MEM_TRML_ESTIMATION_CONFIG" }, // Memory Thermal Estimation Configuration
	{ 0x5884, 4, "RESERVED" }, // Reserved
	{ 0x5888, 4, "MEM_TRML_THRESHOLDS_CONFIG" }, // Memory Thermal Thresholds Configuration
	{ 0x58A0, 4, "MEM_TRML_STATUS_REPORT" }, // Memory Thermal Status Report
	{ 0x58A4, 4, "MEM_TRML_TEMPERATURE_REPORT" }, // Memory Thermal Temperature Report
	{ 0x58A8, 4, "MEM_TRML_INTERRUPT" }, // Memory Thermal Interrupt
	{ 0x5948, 4, "GT_PERF_STATUS" }, // GT Performance Status
	{ 0x5998, 4, "RP_STATE_CAP" }, // RP State Capability
	{ 0x5D10, 8, "SSKPD" }, // Sticky Scratchpad Data
};

/*
 * (G)MCH MMIO Config Space
 */
int print_mchbar(struct pci_dev *nb, struct pci_access *pacc)
{
	int i, size = (16 * 1024);
	volatile uint8_t *mchbar;
	uint64_t mchbar_phys;
	const io_register_t *mch_registers = NULL;
	struct pci_dev *nb_device6; /* "overflow device" on i865 */
	uint16_t pcicmd6;

	printf("\n============= MCHBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82865:
		/*
		 * On i865, the memory access enable/disable bit (MCHBAREN on
		 * i945/i965) is not in the MCHBAR (i945/i965) register but in
		 * the PCICMD6 register. BAR6 and PCICMD6 reside on device 6.
		 *
		 * The actual base address is in BAR6 on i865 where on
		 * i945/i965 the base address is in MCHBAR.
		 */
		nb_device6 = pci_get_dev(pacc, 0, 0, 0x06, 0);  /* Device 6 */
		mchbar_phys = pci_read_long(nb_device6, 0x10);  /* BAR6 */
		pcicmd6 = pci_read_long(nb_device6, 0x04);      /* PCICMD6 */

		/* Try to enable Memory Access Enable (MAE). */
		if (!(pcicmd6 & (1 << 1))) {
			printf("Access to BAR6 is currently disabled, "
			       "attempting to enable.\n");
			pci_write_long(nb_device6, 0x04, pcicmd6 | (1 << 1));
			if (pci_read_long(nb_device6, 0x04) & (1 << 1))
				printf("Enabled successfully.\n");
			else
				printf("Enable FAILED!\n");
		}
		mchbar_phys &= 0xfffff000; /* Bits 31:12 from BAR6 */
		break;
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		mchbar_phys = pci_read_long(nb, 0x44) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
		mchbar_phys = pci_read_long(nb, 0x48) & 0xfffffffe;
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
		mchbar_phys = pci_read_long(nb, 0x48);

		/* Test if bit 0 of the MCHBAR reg is 1 to enable memory reads.
		 * If it isn't, try to set it. This may fail, because there is
		 * some bit that locks that bit, and isn't in the public
		 * datasheets.
		 */

		if(!(mchbar_phys & 1))
		{
			printf("Access to the MCHBAR is currently disabled, "
				   "attempting to enable.\n");
			mchbar_phys |= 0x1;
			pci_write_long(nb, 0x48, mchbar_phys);
			if(pci_read_long(nb, 0x48) & 1)
				printf("Enabled successfully.\n");
			else
				printf("Enable FAILED!\n");
		}
		mchbar_phys &= 0xfffffffe;
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82443LX:
	case PCI_DEVICE_ID_INTEL_82443BX:
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82830M:
		printf("This northbridge does not have MCHBAR.\n");
		return 1;
	case PCI_DEVICE_ID_INTEL_82X4X:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
		mchbar_phys = pci_read_long(nb, 0x48) & 0xfffffffe;
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN:
		mchbar_phys = pci_read_long(nb, 0x48);
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		mchbar_phys &= 0x0000000fffffc000UL; /* 35:14 */
		mch_registers = NULL; /* No public documentation */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN:
		mch_registers = sandybridge_mch_registers;
		size = ARRAY_SIZE(sandybridge_mch_registers);
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN: /* pretty printing not implemented yet */
		mchbar_phys = pci_read_long(nb, 0x48);
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		mchbar_phys &= 0x0000007fffff8000UL; /* 38:15 */
		break;
	default:
		printf("Error: Dumping MCHBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	mchbar = map_physical(mchbar_phys, size);

	if (mchbar == NULL) {
		if (nb->device_id == PCI_DEVICE_ID_INTEL_82865)
			perror("Error mapping BAR6");
		else
			perror("Error mapping MCHBAR");
		exit(1);
	}

	if (nb->device_id == PCI_DEVICE_ID_INTEL_82865)
		printf("BAR6 = 0x%08" PRIx64 " (MEM)\n\n", mchbar_phys);
	else
		printf("MCHBAR = 0x%08" PRIx64 " (MEM)\n\n", mchbar_phys);

	if (mch_registers != NULL) {
		printf("%d registers:\n", size);
		for (i = 0; i < size; i++) {
			switch (mch_registers[i].size) {
				case 8:
					printf("mchbase+0x%04x: 0x%016lx (%s)\n",
						mch_registers[i].addr,
						*(uint64_t *)(mchbar+mch_registers[i].addr),
						mch_registers[i].name);
					break;
				case 4:
					printf("mchbase+0x%04x: 0x%08x         (%s)\n",
						mch_registers[i].addr,
						*(uint32_t *)(mchbar+mch_registers[i].addr),
						mch_registers[i].name);
					break;
				case 2:
					printf("mchbase+0x%04x: 0x%04x             (%s)\n",
						mch_registers[i].addr,
						*(uint16_t *)(mchbar+mch_registers[i].addr),
						mch_registers[i].name);
					break;
				case 1:
					printf("mchbase+0x%04x: 0x%02x               (%s)\n",
						mch_registers[i].addr,
						*(uint8_t *)(mchbar+mch_registers[i].addr),
						mch_registers[i].name);
					break;
			}
		}
	} else {
		for (i = 0; i < size; i += 4) {
			if (*(uint32_t *)(mchbar + i))
				printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(mchbar+i));
		}
	}

	unmap_physical((void *)mchbar, size);
	return 0;
}


