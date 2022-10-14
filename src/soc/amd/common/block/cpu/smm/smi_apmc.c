/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/psp.h>
#include <amdblocks/smm.h>
#include <arch/io.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <smmstore.h>
#include <types.h>

/* bits in smm_io_trap   */
#define SMM_IO_TRAP_PORT_OFFSET		16
#define SMM_IO_TRAP_PORT_ADDRESS_MASK	0xffff
#define SMM_IO_TRAP_RW			(1 << 0)
#define SMM_IO_TRAP_VALID		(1 << 1)

static inline u16 get_io_address(u32 info)
{
	return ((info >> SMM_IO_TRAP_PORT_OFFSET) &
		SMM_IO_TRAP_PORT_ADDRESS_MASK);
}

static void *find_save_state(int cmd)
{
	unsigned int core;
	amd64_smm_state_save_area_t *state;
	u32 smm_io_trap;
	u8 reg_al;

	/* Check all nodes looking for the one that issued the IO */
	for (core = 0; core < CONFIG_MAX_CPUS; core++) {
		state = smm_get_save_state(core);
		smm_io_trap = state->smm_io_trap_offset;
		/* Check for Valid IO Trap Word (bit1==1) */
		if (!(smm_io_trap & SMM_IO_TRAP_VALID))
			continue;
		/* Make sure it was a write (bit0==0) */
		if (smm_io_trap & SMM_IO_TRAP_RW)
			continue;
		/* Check for APMC IO port */
		if (pm_acpi_smi_cmd_port() != get_io_address(smm_io_trap))
			continue;
		/* Check AL against the requested command */
		reg_al = state->rax;
		if (reg_al == cmd)
			return state;
	}
	return NULL;
}

void handle_smi_gsmi(void)
{
	u8 sub_command;
	amd64_smm_state_save_area_t *io_smi;
	u32 reg_ebx;

	io_smi = find_save_state(APM_CNT_ELOG_GSMI);
	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (io_smi->rax >> 8) & 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = io_smi->rbx;

	/* drivers/elog/gsmi.c */
	io_smi->rax = gsmi_exec(sub_command, &reg_ebx);
}

void handle_smi_store(void)
{
	u8 sub_command;
	amd64_smm_state_save_area_t *io_smi;
	u32 reg_ebx;

	io_smi = find_save_state(APM_CNT_SMMSTORE);
	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (io_smi->rax >> 8) & 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = io_smi->rbx;

	/* drivers/smmstore/smi.c */
	io_smi->rax = smmstore_exec(sub_command, (void *)(uintptr_t)reg_ebx);
}

void fch_apmc_smi_handler(void)
{
	const uint8_t cmd = inb(pm_acpi_smi_cmd_port());

	switch (cmd) {
	case APM_CNT_ACPI_ENABLE:
		acpi_clear_pm_gpe_status();
		acpi_enable_sci();
		break;
	case APM_CNT_ACPI_DISABLE:
		acpi_disable_sci();
		break;
	case APM_CNT_ELOG_GSMI:
		if (CONFIG(ELOG_GSMI))
			handle_smi_gsmi();
	break;
	case APM_CNT_SMMSTORE:
		if (CONFIG(SMMSTORE))
			handle_smi_store();
	break;
	case APM_CNT_SMMINFO:
		psp_notify_smm();
		break;
	}

	mainboard_smi_apmc(cmd);
}
