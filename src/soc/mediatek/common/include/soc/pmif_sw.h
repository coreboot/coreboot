/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_PMIF_SW_H__
#define __SOC_MEDIATEK_PMIF_SW_H__

/* macro for SWINF_FSM */
#define SWINF_FSM_IDLE		0x00
#define SWINF_FSM_REQ		0x02
#define SWINF_FSM_WFDLE		0x04
#define SWINF_FSM_WFVLDCLR	0x06
#define SWINF_INIT_DONE		0x01

#define GET_SWINF_0_FSM(x)	(((x) >> 1) & 0x7)

enum {
	PMIF_READ_US        = 1000,
	PMIF_WAIT_IDLE_US   = 1000,
};

u32 pmif_get_ulposc_freq_mhz(u32 cali_val);
int pmif_clk_init(void);
#endif /* __SOC_MEDIATEK_PMIF_SW_H__ */
