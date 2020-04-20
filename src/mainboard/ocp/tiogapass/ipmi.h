/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TIOGAPASS_IPMI_H
#define TIOGAPASS_IPMI_H
#include <types.h>

#define IPMI_NETFN_OEM 0x30
#define IPMI_OEM_SET_PPIN 0x77

/* PPIN for 2 CPU IPMI request */
struct ppin_req {
	uint32_t cpu0_lo;
	uint32_t cpu0_hi;
	uint32_t cpu1_lo;
	uint32_t cpu1_hi;
} __packed;
/* Send CPU0 and CPU1 PPIN to BMC */
void ipmi_set_ppin(struct ppin_req *req);
#endif
