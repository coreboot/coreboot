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
#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>



/**
 * Create the Fixed ACPI Description Tables (FADT) for this board.
 The FADT defines various fixed hardware ACPI information vital to an ACPI-compatible
 OS, such as the base address for the following hardware registers blocks:
 PM1a_EVT_BLK, PM1b_EVT_BLK, PM1a_CNT_BLK, PM1b_CNT_BLK,
 PM2_CNT_BLK, PM_TMR_BLK, GPE0_BLK and GPE1_BLK.
 The FADT also has a pointer to the DSDT that contains the Differentiated Definition Block,
 which in turn provides variable information to an ACPI-compatible OS concerning the base
 system design.

 Not all blocks are necessary usualy only PM1a, PMTMR and GPE0 are used.
 */
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header=&(fadt->header);
	
	printk_debug("pm_base: 0x%04x\n", pm_base);
	
	memset((void *)fadt,0,sizeof(acpi_fadt_t));

	/* Prepare the header */
	memcpy(header->signature,"FACP",4);
	header->length = 244;
	header->revision = 1;
	memcpy(header->oem_id,OEM_ID,6);
	memcpy(header->oem_table_id,"LXBACPI ",8);
	memcpy(header->asl_compiler_id,ASLC,4);
	header->asl_compiler_revision=0;

	
	fadt->firmware_ctrl=(u32)facs;
	fadt->dsdt= (u32)dsdt;

	/*
	0:	unspecified
	1:	desktop
	2:	mobile
	3:	workstation
	4:	enterprise server
	*/
	fadt->preferred_pm_profile=0x01;

	/*
	System vector the SCI interrupt is wired to in 8259 mode. 
	On systems that do not contain the 8259, this field contains the Global
	System interrupt number of the SCI interrupt. OSPM is required to treat
	the ACPI SCI interrupt as a sharable, level, active low interrupt.
	SB600 BDG 4.1
	*/
	fadt->sci_int=4;

	/*
	System port address of the SMI Command Port. During ACPI OS initialization,
	OSPM can determine that the ACPI hardware registers are owned by SMI (by way
	of the SCI_EN bit), in which case the ACPI OS issues the ACPI_ENABLE command
	to the SMI_CMD port. The SCI_EN bit effectively tracks the ownership of the
	ACPI hardware registers. OSPM issues commands to the SMI_CMD port
	synchronously from the boot processor.
	This filed is reserved and must be zero on system that does not support
	System Management mode.
	*/
	fadt->smi_cmd = 0;

	/*Those two fields are reserved and must be zero on systems that do not
	support Legacy Mode.*/
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
	
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0x0;
	
}
