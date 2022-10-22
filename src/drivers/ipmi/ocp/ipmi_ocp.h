/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __IPMI_OCP_H
#define __IPMI_OCP_H

#include <commonlib/bsd/cb_err.h>
#include <device/pci_type.h>

#define IPMI_NETFN_OEM				0x30
#define  IPMI_OEM_SET_PPIN			0x77
#define  IPMI_BMC_SET_POST_START		0x73
#define  IPMI_OEM_SET_BIOS_BOOT_ORDER		0x52
#define  IPMI_OEM_GET_BIOS_BOOT_ORDER		0x53
#define  IPMI_OEM_GET_BOARD_ID			0x37

#define CMOS_BIT  (1 << 1)
#define VALID_BIT (1 << 7)
#define CLEAR_CMOS_AND_VALID_BIT(x) ((x) &= ~(CMOS_BIT | VALID_BIT))
#define SET_CMOS_AND_VALID_BIT(x)   ((x) |= (CMOS_BIT | VALID_BIT))
#define IS_CMOS_AND_VALID_BIT(x)    ((x)&CMOS_BIT && (x)&VALID_BIT)

struct ppin_req {
	uint32_t cpu0_lo;
	uint32_t cpu0_hi;
	uint32_t cpu1_lo;
	uint32_t cpu1_hi;
} __packed;

struct boot_order {
	uint8_t boot_mode;
	uint8_t boot_dev0;
	uint8_t boot_dev1;
	uint8_t boot_dev2;
	uint8_t boot_dev3;
	uint8_t boot_dev4;
} __packed;

struct pci_dev_fn {
	u32 func:15;
	u32 dev:5;
	u32 bus:12;
};

struct ipmi_pci_dev_fn {
	uint16_t func:3;
	uint16_t dev:5;
	uint16_t bus:8;
};

struct ipmi_sel_pcie_dev_err {
	uint16_t record_id;
	uint8_t record_type;
	uint8_t general_info;
	uint32_t timestamp;
	uint16_t aux_loc;
	struct ipmi_pci_dev_fn bdf;
	uint16_t primary_err_count;
	uint8_t secondary_id;
	uint8_t primary_id;
} __packed;

struct iio_port_location {
	uint8_t socket:4;
	uint8_t sled:2;
	uint8_t rsvd:2;
};

struct ipmi_sel_iio_err {
	uint16_t record_id;
	uint8_t record_type;
	uint8_t general_info;
	uint32_t timestamp;
	struct iio_port_location loc;
	uint8_t iio_stack_num;
	uint8_t rsvd0;
	uint8_t rsvd1;
	uint8_t iio_err_id;
	uint8_t rsvd2;
	uint8_t rsvd3;
	uint8_t rsvd4;
} __packed;

enum fail_type {
	PCIE_DPC_EVNT = 0,
	PCIE_LER_EVNT = 1,
	PCIE_LRTRN_REC = 2,
	PCIE_CRC_RETRY = 3,
	PCIE_CRPT_DATA_CONTMT = 4,
	PCIE_ECRC_EVNT = 5,
};

struct ipmi_sel_pcie_dev_fail {
	uint16_t record_id;
	uint8_t record_type;
	uint8_t general_info;
	uint32_t timestamp;
	enum fail_type type;
	uint8_t rsvd0;
	uint16_t failure_details1; /* if DPC, DPC sts reg of root port */
	uint16_t failure_details2; /* if DPC, source ID of root port */
	uint8_t rsvd1;
	uint8_t rsvd2;
} __packed;

struct ipmi_config_rsp {
	uint8_t board_sku_id;
	uint8_t board_rev_id;
	uint8_t slot_id;
	uint8_t slot_config_id;
} __packed;

#define SEL_RECORD_ID			0x01
#define SEL_PCIE_DEV_ERR		0x20
#define SEL_PCIE_IIO_ERR		0x23
#define SEL_PCIE_DEV_FAIL_ID		0x29

/* PCIE Unified Messages */

/* PCIE CE */
#define RECEIVER_ERROR			0x00
#define BAD_TLP				0x01
#define BAD_DLLP			0x02
#define REPLAY_TIME_OUT			0x03
#define REPLAY_NUMBER_ROLLOVER		0x04
#define ADVISORY_NONFATAL_ERROR_STATUS	0x05
#define CORRECTED_INTERNAL_ERROR_STATUS	0x06
#define HEADER_LOG_OVERFLOW_STATUS	0x07

