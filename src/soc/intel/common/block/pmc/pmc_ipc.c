/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/pmc_ipc.h>
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
				return -1;
			}
			return 0;
		}
		udelay(50);

	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "PMC IPC timeout after %u ms\n", PMC_IPC_XFER_TIMEOUT_MS);
	return -1;
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
