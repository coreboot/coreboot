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
 */


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "inteltool.h"

volatile uint8_t *mchbar;

static void write_mchbar32 (uint32_t addr, uint32_t val)
{
	* (volatile uint32_t *) (mchbar + addr) = val;
}

static uint32_t read_mchbar32 (uint32_t addr)
{
	return * (volatile uint32_t *) (mchbar + addr);
}

static uint8_t read_mchbar8 (uint32_t addr)
{
	return * (volatile uint8_t *) (mchbar + addr);
}

static u16 read_500 (int channel, u16 addr, int split)
{
	uint32_t val;
	write_mchbar32 (0x500 + (channel << 10), 0);
	while (read_mchbar32 (0x500 + (channel << 10)) & 0x800000);
	write_mchbar32 (0x500 + (channel << 10), 0x80000000 | (((read_mchbar8 (0x246 + (channel << 10)) >> 2) & 3) + 0xb88 - addr));
	while (read_mchbar32 (0x500 + (channel << 10)) & 0x800000);
	val = read_mchbar32 (0x508 + (channel << 10));

	return val & ((1 << split) - 1);
}

static inline u16 get_lane_offset (int slot, int rank, int lane)
{
	return 0x124 * lane + ((lane & 4) ? 0x23e : 0) + 11 * rank + 22 * slot - 0x452 * (lane == 8);
}

static inline u16 get_timing_register_addr (int lane, int tm, int slot, int rank)
{
	const u16 offs[] = { 0x1d, 0xa8, 0xe6, 0x5c };
	return get_lane_offset (slot, rank, lane) + offs[(tm + 3) % 4];
}

static void write_1d0 (u32 val, u16 addr, int bits, int flag)
{
	write_mchbar32 (0x1d0, 0);
	while (read_mchbar32 (0x1d0) & 0x800000);
	write_mchbar32 (0x1d4, (val & ((1 << bits) - 1)) | (2 << bits) | (flag << bits));
	write_mchbar32 (0x1d0, 0x40000000 | addr);
	while (read_mchbar32 (0x1d0) & 0x800000);
}

static u16 read_1d0 (u16 addr, int split)
{
	u32 val;
	write_mchbar32 (0x1d0, 0);
	while (read_mchbar32 (0x1d0) & 0x800000);
	write_mchbar32 (0x1d0, 0x80000000 | (((read_mchbar8 (0x246) >> 2) & 3) + 0x361 - addr));
	while (read_mchbar32 (0x1d0) & 0x800000);
	val = read_mchbar32 (0x1d8);
	write_1d0 (0, 0x33d, 0, 0);
	write_1d0 (0, 0x33d, 0, 0);
	return val & ((1 << split) - 1);
}

static void dump_timings (void)
{
	int channel, slot, rank, lane, i;
	printf ("Timings:\n");
	for (channel = 0; channel < 2; channel++)
		for (slot = 0; slot < 2; slot++)
			for (rank = 0; rank < 2; rank++) {
				printf ("channel %d, slot %d, rank %d\n", channel, slot, rank);
				for (lane = 0; lane < 9; lane++) {
					printf ("lane %d: ", lane);
					for (i = 0; i < 4; i++) {
						printf ("%x ", read_500 (channel,
							get_timing_register_addr (lane, i, slot, rank), 9));
					}
				printf ("\n");
				}
			}

	printf ("[178] = %x\n", read_1d0 (0x178, 7));
	printf ("[10b] = %x\n", read_1d0 (0x10b, 6));
}


/*
 * (G)MCH MMIO Config Space
 */
int print_mchbar(struct pci_dev *nb, struct pci_access *pacc, const char *dump_spd_file)
{
	int i, size = (16 * 1024);
	uint64_t mchbar_phys;
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
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
		mchbar_phys = pci_read_long(nb, 0x48) & 0xfffffffe;
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN:
		mchbar_phys = pci_read_long(nb, 0x48);
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		mchbar_phys &= 0x0000000fffffc000UL; /* 35:14 */
		break;
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
		mchbar_phys = pci_read_long(nb, 0x48);
		mchbar_phys |= ((uint64_t)pci_read_long(nb, 0x4c)) << 32;
		mchbar_phys &= 0x0000007fffff8000UL; /* 38:15 */
		size = 32768;
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

	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(mchbar + i))
			printf("0x%04x: 0x%08"PRIx32"\n", i, *(uint32_t *)(mchbar+i));
	}

	switch (nb->device_id)
	{
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN:
		printf ("clock_speed_index = %x\n", read_500 (0,0x609, 6) >> 1);
		dump_timings ();
		if (dump_spd_file != NULL)
			printf("\nCreating a memory timings file is not supported on this chipset.\n");
		break;
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
		ivybridge_dump_timings(dump_spd_file);
		break;
	default:
		if (dump_spd_file != NULL)
			printf("\nCreating a memory timings file is not supported on this chipset.\n");
	}
	unmap_physical((void *)mchbar, size);
	return 0;
}