/* PCIE UCE */
#define PCI_EXPRESS_DATA_LINK_PROTOCOL_ERROR						0x20
#define SURPRISE_DOWN_ERROR								0x21
#define RECEIVED_PCI_EXPRESS_POISONED_TLP						0x22
#define PCI_EXPRESS_FLOW_CONTROL_PROTOCOL_ERROR						0x23
#define COMPLETION_TIMEOUT_ON_NP_TRANSACTIONS_OUTSTANDING_ON_PCI_EXPRESS_DMI		0x24
#define RECEIVED_A_REQUEST_FROM_A_DOWNSTREAM_COMPONENT_THAT_IS_TO_BE_COMPLETER_ABORTED	0x25
#define RECEIVED_PCI_EXPRESS_UNEXPECTED_COMPLETION					0x26
#define PCI_EXPRESS_RECEIVER_OVERFLOW							0x27
#define PCI_EXPRESS_MALFORMED_TLP							0x28
#define ECRC_ERROR_STATUS								0x29
#define RECEIVED_A_REQUEST_FROM_A_DOWNSTREAM_COMPONENT_THAT_IS_UNSUPPORTED		0x2A
#define ACS_VIOLATION									0x2B
#define UNCORRECTABLE_INTERNAL_ERROR_STATUS						0x2C
#define MC_BLOCKED_TLP									0x2D
#define ATOMICOP_EGRESS_BLOCKED_STATUS							0x2E
#define TLP_PREFIX_BLOCKED_ERROR_STATUS							0x2F
#define POISONED_TLP_EGRESS_BLOCKED							0x30

/* Root error status (from PCIE spec) */
#define RECEIVED_ERR_COR_MESSAGE_FROM_DOWNSTREAM_DEVICE					0x50
#define RECEIVED_ERR_NONFATAL_MESSAGE_FROM_DOWNSTREAM_DEVICE				0x51
#define RECEIVED_ERR_FATAL_MESSAGE_FROM_DOWNSTREAM_DEVICE				0x52

/* DPC Trigger Reason */
#define DPC_WAS_TRIGGERED_DUE_TO_AN_UNMASKED_UNCORRECTABLE_ERROR			0x53
#define DPC_WAS_TRIGGERED_DUE_TO_RECEIVING_AN_ERR_NONFATAL				0x54
#define DPC_WAS_TRIGGERED_DUE_TO_RECEIVING_AN_ERR_FATAL					0x55
#define DPC_WAS_TRIGGERED_DUE_TO_RP_PIO_ERROR						0x56
#define DPC_WAS_TRIGGERED_DUE_TO_THE_DPC_SOFTWARE_TRIGGER_BIT				0x57

#define OUTBOUND_SWITCH_FIFO_DATA_PARITY_ERROR_DETECTED					0x80
#define SENT_A_PCI_EXPRESS_COMPLETER_ABORT						0x81
#define SENT_A_PCI_EXPRESS_UNSUPPORTED_REQUEST						0x82
#define RECEIVED_COMPLETER_ABORT							0x83
#define RECEIVED_UNSUPPORTED_REQUEST_COMPLETION_STATUS_FROM_DOWNSTREAM_DEVICE		0x84
#define RECEIVED_MSI_WRITES_GREATER_THAN_A_DWORD					0x85
#define OUTBOUND_POISONED_DATA								0x86
#define PERR_NON_AER									0xA0
#define SERR_NON_AER									0xA1

enum cb_err ipmi_set_post_start(const int port);
enum cb_err ipmi_set_cmos_clear(void);

void ipmi_send_to_bmc(unsigned char *data, size_t size);
void ipmi_send_sel_pcie_dev_err(pci_devfn_t bdf, uint16_t prmry_cnt, uint8_t sec_id,
	uint8_t prmry_id);
void ipmi_send_sel_pcie_dev_fail(uint16_t sts_reg, uint16_t src_id, enum fail_type code);
void ipmi_send_sel_iio_err(uint8_t iio_stack_num, uint8_t err_id);

enum cb_err ipmi_get_board_config(const int port, struct ipmi_config_rsp *config);
uint8_t get_blade_id(void);
#endif
