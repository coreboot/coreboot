/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PMC_IPC_H
#define SOC_INTEL_COMMON_BLOCK_PMC_IPC_H

#include <types.h>

#define PMC_IPC_BUF_COUNT		4

#define PMC_IPC_CMD_COMMAND_SHIFT	0
#define PMC_IPC_CMD_COMMAND_MASK	0xff
#define PMC_IPC_CMD_MSI_SHIFT		8
#define PMC_IPC_CMD_MSI_MASK		0x01
#define PMC_IPC_CMD_SUB_COMMAND_SHIFT	12
#define PMC_IPC_CMD_SUB_COMMAND_MASK	0x0f
#define PMC_IPC_CMD_SIZE_SHIFT		16
#define PMC_IPC_CMD_SIZE_MASK		0xff

#define PMC_IPC_CMD_FIELD(name, val) \
	(((val) & PMC_IPC_CMD_##name##_MASK << PMC_IPC_CMD_##name##_SHIFT))

#define PMC_IPC_CMD_NO_MSI		0

/* IPC command to enable/disable PCIe SRCCLK */
#define PMC_IPC_CMD_ID_SET_PCIE_CLOCK	0xAC

/* IPC return values */
#define PMC_IPC_SUCCESS			0
#define PMC_IPC_ERROR			1
#define PMC_IPC_TIMEOUT			2

/*
 * Create the IPC CMD to send to PMC
 */
static inline uint32_t pmc_make_ipc_cmd(uint32_t cmd, uint32_t subcmd,
					uint32_t size)
{
	return PMC_IPC_CMD_FIELD(COMMAND, cmd) |
		PMC_IPC_CMD_FIELD(SUB_COMMAND, subcmd) |
		PMC_IPC_CMD_FIELD(MSI, PMC_IPC_CMD_NO_MSI) |
		PMC_IPC_CMD_FIELD(SIZE, size);
}

/*
 * Buffer for holding write and read buffers of IPC commands
 */
struct pmc_ipc_buffer {
	uint32_t buf[PMC_IPC_BUF_COUNT];
};

/*
 * Send PMC IPC command
 */
enum cb_err pmc_send_ipc_cmd(uint32_t cmd, const struct pmc_ipc_buffer *wbuf,
			     struct pmc_ipc_buffer *rbuf);

/*
 * Provides an ACPI method in the SSDT to read/write to the IPC mailbox which is
 * defined in the PMC device MMIO address space.
 *
 * One possible use of this method is to enable/disable the clock for a
 * particular PCIe root port at runtime when the device is in D3 state.
 *
 * The ACPI method takes 7 arguments:
 *  IPCW (COMMAND, SUB_ID, SIZE, DATA0, DATA1, DATA2, DATA3)
 *
 * And will return a package with 5 elements:
 *  0    = Return code
 *         PMC_IPC_SUCCESS
 *         PMC_IPC_ERROR
 *         PMC_IPC_TIMEOUT
 *  1..4 = Data read from IPC if return code is PMC_IPC_SUCCESS
 */
void pmc_ipc_acpi_fill_ssdt(void);

/*
 * Call the ACPI method to write to the IPC mailbox and enable/disable the
 * specified clock pin connected to the specified PCIe root port.
 */
void pmc_ipc_acpi_set_pci_clock(unsigned int pcie_rp, unsigned int clock_pin, bool enable);

#endif /* SOC_INTEL_COMMON_BLOCK_PMC_IPC_H */
