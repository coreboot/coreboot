/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
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

#include "raminit.h"

void exit_from_self(int controllers, const struct mem_controller *ctrl,
		    struct sys_info *sysinfo)
{
	int i;
	u32 dcl, dch;
	u32 pcidev;
	u8 bitmask;
	u8 is_post_rev_g;
	u32 local_cpuid;

	for (i = 0; i < controllers; i++) {
		if (!sysinfo->ctrl_present[i])
			continue;
		/* Skip everything if I don't have any memory on this controller */
		dch = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_HIGH);
		if (!(dch & DCH_MemClkFreqVal)) {
			continue;
		}

		local_cpuid = pci_read_config32(ctrl[i].f3, 0xfc);
		is_post_rev_g = ((local_cpuid & 0xfff00) > 0x50f00);

		/* ChipKill */
		dcl = pci_read_config32(ctrl[i].f2, DRAM_CONFIG_LOW);
		if (dcl & DCL_DimmEccEn) {
			u32 mnc;
			printk(BIOS_SPEW, "ECC enabled\n");
			mnc = pci_read_config32(ctrl[i].f3, MCA_NB_CONFIG);
			mnc |= MNC_ECC_EN;
			if (dcl & DCL_Width128) {
				mnc |= MNC_CHIPKILL_EN;
			}
			pci_write_config32(ctrl[i].f3, MCA_NB_CONFIG, mnc);
		}

		printk(BIOS_DEBUG, "before resume errata #%d\n",
			     (is_post_rev_g) ? 270 : 125);
		/*
		   1. Restore memory controller registers as normal.
		   2. Set the DisAutoRefresh bit (Dev:2x8C[18]). (270 only)
		   3. Set the EnDramInit bit (Dev:2x7C[31]), clear all other bits in the same register).
		   4. Wait at least 750 us.
		   5. Clear the EnDramInit bit.
		   6. Clear the DisAutoRefresh bit. (270 only)
		   7. Read the value of Dev:2x80 and write that value back to Dev:2x80.
		   8. Set the exit from the self refresh bit (Dev:2x90[1]).
		   9. Clear the exit from self refresh bit immediately.
		   Note: Steps 8 and 9 must be executed in a single 64-byte aligned uninterrupted instruction stream.
		 */

		enable_lapic();
		init_timer();

		printk(BIOS_DEBUG, "before exit errata - timer enabled\n");

		if (is_post_rev_g) {
			dcl =
			    pci_read_config32(ctrl[i].f2,
					      DRAM_TIMING_HIGH);
			dcl |= (1 << 18);
			pci_write_config32(ctrl[i].f2, DRAM_TIMING_HIGH,
					   dcl);
		}

		dcl = DI_EnDramInit;
		pci_write_config32(ctrl[i].f2, DRAM_INIT, dcl);

		udelay(800);

		printk(BIOS_DEBUG, "before exit errata - after mdelay\n");

		dcl = pci_read_config32(ctrl[i].f2, DRAM_INIT);
		dcl &= ~DI_EnDramInit;
		pci_write_config32(ctrl[i].f2, DRAM_INIT, dcl);

		if (is_post_rev_g) {
			dcl =
			    pci_read_config32(ctrl[i].f2,
					      DRAM_TIMING_HIGH);
			dcl &= ~(1 << 18);
			pci_write_config32(ctrl[i].f2, DRAM_TIMING_HIGH,
					   dcl);
		}

		dcl = pci_read_config32(ctrl[i].f2, DRAM_BANK_ADDR_MAP);
		pci_write_config32(ctrl[i].f2, DRAM_BANK_ADDR_MAP, dcl);

		/* I was unable to do that like: ctrl[i].f2->path.pci.devfn << 8 */
		pcidev =
		    0x80000000 | ((((ctrl[i].node_id + 0x18) << 3) | 0x2)
				  << 8) | 0x90;
		printk(BIOS_DEBUG, "pcidev is %x\n", pcidev);
		bitmask = 2;
		__asm__ __volatile__("pushl %0\n\t"
				     "movw $0xcf8, %%dx\n\t"
				     "out %%eax, (%%dx)\n\t"
				     "movw $0xcfc, %%dx\n\t"
				     "inl %%dx, %%eax\n\t"
				     "orb %1, %%al\n\t"
				     "not %1\n\t"
				     ".align 64\n\t"
				     "outl  %%eax, (%%dx) \n\t"
				     "andb %1, %%al\n\t"
				     "outl %%eax, (%%dx)\n\t"
				     "popl %0\n\t"::"a"(pcidev),
				     "q"(bitmask):"edx");
	}

	printk(BIOS_DEBUG, "after exit errata\n");


	for (i = 0; i < controllers; i++) {
		u32 dcm;
		if (!sysinfo->ctrl_present[i])
			continue;
		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00)
			continue;

		printk(BIOS_DEBUG, "Exiting memory from self refresh: ");
		int loops = 0;
		do {
			loops++;
			if ((loops & 1023) == 0) {
				printk(BIOS_DEBUG, ".");
			}
			dcm =
			    pci_read_config32(ctrl[i].f2, DRAM_CTRL_MISC);
		} while (((dcm & DCM_MemClrStatus) ==
			  0) /* || ((dcm & DCM_DramEnabled) == 0) */ );

		if (loops >= TIMEOUT_LOOPS) {
			printk(BIOS_DEBUG, "timeout with with cntrl[%d]\n", i);
			continue;
		}

		printk(BIOS_DEBUG, " done\n");
	}

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	/* init hw mem hole here */
	/* DramHoleValid bit only can be set after MemClrStatus is set by Hardware */
	set_hw_mem_hole(controllers, ctrl);
#endif

	/* store tom to sysinfo, and it will be used by dqs_timing */
	{
		msr_t msr;
		//[1M, TOM)
		msr = rdmsr(TOP_MEM);
		sysinfo->tom_k = ((msr.hi << 24) | (msr.lo >> 8)) >> 2;

		//[4G, TOM2)
		msr = rdmsr(TOP_MEM2);
		sysinfo->tom2_k = ((msr.hi << 24) | (msr.lo >> 8)) >> 2;
	}

	for (i = 0; i < controllers; i++) {

		if (!sysinfo->ctrl_present[i])
			continue;

		/* Skip everything if I don't have any memory on this controller */
		if (sysinfo->meminfo[i].dimm_mask == 0x00)
			continue;

		dqs_restore_MC_NVRAM((ctrl + i)->f2);
		sysinfo->mem_trained[i] = 1;	// mem was trained
	}
}
