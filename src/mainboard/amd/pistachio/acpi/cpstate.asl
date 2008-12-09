/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* This file defines the processor and performance state capability
 * for each core in the system.  It is included into the DSDT for each
 * core.  It assumes that each core of the system has the same performance
 * characteristics.
*/
/*
DefinitionBlock ("DSDT.AML","DSDT",0x01,"XXXXXX","XXXXXXXX",0x00010001)
	{
		Scope (\_PR) {
		Processor(CPU0,0,0x808,0x06) {
			Include ("cpstate.asl")
		}
		Processor(CPU1,1,0x0,0x0) {
			Include ("cpstate.asl")
		}
		Processor(CPU2,2,0x0,0x0) {
			Include ("cpstate.asl")
		}
		Processor(CPU3,3,0x0,0x0) {
			Include ("cpstate.asl")
		}
	}
*/
/*
 * 1. Get the CPUID to know what the version of the CPU is. (or see what powernow
 *     reports)
 * 2. Find how many P-states the CPU supports, power and frequecy in each
 *     P-states in 'Power and Thermal Data Sheet.' (PTDS)
 * 3. Go to BIOS and Kernel Developer's Guide (BKDG) and find Low FID Frequency
 *     Table & High FID Frequency Table. Find Fid for each frequency.
 * 4. In PTDS, got the Voltage for each P-state. In table VID Code Voltages of BKDG,
 *     find the VID for each Voltage.
 *  Is that clear?
 */

	/* P-state support: The maximum number of P-states supported by the */
	/* CPUs we'll use is 6. */
	Name(_PSS, Package(){
		/* The processor core clock PLL lock time is 2 us for AMD NPT Family 0Fh Processors.*/
		/* vst=100us*/
		/* 3<<31|2<<28|1<<27|2<<20|0<<18|5<<11|0x13<<6|0xD, 0x13<<6|0xD */
		/* 3<<31|2<<28|1<<27|2<<20|0<<18|5<<11|0x14<<6|0xC, 0x14<<6|0xC */
		/* 3<<31|2<<28|1<<27|2<<20|0<<18|5<<11|0x15<<6|0xA, 0x15<<6|0xA */
		/* 3<<31|2<<28|1<<27|2<<20|0<<18|5<<11|0x16<<6|0x8, 0x16<<6|0x8 */
		/* 3<<31|2<<28|1<<27|2<<20|0<<18|5<<11|0x1E<<6|0x0, 0x1E<<6|0x0 */
		/*
		* Package() {2100, 35000, 100, 7, 0xE8202CCD, 0x04CD},
		* Package() {2000, 30100, 100, 7, 0xE8202D0C, 0x050C},
		* Package() {1800, 26400, 100, 7, 0xE8202D4A, 0x054A},
		* Package() {1600, 23000, 100, 7, 0xE8202D88, 0x0588},
		* Package() { 800,  9400, 100, 7, 0xE8202F80, 0x0780},
		*/
		/*Use this tricky method to reserve 8 Pstates space*/
		Package() {0x1FFFFFFF, 0x2FFFFFFF, 0x3FFFFFFF, 0x4FFFFFFF, 0x5FFFFFFF, 0x6FFFFFFF},
		Package() {0x7FFFFFFF, 0x8FFFFFFF, 0x9FFFFFFF, 0xAFFFFFFF, 0xBFFFFFFF, 0xCFFFFFFF},
		Package() {0xDFFFFFFF, 0xEFFFFFFF, 0x1FFFFFFF, 0x2FFFFFFF, 0x3FFFFFFF, 0x4FFFFFFF},
		Package() {0x5FFFFFFF, 0x6FFFFFFF, 0x7FFFFFFF, 0x8FFFFFFF, 0x9FFFFFFF, 0xAFFFFFFF},
		Package() {0xBFFFFFFF, 0xCFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF, 0x1FFFFFFF, 0x2FFFFFFF},
		Package() {0x3FFFFFFF, 0x4FFFFFFF, 0x5FFFFFFF, 0x6FFFFFFF, 0x7FFFFFFF, 0x8FFFFFFF},
		Package() {0x9FFFFFFF, 0xAFFFFFFF, 0xBFFFFFFF, 0xCFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF},
		Package() {0x1FFFFFFF, 0x2FFFFFFF, 0x3FFFFFFF, 0x4FFFFFFF, 0x5FFFFFFF, 0x6FFFFFFF},
	})

	Name(_PCT, Package(){
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
	})

	Method(_PPC, 0){
		Return(0)
	}
