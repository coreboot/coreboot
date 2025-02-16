/* SPDX-License-Identifier: GPL-2.0-only */

/* ACPI - create the Fixed ACPI Description Tables (FADT) */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/acp.h>
#include <amdblocks/acpi.h>
#include <amdblocks/cppc.h>
#include <amdblocks/cpu.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/ioapic.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/amd/cpuid.h>
#include <device/device.h>
#include <soc/iomap.h>
#include <static.h>
#include <types.h>
#include "chip.h"

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const struct soc_amd_cezanne_config *cfg = config_of_soc();

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", ACPI_IO_BASE);

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = cfg->common_config.fadt_boot_arch; /* legacy free default */
	fadt->flags |=	ACPI_FADT_WBINVD | /* See table 5-34 ACPI 6.3 spec */
			ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_32BIT_TIMER |
			ACPI_FADT_PCI_EXPRESS_WAKE |
			ACPI_FADT_PLATFORM_CLOCK |
			ACPI_FADT_S4_RTC_VALID |
			ACPI_FADT_REMOTE_POWER_ON;
	if (cfg->s0ix_enable)
		fadt->flags |= ACPI_FADT_LOW_PWR_IDLE_S0;

	fadt->flags |= cfg->common_config.fadt_flags; /* additional board-specific flags */
}

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp)
{
	/* IVRS */
	current = acpi_add_ivrs_table(current, rsdp);

	if (CONFIG(PLATFORM_USES_FSP2_0))
		current = acpi_add_fsp_tables(current, rsdp);

	return current;
}

const acpi_cstate_t cstate_cfg_table[] = {
	[0] = {
		.ctype = 1,
		.latency = 1,
		.power = 0,
	},
	[1] = {
		.ctype = 2,
		.latency = 0x12,
		.power = 0,
	},
	[2] = {
		.ctype = 3,
		.latency = 350,
		.power = 0,
	},
};

const acpi_cstate_t *get_cstate_config_data(size_t *size)
{
	*size = ARRAY_SIZE(cstate_cfg_table);
	return cstate_cfg_table;
}

static void acp_soc_write_smn_access_methods(void)
{
	acpigen_write_method_serialized("SMNR", 1);
	acpigen_write_store_op_to_namestr(ARG0_OP, "\\_SB.PCI0.GNB.SMNA");
	acpigen_write_return_namestr("\\_SB.PCI0.GNB.SMND");
	acpigen_write_method_end();

	acpigen_write_method_serialized("SMNW", 2);
	acpigen_write_store_op_to_namestr(ARG0_OP, "\\_SB.PCI0.GNB.SMNA");
	acpigen_write_store_op_to_namestr(ARG1_OP, "\\_SB.PCI0.GNB.SMND");
	acpigen_write_method_end();
}

/* write value of ACPI variable into SMN register */
static void acp_soc_write_smn_register_write(uint8_t op, uint32_t smn_address)
{
	acpigen_emit_namestring("^SMNW");
	acpigen_write_integer(smn_address);
	acpigen_emit_byte(op);
}

/* read SMN register and store value into ACPI variable */
static void acp_soc_write_smn_register_read(uint32_t smn_address, uint8_t op)
{
	acpigen_write_store();
	acpigen_emit_namestring("^SMNR");
	acpigen_write_integer(smn_address);
	acpigen_emit_byte(op);
}

static void acp_soc_write_mailbox_access(void)
{
	acp_soc_write_smn_register_write(ARG0_OP, 0x00058A74);
	acp_soc_write_smn_register_write(ARG1_OP, 0x00058A54);
	acp_soc_write_smn_register_write(ARG2_OP, 0x00058A14);
	acp_soc_write_smn_register_read(0x00058A74, LOCAL0_OP);

	/* While (LEqual (Local0, 0)) */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_byte(ZERO_OP);

	acp_soc_write_smn_register_read(0x00058A74, LOCAL0_OP);

	acpigen_pop_len(); /* While */

	acp_soc_write_smn_register_read(0x00058A54, LOCAL1_OP);

	acpigen_write_return_op(LOCAL1_OP);
}

static void acp_soc_write_psp_mbox_buffer_field(void)
{
	acpigen_write_name("MBOX");
	acpigen_write_byte_buffer(NULL, 4);
	acpigen_write_create_buffer_word_field("MBOX", 0, "STAT");
	acpigen_write_create_buffer_byte_field("MBOX", 2, "CMDI");
	acpigen_write_create_buffer_bit_field("MBOX", 31, "REDY");
}

static void acp_soc_write_psp_read_mbox_from_hw(void)
{
	acp_soc_write_smn_register_read(0x03810570, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "MBOX");
}

static void acp_soc_write_psp_write_mbox_to_hw(void)
{
	acpigen_write_store_namestr_to_op("MBOX", LOCAL0_OP);
	acp_soc_write_smn_register_write(LOCAL0_OP, 0x03810570);
}

