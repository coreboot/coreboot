/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_INTEL_DTBT_H_
#define _DRIVERS_INTEL_DTBT_H_

/* Alpine Ridge device IDs */
#define AR_2C_NHI	0x1575
#define AR_2C_BRG	0x1576
#define AR_2C_USB	0x15B5
#define AR_4C_NHI	0x1577
#define AR_4C_BRG	0x1578
#define AR_4C_USB	0x15B6
#define AR_LP_NHI	0x15BF
#define AR_LP_BRG	0x15C0
#define AR_LP_USB	0x15C1
#define AR_4C_C0_NHI	0x15D2
#define AR_4C_C0_BRG	0x15D3
#define AR_4C_C0_USB	0x15D4
#define AR_2C_C0_NHI	0x15D9
#define AR_2C_C0_BRG	0x15DA
#define AR_2C_C0_USB	0x15DB

/* Titan Ridge device IDs */
#define TR_2C_BRG	0x15E7
#define TR_2C_NHI	0x15E8
#define TR_2C_USB	0x15E9
#define TR_4C_BRG	0x15EA
#define TR_4C_NHI	0x15EB
#define TR_4C_USB	0x15EC
#define TR_DD_BRG	0x15EF
#define TR_DD_USB	0x15F0

/* Maple Ridge device IDs */
#define MR_2C_BRG	0x1133
#define MR_2C_NHI	0x1134
#define MR_2C_USB	0x1135
#define MR_4C_BRG	0x1136
#define MR_4C_NHI	0x1137
#define MR_4C_USB	0x1138

/* Security Levels */
#define SEC_LEVEL_NONE			0
#define SEC_LEVEL_USER			1
#define SEC_LEVEL_AUTH			2
#define SEC_LEVEL_DP_ONLY		3

#define PCIE2TBT			0x54C
#define PCIE2TBT_VALID			BIT(0)
#define PCIE2TBT_GO2SX			2
#define PCIE2TBT_GO2SX_NO_WAKE		3
#define PCIE2TBT_SX_EXIT_TBT_CONNECTED	4
#define PCIE2TBT_OS_UP			6
#define PCIE2TBT_SET_SECURITY_LEVEL	8
#define PCIE2TBT_GET_SECURITY_LEVEL	9
#define PCIE2TBT_BOOT_ON		24
#define PCIE2TBT_USB_ON			25
#define PCIE2TBT_GET_ENUMERATION_METHOD	26
#define PCIE2TBT_SET_ENUMERATION_METHOD	27
#define PCIE2TBT_POWER_CYCLE		28
#define PCIE2TBT_SX_START		29
#define PCIE2TBT_ACL_BOOT		30
#define PCIE2TBT_CONNECT_TOPOLOGY	31

#define TBT2PCIE			0x548
#define TBT2PCIE_DONE			BIT(0)
#define TBT2PCIE_ERROR_MASK		(0xF << 12)	/* bits 15:12, Table 38 */
#define TBT2PCIE_ERROR_SUCCESS		0
#define TBT2PCIE_ERROR_GENERAL		1
#define TBT2PCIE_ERROR_ILLEGAL_DATA	2
#define TBT2PCIE_ERROR_TIMEOUT		3

// Timeout for mailbox commands unless otherwise specified.
#define MBOX_TIMEOUT_MS			5000

// Timeout for controller to ack GO2SX/GO2SX_NO_WAKE mailbox command.
#define GO2SX_TIMEOUT_MS		600

#endif /* _DRIVERS_INTEL_DTBT_H_ */
