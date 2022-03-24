/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.7
 */

#include <device/mmio.h>
#include <soc/pmif.h>

#define SLEEP_PROT_CTRL 0x3F0

DEFINE_BITFIELD(SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(SCP_SLEEP_REQ_SEL, 10, 9)

void pmif_spmi_set_lp_mode(void)
{
	SET32_BITFIELDS((void *)(PMIF_BASE + SLEEP_PROT_CTRL),
			SPM_SLEEP_REQ_SEL, 0,
			SCP_SLEEP_REQ_SEL, 0);
}
