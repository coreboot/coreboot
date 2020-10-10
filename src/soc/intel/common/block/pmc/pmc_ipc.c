/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/pmc_ipc.h>
#include <soc/pci_devs.h>
#include <stdint.h>
#include <timer.h>

/*
 * WBUF register block offset 0x80..0x8f there are 4 consecutive
 * 32 bit registers
 */
#define IPC_WBUF0	0x80

/*
 * RBUF registers block offset 0x90..0x9f there are 4 consecutive
 * 32 bit registers
 */
#define IPC_RBUF0	0x90

/*
 * From Intel 500 Series PCH EDS vol2 s4.4
 */
#define PMC_IPC_CMD_OFFSET		0x0
#define PMC_IPC_STS_OFFSET		0x4
#define PMC_IPC_STS_BUSY		BIT(0)
#define PMC_IPC_STS_ERR			BIT(1)
#define PMC_IPC_ERR_CODE_SHIFT		16
#define PMC_IPC_ERR_CODE_MASK		0xff

#define PMC_IPC_XFER_TIMEOUT_MS		(1 * MSECS_PER_SEC) /* max 1s */
#define IS_IPC_STS_BUSY(status)		((status) & PMC_IPC_STS_BUSY)
#define IPC_STS_HAS_ERROR(status)	((status) & PMC_IPC_STS_ERR)
#define IPC_STS_ERROR_CODE(sts)		(((sts) >> PMC_IPC_ERR_CODE_SHIFT & \
					PMC_IPC_ERR_CODE_MASK))

static void *pmc_reg(unsigned int pmc_reg_offset)
{
	const uintptr_t pmcbase = soc_read_pmc_base();
	return (void *)(pmcbase + pmc_reg_offset);
}

static const void *pmc_rbuf(unsigned int ix)
{
	return pmc_reg(IPC_RBUF0 + ix * sizeof(uint32_t));
}

static void *pmc_wbuf(unsigned int ix)
{
	return pmc_reg(IPC_WBUF0 + ix * sizeof(uint32_t));
}

static int check_ipc_sts(void)
{
	struct stopwatch sw;
	uint32_t ipc_sts;

	stopwatch_init_msecs_expire(&sw, PMC_IPC_XFER_TIMEOUT_MS);
	do {
		ipc_sts = read32(pmc_reg(PMC_IPC_STS_OFFSET));
		if (!(IS_IPC_STS_BUSY(ipc_sts))) {
			if (IPC_STS_HAS_ERROR(ipc_sts)) {
				printk(BIOS_ERR, "IPC_STS.error_code 0x%x\n",
				       IPC_STS_ERROR_CODE(ipc_sts));
				return PMC_IPC_ERROR;
			}
			return PMC_IPC_SUCCESS;
		}
		udelay(50);

	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "PMC IPC timeout after %u ms\n", PMC_IPC_XFER_TIMEOUT_MS);
	return PMC_IPC_TIMEOUT;
}

enum cb_err pmc_send_ipc_cmd(uint32_t cmd, const struct pmc_ipc_buffer *wbuf,
			     struct pmc_ipc_buffer *rbuf)
{
	for (int i = 0; i < PMC_IPC_BUF_COUNT; ++i)
		write32(pmc_wbuf(i), wbuf->buf[i]);

	write32(pmc_reg(PMC_IPC_CMD_OFFSET), cmd);

	if (check_ipc_sts()) {
		printk(BIOS_ERR, "PMC IPC command 0x%x failed\n", cmd);
		return CB_ERR;
	}

	for (int i = 0; i < PMC_IPC_BUF_COUNT; ++i)
		rbuf->buf[i] = read32(pmc_rbuf(i));

	return CB_SUCCESS;
}

