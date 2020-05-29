/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PMIF_SW_H__
#define __PMIF_SW_H__

/* Read/write byte limitation, by project */
/* hw bytecnt indicate when we set 0, it can send 1 byte;
 * set 1, it can send 2 byte.
 */
#define PMIF_BYTECNT_MAX	1

/* macro for SWINF_FSM */
#define SWINF_FSM_IDLE		0x00
#define SWINF_FSM_REQ		0x02
#define SWINF_FSM_WFDLE		0x04
#define SWINF_FSM_WFVLDCLR	0x06
#define SWINF_INIT_DONE		0x01

#define FREQ_METER_ABIST_AD_OSC_CK	37
#define GET_SWINF_0_FSM(x)	(((x) >> 1) & 0x7)

struct pmif_mpu {
	unsigned int rgn_slvid;
	unsigned short rgn_s_addr;
	unsigned short rgn_e_addr;
	unsigned int rgn_domain_per;
};

enum {
	PMIF_READ_US        = 1000,
	PMIF_WAIT_IDLE_US   = 1000,
};

enum {
	FREQ_260MHZ = 260,
};

/* calibation tolerance rate, unit: 0.1% */
enum {
	CAL_TOL_RATE = 40,
	CAL_MAX_VAL = 0x7F,
};

extern int pmif_clk_init(void);
#endif /*__PMIF_SW_H__*/
