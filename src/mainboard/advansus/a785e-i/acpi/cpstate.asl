/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
			#include "cpstate.asl"
		}
		Processor(CPU1,1,0x0,0x0) {
			#include "cpstate.asl"
		}
		Processor(CPU2,2,0x0,0x0) {
			#include "cpstate.asl"
		}
		Processor(CPU3,3,0x0,0x0) {
			#include "cpstate.asl"
		}
	}
*/
	/* P-state support: The maximum number of P-states supported by the */
	/* CPUs we'll use is 6. */
	/* Get from AMI BIOS. */
	Name(_PSS, Package(){
		Package ()
		{
		    0x00000AF0,
		    0x0000BF81,
		    0x00000002,
		    0x00000002,
		    0x00000000,
		    0x00000000
		},

		Package ()
		{
		    0x00000578,
		    0x000076F2,
		    0x00000002,
		    0x00000002,
		    0x00000001,
		    0x00000001
		}
	})

	Name(_PCT, Package(){
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
	})

	Method(_PPC, 0){
		Return(0)
	}
