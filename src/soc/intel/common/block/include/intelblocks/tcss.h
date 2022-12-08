/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TCSS_H_
#define _TCSS_H_

#include <intelblocks/gpio.h>
#if !defined(__ACPI__)
#include <device/usbc_mux.h>

/* PMC IPC related offsets and commands */
#define PMC_IPC_USBC_CMD_ID		0xA7
#define PMC_IPC_USBC_SUBCMD_ID		0x0
#define PMC_IPC_CMD			0x0
#define PMC_IPC_TCSS_CONN_REQ_RES	0x0
#define PMC_IPC_TCSS_DISC_REQ_RES	0x1
#define PMC_IPC_TCSS_SAFE_MODE_REQ_RES	0x2
#define PMC_IPC_TCSS_ALTMODE_REQ_RES	0x3
#define PMC_IPC_TCSS_HPD_REQ_RES	0x4
#define PMC_IPC_CONN_REQ_SIZE		2
#define PMC_IPC_DISC_REQ_SIZE		2
#define PMC_IPC_ALT_REQ_SIZE		8
#define PMC_IPC_SAFE_REQ_SIZE		1
#define PMC_IPC_HPD_REQ_SIZE		2
#define PMC_IPC_DP_MODE			1

#define TCSS_CD_USAGE_SHIFT		0
#define TCSS_CD_USAGE_MASK		0x0f
#define TCSS_CD_USB3_SHIFT		4
#define TCSS_CD_USB3_MASK		0x0f
#define TCSS_CD_USB2_SHIFT		8
#define TCSS_CD_USB2_MASK		0x0f
#define TCSS_CD_UFP_SHIFT		12
#define TCSS_CD_UFP_MASK		0x01
#define TCSS_CD_HSL_SHIFT		13
#define TCSS_CD_HSL_MASK		0x01
#define TCSS_CD_SBU_SHIFT		14
#define TCSS_CD_SBU_MASK		0x01
#define TCSS_CD_ACC_SHIFT		15
#define TCSS_CD_ACC_MASK		0x01
#define TCSS_CD_FAILED_SHIFT		16
#define TCSS_CD_FAILED_MASK		0x01
#define TCSS_CD_FATAL_SHIFT		17
#define TCSS_CD_FATAL_MASK		0x01

#define TCSS_ALT_USAGE_SHIFT		0
#define TCSS_ALT_USAGE_MASK		0x0f
#define TCSS_ALT_USB3_SHIFT		4
#define TCSS_ALT_USB3_MASK		0x0f
#define TCSS_ALT_MODE_SHIFT		12
#define TCSS_ALT_MODE_MASK		0x0f
#define TCSS_ALT_POLARITY_SHIFT		1
#define TCSS_ALT_POLARITY_MASK		0x01
#define TCSS_ALT_CABLE_SHIFT		2
#define TCSS_ALT_CABLE_MASK		0x01
#define TCSS_ALT_UFP_SHIFT		3
#define TCSS_ALT_UFP_MASK		0x01
#define TCSS_ALT_DP_MODE_SHIFT		8
#define TCSS_ALT_DP_MODE_MASK		0x0f
#define TCSS_ALT_FAILED_SHIFT		8
#define TCSS_ALT_FAILED_MASK		0x01
#define TCSS_ALT_FATAL_SHIFT		9
#define TCSS_ALT_FATAL_MASK		0x01

#define TCSS_HPD_USAGE_SHIFT		0
#define TCSS_HPD_USAGE_MASK		0x0f
#define TCSS_HPD_USB3_SHIFT		4
#define TCSS_HPD_USB3_MASK		0x0f
#define TCSS_HPD_LVL_SHIFT		12
#define TCSS_HPD_LVL_MASK		0x01
#define TCSS_HPD_IRQ_SHIFT		13
#define TCSS_HPD_IRQ_MASK		0x01

enum typec_port_index {
	TYPE_C_PORT_0,
	TYPE_C_PORT_1,
	TYPE_C_PORT_2,
	TYPE_C_PORT_3,
	MAX_TYPE_C_PORTS,
};

