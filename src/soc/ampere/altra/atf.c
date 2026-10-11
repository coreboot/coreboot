/* SPDX-License-Identifier: GPL-2.0-only */

#include "atf.h"
#include <armv8/arch/smc.h>
#include <console/console.h>
#include <stdint.h>

#define MM_COMM_BUFFER 0x88300000

uint32_t atf_mm_version(void)
{
	return smc_call0(MM_VERSION);
}

uint32_t atf_mm_communicate(void)
{
	return smc_call2(MM_COMMUNICATE, 0, MM_COMM_BUFFER);
}

int atf_mm_request(const guid_t *guid, uint64_t dw[5])
{
	volatile struct mm_communicate *comm = (void *)MM_COMM_BUFFER;

	comm->guid = *guid;
	comm->msg_length = 5 * sizeof(uint64_t);
	comm->dw0 = dw[0];
	comm->dw1 = dw[1];
	comm->dw2 = dw[2];
	comm->dw3 = dw[3];
	comm->dw4 = dw[4];

	uint32_t ret = atf_mm_communicate();
	if (ret != MM_SUCCESS) {
		printk(BIOS_ERR, "MM_COMMUNICATE returned %d\n", (int32_t)ret);
		return -1;
	}

	dw[0] = comm->dw0;
	dw[1] = comm->dw1;
	dw[2] = comm->dw2;
	dw[3] = comm->dw3;
	dw[4] = comm->dw4;

	return 0;
}
