/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <ppc_asm.tmpl>
#include <ppc.h>
#include <ppc4xx.h>
#include <ppcreg.h>
#include <string.h>

/* ------------------------------------------------------------------------- */

#define ONE_BILLION        1000000000

void get_sys_info (struct ppc4xx_sys_info * sysInfo)
{
	unsigned long pllmr;
	unsigned long sysClkPeriodPs = ONE_BILLION / (CONFIG_SYS_CLK_FREQ * 1000);
	unsigned pvr = ppc_getpvr();
	unsigned long psr;
	unsigned long m;

	memset(sysInfo, 0, sizeof(*sysInfo));

	/*
	 * Read PLL Mode register
	 */
	pllmr = mfdcr(CPC0_PLLMR);

	/*
	 * Read Pin Strapping register
	 */
	psr = mfdcr(CPC0_PSR);

	/*
	 * Determine FWD_DIV.
	 */
	sysInfo->pllFwdDiv = 8 - ((pllmr & PLLMR_FWD_DIV_MASK) >> 29);

	/*
	 * Determine FBK_DIV.
	 */
	sysInfo->pllFbkDiv = ((pllmr & PLLMR_FB_DIV_MASK) >> 25);
	if (sysInfo->pllFbkDiv == 0) {
		sysInfo->pllFbkDiv = 16;
	}

	/*
	 * Determine PLB_DIV.
	 */
	sysInfo->pllPlbDiv = ((pllmr & PLLMR_CPU_TO_PLB_MASK) >> 17) + 1;

	/*
	 * Determine PCI_DIV.
	 */
	sysInfo->pllPciDiv = ((pllmr & PLLMR_PCI_TO_PLB_MASK) >> 13) + 1;

	/*
	 * Determine EXTBUS_DIV.
	 */
	sysInfo->pllExtBusDiv = ((pllmr & PLLMR_EXB_TO_PLB_MASK) >> 11) + 2;

	/*
	 * Determine OPB_DIV.
	 */
	sysInfo->pllOpbDiv = ((pllmr & PLLMR_OPB_TO_PLB_MASK) >> 15) + 1;

	/*
	 * Check if PPC405GPr used (mask minor revision field)
	 */
	if ((pvr & 0xfffffff0) == (PVR_405GPR_RB & 0xfffffff0)) {
		/*
		 * Determine FWD_DIV B (only PPC405GPr with new mode strapping).
		 */
		sysInfo->pllFwdDivB = 8 - (pllmr & PLLMR_FWDB_DIV_MASK);

		/*
		 * Determine factor m depending on PLL feedback clock source
		 */
		if (!(psr & PSR_PCI_ASYNC_EN)) {
			if (psr & PSR_NEW_MODE_EN) {
				/*
				 * sync pci clock used as feedback (new mode)
				 */
				m = 1 * sysInfo->pllFwdDivB * 2 * sysInfo->pllPciDiv;
			} else {
				/*
				 * sync pci clock used as feedback (legacy mode)
				 */
				m = 1 * sysInfo->pllFwdDivB * sysInfo->pllPlbDiv * sysInfo->pllPciDiv;
			}
		} else if (psr & PSR_NEW_MODE_EN) {
			if (psr & PSR_PERCLK_SYNC_MODE_EN) {
				/*
				 * PerClk used as feedback (new mode)
				 */
				m = 1 * sysInfo->pllFwdDivB * 2 * sysInfo->pllExtBusDiv;
			} else {
				/*
				 * CPU clock used as feedback (new mode)
				 */
				m = sysInfo->pllFbkDiv * sysInfo->pllFwdDiv;
			}
		} else if (sysInfo->pllExtBusDiv == sysInfo->pllFbkDiv) {
			/*
			 * PerClk used as feedback (legacy mode)
			 */
			m = 1 * sysInfo->pllFwdDivB * sysInfo->pllPlbDiv * sysInfo->pllExtBusDiv;
		} else {
			/*
			 * PLB clock used as feedback (legacy mode)
			 */
			m = sysInfo->pllFbkDiv * sysInfo->pllFwdDivB * sysInfo->pllPlbDiv;
		}

		sysInfo->freqVCOMhz = (1000000 * m) / sysClkPeriodPs;
		sysInfo->freqProcessor = (sysInfo->freqVCOMhz * 1000000) / sysInfo->pllFwdDiv;
		sysInfo->freqPLB = (sysInfo->freqVCOMhz * 1000000) /
			(sysInfo->pllFwdDivB * sysInfo->pllPlbDiv);
	} else {
		/*
		 * Check pllFwdDiv to see if running in bypass mode where the CPU speed
		 * is equal to the 405GP SYS_CLK_FREQ. If not in bypass mode, check VCO
		 * to make sure it is within the proper range.
		 *    spec:    VCO = SYS_CLOCK x FBKDIV x PLBDIV x FWDDIV
		 * Note freqVCO is calculated in Mhz to avoid errors introduced by rounding.
		 */
		if (sysInfo->pllFwdDiv == 1) {
			sysInfo->freqProcessor = CONFIG_SYS_CLK_FREQ * 1000000;
			sysInfo->freqPLB = sysInfo->freqProcessor / sysInfo->pllPlbDiv;
		} else {
			sysInfo->freqVCOMhz = ( 1000000 *
						sysInfo->pllFwdDiv *
						sysInfo->pllFbkDiv *
						sysInfo->pllPlbDiv
				) / sysClkPeriodPs;
			if (sysInfo->freqVCOMhz >= VCO_MIN
			    && sysInfo->freqVCOMhz <= VCO_MAX) {
				sysInfo->freqPLB = (ONE_BILLION /
						    ((sysClkPeriodPs * 10) /
						     sysInfo->pllFbkDiv)) * 10000;
				sysInfo->freqProcessor = sysInfo->freqPLB * sysInfo->pllPlbDiv;
			}
		}
	}
}

unsigned long get_timer_freq(void)
{
	struct ppc4xx_sys_info sys_info;

	get_sys_info(&sys_info);
	return sys_info.freqProcessor;
}

unsigned long get_pci_bus_freq(void)
{
	struct ppc4xx_sys_info sys_info;

	get_sys_info(&sys_info);
	return sys_info.freqPLB;
}