#define TCSS_CD_FIELD(name, val) \
	(((val) & TCSS_CD_##name##_MASK) << TCSS_CD_##name##_SHIFT)

#define GET_TCSS_CD_FIELD(name, val) \
	(((val) >> TCSS_CD_##name##_SHIFT) & TCSS_CD_##name##_MASK)


#define TCSS_ALT_FIELD(name, val) \
	(((val) & TCSS_ALT_##name##_MASK) << TCSS_ALT_##name##_SHIFT)

#define TCSS_HPD_FIELD(name, val) \
	(((val) & TCSS_HPD_##name##_MASK) << TCSS_HPD_##name##_SHIFT)

#define GET_TCSS_ALT_FIELD(name, val) \
	(((val) >> TCSS_ALT_##name##_SHIFT) & TCSS_ALT_##name##_MASK)

#define TCSS_CONN_STATUS_HAS_FAILED(s)	GET_TCSS_CD_FIELD(FAILED, s)
#define TCSS_STATUS_HAS_FAILED(s)	GET_TCSS_ALT_FIELD(FAILED, s)
/* !fatal means retry */
#define TCSS_CONN_STATUS_IS_FATAL(s)	GET_TCSS_CD_FIELD(FATAL, s)
#define TCSS_STATUS_IS_FATAL(s)		GET_TCSS_ALT_FIELD(FATAL, s)

#define USB_2_PORT_MASK			0x0f
#define USB_3_PORT_MASK			0xf0

/* TCSS connection modes for PMC */
enum pmc_ipc_conn_mode {
	PMC_IPC_TCSS_DISCONNECT_MODE,
	PMC_IPC_TCSS_USB_MODE,
	PMC_IPC_TCSS_ALTERNATE_MODE,
	PMC_IPC_TCSS_SAFE_MODE,
	PMC_IPC_TCSS_HPD_MODE,
	PMC_IPC_TCSS_TOTAL_MODES,
};

enum pmc_ipc_command_type {
	CONNECT_REQ,
	SAFE_REQ,
	DP_REQ,
	HPD_REQ,
};

/* DP Mode pin definitions */
#define MODE_DP_PIN_A BIT(0)
#define MODE_DP_PIN_B BIT(1)
#define MODE_DP_PIN_C BIT(2)
#define MODE_DP_PIN_D BIT(3)
#define MODE_DP_PIN_E BIT(4)
#define MODE_DP_PIN_F BIT(5)

struct tcss_port_map {
	uint8_t usb2_port; /* USB2 Port Number */
	uint8_t usb3_port; /* USB3 Port Number */
};

struct typec_aux_bias_pads {
	gpio_t pad_auxn_dc;
	gpio_t pad_auxp_dc;
};

struct soc_tcss_ops {
	void (*configure_aux_bias_pads)(const struct typec_aux_bias_pads *pads);
	bool (*valid_tbt_auth)(void);
};

extern const struct soc_tcss_ops tcss_ops;

/* Method to configure pads */
void tcss_configure_aux_bias_pads_regbar(const struct typec_aux_bias_pads *pads);
void ioe_tcss_configure_aux_bias_pads_sbi(const struct typec_aux_bias_pads *pads);

/*
 * 1) Initialize TCSS muxes to disconnected state
 * 2) Configure GPIO pads to provide DC Bias on AUX signals
 * 3) Detect DP-over-Type-C alternate mode
 */
void tcss_configure(const struct typec_aux_bias_pads aux_bias_pads[MAX_TYPE_C_PORTS]);

/*
 * Method to get only the port information to initialize the muxes to
 * disconnect mode during boot.
 * Returns tcss_port_map of all ports on system
 */
const struct tcss_port_map *tcss_get_port_info(size_t *num_ports);

/* Method to validate the Thunderbolt authentication */
bool tcss_valid_tbt_auth(void);
bool ioe_tcss_valid_tbt_auth(void);
#endif /* !defined(__ACPI__) */

/*
 * The PCI-SIG engineering change requirement provides the ACPI additions for firmware latency
 * optimization. Both of FW_RESET_TIME and FW_D3HOT_TO_D0_TIME are applicable to the upstream
 * port of the USB4/TBT topology.
 */
/* Number of microseconds to wait after a conventional reset */
#define FW_RESET_TIME			50000

/* Number of microseconds to wait after data link layer active report */
#define FW_DL_UP_TIME			1

/* Number of microseconds to wait after a function level reset */
#define FW_FLR_RESET_TIME		1

/* Number of microseconds to wait from D3 hot to D0 transition */
#define FW_D3HOT_TO_D0_TIME		50000

/* Number of microseconds to wait after setting the VF enable bit  */
#define FW_VF_ENABLE_TIME		1

#endif /* _TCSS_H_ */