static void acp_soc_write_psp_mailbox_access(void)
{

	acp_soc_write_psp_mbox_buffer_field();

	acp_soc_write_psp_read_mbox_from_hw();

	/* While (LOr (LNotEqual (REDY, 0x1), LNotEqual (CMDI, 0x00))) */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LOR_OP);
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("REDY");
	acpigen_write_integer(1);
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("CMDI");
	acpigen_write_integer(0);

	acp_soc_write_psp_read_mbox_from_hw();

	acpigen_pop_len(); /* While */

	acpigen_write_store_int_to_op(0, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "MBOX");
	acpigen_write_store_int_to_namestr(0x33, "CMDI");

	acp_soc_write_psp_write_mbox_to_hw();

	acpigen_write_sleep(1);

	acp_soc_write_psp_read_mbox_from_hw();

	/* While (LNotEqual (CMDI, 0x00)) */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("CMDI");
	acpigen_write_integer(0);

	acp_soc_write_psp_read_mbox_from_hw();

	acpigen_pop_len(); /* While */

	acpigen_write_return_op(LOCAL0_OP);
}

static void acp_soc_write_msg0_method(void)
{
	acpigen_write_method_serialized("MSG0", 3);

	/* If (ARG2 != 0x9) */
	acpigen_write_if_lnotequal_op_int(ARG2_OP, 0x9);

	acp_soc_write_mailbox_access();

	acpigen_write_else();

	acp_soc_write_psp_mailbox_access();

	acpigen_write_if_end();

	acpigen_write_method_end(); /* MSG0 */
}

void acp_soc_write_ssdt_entry(const struct device *dev)
{
	/*
	 * SMN and mailbox interface using the SMN OperationRegion on the host bridge
	 *
	 * Provide both SMN read/write methods for direct SMN register access and the MSG0
	 * method which is used by some ACP drivers to access two different mailbox interfaces
	 * in the hardware. One mailbox interface is used to configure the ACP's clock source,
	 * the other one is used to notify the PSP that the DSP firmware has been loaded, so
	 * that the PSP can validate the firmware and set the qualifier bit to enable running
	 * it.
	 *
	 * Scope (\_SB.PCI0.GP41.ACPD)
	 * {
	 *	Method (SMNR, 1, Serialized)
	 *	{
	 *		Store (Arg0, \_SB.PCI0.GNB.SMNA)
	 *		Return (\_SB.PCI0.GNB.SMND)
	 *	}
	 *
	 *	Method (SMNW, 2, Serialized)
	 *	{
	 *		Store (Arg0, \_SB.PCI0.GNB.SMNA)
	 *		Store (Arg1, \_SB.PCI0.GNB.SMND)
	 *	}
	 *
	 *	Method (MSG0, 3, Serialized)
	 *	{
	 *		If (LNotEqual (Arg2, 0x09))
	 *		{
	 *			^SMNW (0x00058A74, Arg0)
	 *			^SMNW (0x00058A54, Arg1)
	 *			^SMNW (0x00058A14, Arg2)
	 *			Store (^SMNR (0x00058A74), Local0)
	 *			While (LEqual (Local0, Zero))
	 *			{
	 *				Store (^SMNR (0x00058A74), Local0)
	 *			}
	 *			Store (^SMNR (0x00058A54), Local1)
	 *			Return (Local1)
	 *		}
	 *		Else
	 *		{
	 *			Name (MBOX, Buffer (0x04){})
	 *			CreateWordField (MBOX, Zero, STAT)
	 *			CreateByteField (MBOX, 0x02, CMDI)
	 *			CreateBitField (MBOX, 0x1F, REDY)
	 *			Store (^SMNR (0x03810570), Local0)
	 *			Store (Local0, MBOX)
	 *			While (LOr (LNotEqual (REDY, One), LNotEqual (CMDI, Zero)))
	 *			{
	 *				Store (^SMNR (0x03810570), Local0)
	 *				Store (Local0, MBOX)
	 *			}
	 *			Store (Zero, Local0)
	 *			Store (Local0, MBOX)
	 *			Store (0x33, CMDI)
	 *			Store (MBOX, Local0)
	 *			^SMNW (0x03810570, Local0)
	 *			Sleep (One)
	 *			Store (^SMNR (0x03810570), Local0)
	 *			Store (Local0, MBOX)
	 *			While (LNotEqual (CMDI, Zero))
	 *			{
	 *				Store (^SMNR (0x03810570), Local0)
	 *				Store (Local0, MBOX)
	 *			}
	 *			Return (Local0)
	 *		}
	 *	}
	 * }
	 */

	acpigen_write_scope(acpi_device_path(dev));

	acp_soc_write_smn_access_methods();

	acp_soc_write_msg0_method();

	acpigen_write_scope_end();
}