void pmc_ipc_acpi_fill_ssdt(void)
{
	const struct fieldlist ipcs_fields[] = {
		FIELDLIST_OFFSET(PMC_IPC_CMD_OFFSET),	/* Command */
		FIELDLIST_NAMESTR("ICMD", 32),		/* Command Register */
		FIELDLIST_OFFSET(PMC_IPC_STS_OFFSET),	/* Status */
		FIELDLIST_NAMESTR("IBSY", 1),		/* Status Busy */
		FIELDLIST_NAMESTR("IERR", 1),		/* Status Error */
		FIELDLIST_RESERVED(14),
		FIELDLIST_NAMESTR("IERC", 8),		/* Status Error Code */
		FIELDLIST_OFFSET(IPC_WBUF0),		/* Write Buffer */
		FIELDLIST_NAMESTR("IWB0", 32),		/* Write Buffer 0 */
		FIELDLIST_NAMESTR("IWB1", 32),		/* Write Buffer 1 */
		FIELDLIST_NAMESTR("IWB2", 32),		/* Write Buffer 2 */
		FIELDLIST_NAMESTR("IWB3", 32),		/* Write Buffer 3 */
		FIELDLIST_OFFSET(IPC_RBUF0),		/* Read Buffer */
		FIELDLIST_NAMESTR("IRB0", 32),		/* Read Buffer 0 */
		FIELDLIST_NAMESTR("IRB1", 32),		/* Read Buffer 1 */
		FIELDLIST_NAMESTR("IRB2", 32),		/* Read Buffer 2 */
		FIELDLIST_NAMESTR("IRB3", 32),		/* Read Buffer 3 */
	};
	const struct opregion ipcs_opregion = OPREGION("IPCM", SYSTEMMEMORY,
						       soc_read_pmc_base(), 0xff);
	int i;

	/* Package with return value and read buffer. */
	acpigen_write_name("RVAL");
	acpigen_write_package(5);
	for (i = 0; i < 5; ++i)
		acpigen_write_integer(0);
	acpigen_pop_len();

	acpigen_write_method_serialized("IPCS", 7);

	acpigen_write_opregion(&ipcs_opregion);
	acpigen_write_field("IPCM", ipcs_fields, ARRAY_SIZE(ipcs_fields),
			    FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/* Fill write buffer data registers. */
	acpigen_write_store_op_to_namestr(ARG3_OP, "IWB0");
	acpigen_write_store_op_to_namestr(ARG4_OP, "IWB1");
	acpigen_write_store_op_to_namestr(ARG5_OP, "IWB2");
	acpigen_write_store_op_to_namestr(ARG6_OP, "IWB3");

	/* Program the command register with command and size of write data. */
	acpigen_write_store_int_to_op(0, LOCAL0_OP);

	/* Local0 += (Arg0 << PMC_IPC_CMD_COMMAND_SHIFT) */
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_shiftleft_op_int(ARG0_OP, PMC_IPC_CMD_COMMAND_SHIFT);
	acpigen_emit_byte(LOCAL0_OP);

	/* Local0 += (Arg1 << PMC_IPC_CMD_SUB_COMMAND_SHIFT) */
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_shiftleft_op_int(ARG1_OP, PMC_IPC_CMD_SUB_COMMAND_SHIFT);
	acpigen_emit_byte(LOCAL0_OP);

	/* Local1 = PMC_IPC_CMD_NO_MSI */
	acpigen_write_store_int_to_op(PMC_IPC_CMD_NO_MSI, LOCAL1_OP);
	/* Local0 += (Local1 << PMC_IPC_CMD_MSI_SHIFT) */
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_shiftleft_op_int(LOCAL1_OP, PMC_IPC_CMD_MSI_SHIFT);
	acpigen_emit_byte(LOCAL0_OP);

	/* Local0 += (Arg1 << PMC_IPC_CMD_SIZE_SHIFT) */
	acpigen_emit_byte(ADD_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_shiftleft_op_int(ARG2_OP, PMC_IPC_CMD_SIZE_SHIFT);
	acpigen_emit_byte(LOCAL0_OP);

	/* Start mailbox command with one 32bit write. */
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "ICMD");

	/* Read status register to get busy/error status. */
	acpigen_write_store_int_to_op(PMC_IPC_XFER_TIMEOUT_MS, LOCAL1_OP);

	/* While (Local1 > 0) */
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_emit_byte(ZERO_OP);

	/* If (IBSY == 0) { Return (SUCCESS) } */
	acpigen_write_if_lequal_namestr_int("IBSY", 0);
	acpigen_set_package_element_int("RVAL", 0, PMC_IPC_SUCCESS);
	acpigen_set_package_element_namestr("RVAL", 1, "IRB0");
	acpigen_set_package_element_namestr("RVAL", 2, "IRB1");
	acpigen_set_package_element_namestr("RVAL", 3, "IRB2");
	acpigen_set_package_element_namestr("RVAL", 4, "IRB3");
	acpigen_write_return_namestr("RVAL");
	acpigen_pop_len();

	/* If (IERR == 1) { Return (ERROR) } */
	acpigen_write_if_lequal_namestr_int("IERR", 1);
	acpigen_write_debug_string("IPCS ERROR");
	acpigen_write_debug_namestr("IERC");
	acpigen_set_package_element_int("RVAL", 0, PMC_IPC_ERROR);
	acpigen_write_return_namestr("RVAL");
	acpigen_pop_len();

	/* Sleep (1) */
	acpigen_write_sleep(1);
	/* Decrement (Local1) */
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_pop_len(); /* While */

	/* Return (TIMEOUT) */
	acpigen_write_debug_string("IPCS TIMEOUT");
	acpigen_set_package_element_int("RVAL", 0, PMC_IPC_TIMEOUT);
	acpigen_write_return_namestr("RVAL");

	acpigen_pop_len(); /* Method */
}

void pmc_ipc_acpi_set_pci_clock(unsigned int pcie_rp, unsigned int clock_pin, bool enable)
{
	const uint32_t data[] = {
		1 << clock_pin,			/* Clock pin to be modified */
		(enable ? 1 : 0) << clock_pin,	/* Clock pin to set */
		1 << pcie_rp,			/* PCIe root port to be modified */
		(enable ? 1 : 0) << pcie_rp,	/* PCIe root port to set */
	};
	const char *method = acpi_device_path_join(pcidev_path_on_root(PCH_DEVFN_PMC), "IPCS");

	if (!method) {
		printk(BIOS_ERR, "%s: Unable to find PMC device IPCS method\n", __func__);
		return;
	}

	/*
	 * The PMC IPC mailbox method takes 7 arguments:
	 *  IPCS (COMMAND, SUB_ID, SIZE, DATA0, DATA1, DATA2, DATA3)
	 */
	acpigen_emit_namestring(method);
	acpigen_write_integer(PMC_IPC_CMD_ID_SET_PCIE_CLOCK);
	acpigen_write_integer(0);
	acpigen_write_integer(sizeof(data));
	acpigen_write_dword(data[0]);
	acpigen_write_dword(data[1]);
	acpigen_write_dword(data[2]);
	acpigen_write_dword(data[3]);
}
