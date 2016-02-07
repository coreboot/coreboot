/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <arch/early_variables.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/pci_def.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <soc/intel/common/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>

void car_soc_pre_console_init(void)
{
	if (IS_ENABLED(CONFIG_ENABLE_BUILTIN_HSUART1))
		set_base_address_and_enable_uart(0, HSUART1_DEV, HSUART1_FUNC,
			UART_BASE_ADDRESS);
}

void car_soc_post_console_init(void)
{
	report_platform_info();
};

static struct chipset_power_state power_state CAR_GLOBAL;

struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	ps->prev_sleep_state = 0;
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);
	return ps;
}

void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MCR,
		(opcode << QNC_MCR_OP_OFFSET)
		| ((uint32_t)port << QNC_MCR_PORT_OFFSET)
		| ((reg_address & QNC_MCR_MASK) << QNC_MCR_REG_OFFSET)
		| QNC_MCR_BYTE_ENABLES);
}

uint32_t mdr_read(void)
{
	return pci_read_config32(MC_BDF, QNC_ACCESS_PORT_MDR);
}

void mdr_write(uint32_t value)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MDR, value);
}

void mea_write(uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MEA, reg_address
		& QNC_MEA_MASK);
}

static uint32_t mtrr_index_to_host_bridge_register_offset(unsigned long index)
{
	uint32_t offset;

	/* Convert from MTRR index to host brigde offset (Datasheet 12.7.2) */
	if (index == MTRR_CAP_MSR)
		offset = QUARK_NC_HOST_BRIDGE_IA32_MTRR_CAP;
	else if (index == MTRR_DEF_TYPE_MSR)
		offset = QUARK_NC_HOST_BRIDGE_IA32_MTRR_DEF_TYPE;
	else if (index == MTRR_FIX_64K_00000)
		offset = QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000;
	else if ((index >= MTRR_FIX_16K_80000) && (index <= MTRR_FIX_16K_A0000))
		offset = ((index - MTRR_FIX_16K_80000) << 1)
			+ QUARK_NC_HOST_BRIDGE_MTRR_FIX16K_80000;
	else if ((index >= MTRR_FIX_4K_C0000) && (index <= MTRR_FIX_4K_F8000))
		offset = ((index - MTRR_FIX_4K_C0000) << 1)
			+ QUARK_NC_HOST_BRIDGE_IA32_MTRR_PHYSBASE0;
	else if ((index >= MTRR_PHYS_BASE(0)) && (index <= MTRR_PHYS_MASK(7)))
		offset = (index - MTRR_PHYS_BASE(0))
			+ QUARK_NC_HOST_BRIDGE_IA32_MTRR_PHYSBASE0;
	else {
		printk(BIOS_DEBUG, "index: 0x%08lx\n", index);
		die("Invalid MTRR index specified!\n");
	}
	return offset;
}

msr_t soc_mtrr_read(unsigned long index)
{
	uint32_t offset;
	union {
		uint64_t u64;
		msr_t msr;
	} value;

	/* Read the low 32-bits of the register */
	offset = mtrr_index_to_host_bridge_register_offset(index);
	mea_write(offset);
	mcr_write(QUARK_OPCODE_READ, QUARK_NC_HOST_BRIDGE_SB_PORT_ID, offset);
	value.u64 = mdr_read();

	/* For 64-bit registers, read the upper 32-bits */
	if ((offset >=  QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000)
		&& (offset <= QUARK_NC_HOST_BRIDGE_MTRR_FIX4K_F8000)) {
		offset += 1;
		mea_write(offset);
		mcr_write(QUARK_OPCODE_READ, QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
			offset);
		value.u64 |= mdr_read();
	}
	return value.msr;
}

void soc_mtrr_write(unsigned long index, msr_t msr)
{
	uint32_t offset;
	union {
		uint32_t u32[2];
		msr_t msr;
	} value;

	/* Write the low 32-bits of the register */
	value.msr = msr;
	offset = mtrr_index_to_host_bridge_register_offset(index);
	mea_write(offset);
	mdr_write(value.u32[0]);
	mcr_write(QUARK_OPCODE_WRITE, QUARK_NC_HOST_BRIDGE_SB_PORT_ID, offset);

	/* For 64-bit registers, write the upper 32-bits */
	if ((offset >=  QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000)
		&& (offset <= QUARK_NC_HOST_BRIDGE_MTRR_FIX4K_F8000)) {
		offset += 1;
		mea_write(offset);
		mdr_write(value.u32[1]);
		mcr_write(QUARK_OPCODE_WRITE, QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
			offset);
	}
}

asmlinkage void *soc_set_mtrrs(void *top_of_stack)
{
	union {
		uint32_t u32[2];
		uint64_t u64;
		msr_t msr;
	} data;
	uint32_t mtrr_count;
	uint32_t *mtrr_data;
	uint32_t mtrr_reg;

	/*
	 * The stack contents are initialized in src/soc/intel/common/stack.c
	 * to be the following:
	 *
	 *				*
	 *				*
	 *				*
	 *		   +36: MTRR mask 1 63:32
	 *		   +32: MTRR mask 1 31:0
	 *		   +28: MTRR base 1 63:32
	 *		   +24: MTRR base 1 31:0
	 *		   +20: MTRR mask 0 63:32
	 *		   +16: MTRR mask 0 31:0
	 *		   +12: MTRR base 0 63:32
	 *		    +8: MTRR base 0 31:0
	 *		    +4: Number of MTRRs to setup (described above)
	 * top_of_stack --> +0: Number of variable MTRRs to clear
	 *
	 * This routine:
	 *	* Clears all of the variable MTRRs
	 *	* Initializes the variable MTRRs with the data passed in
	 *	* Returns the new top of stack after removing all of the
	 *	  data passed in.
	 */

	/* Clear all of the variable MTRRs (base and mask). */
	mtrr_reg = MTRR_PHYS_BASE(0);
	mtrr_data = top_of_stack;
	mtrr_count = (*mtrr_data++) * 2;
	data.u64 = 0;
	while (mtrr_count-- > 0)
		soc_mtrr_write(mtrr_reg++, data.msr);

	/* Setup the specified variable MTRRs */
	mtrr_reg = MTRR_PHYS_BASE(0);
	mtrr_count = *mtrr_data++;
	while (mtrr_count-- > 0) {
		data.u32[0] = *mtrr_data++;
		data.u32[1] = *mtrr_data++;
		soc_mtrr_write(mtrr_reg++, data.msr); /* Base */
		data.u32[0] = *mtrr_data++;
		data.u32[1] = *mtrr_data++;
		soc_mtrr_write(mtrr_reg++, data.msr); /* Mask */
	}

	/* Remove setup_stack_and_mtrrs data and return the new top_of_stack */
	top_of_stack = mtrr_data;
	return top_of_stack;
}

asmlinkage void soc_enable_mtrrs(void)
{
	union {
		uint32_t u32[2];
		uint64_t u64;
		msr_t msr;
	} data;

	/* Enable MTRR. */
	data.msr = soc_mtrr_read(MTRR_DEF_TYPE_MSR);
	data.u32[0] |= MTRR_DEF_TYPE_EN;
	soc_mtrr_write(MTRR_DEF_TYPE_MSR, data.msr);
}
