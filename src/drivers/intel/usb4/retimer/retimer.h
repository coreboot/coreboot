/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _DRIVERS_INTEL_USB4_RETIMER_H_
#define _DRIVERS_INTEL_USB4_RETIMER_H_

/* Flags representing mux state */
#define USB_PD_MUX_NONE               0      /* Open switch */
#define USB_PD_MUX_USB_ENABLED        BIT(0) /* USB connected */
#define USB_PD_MUX_DP_ENABLED         BIT(1) /* DP connected */
#define USB_PD_MUX_SAFE_MODE          BIT(5) /* DP is in safe mode */
#define USB_PD_MUX_TBT_COMPAT_ENABLED BIT(6) /* TBT compat enabled */
#define USB_PD_MUX_USB4_ENABLED       BIT(7) /* USB4 enabled */

/* USB Retimer firmware update mux mask */
#define USB_RETIMER_FW_UPDATE_MUX_MASK (USB_PD_MUX_USB_ENABLED | \
					USB_PD_MUX_DP_ENABLED  | \
					USB_PD_MUX_SAFE_MODE   | \
					USB_PD_MUX_TBT_COMPAT_ENABLED | \
					USB_PD_MUX_USB4_ENABLED)

#define USB_RETIMER_FW_UPDATE_OP_SHIFT		4
#define USB_RETIMER_FW_UPDATE_ERROR		0xfe
#define USB_RETIMER_FW_UPDATE_INVALID_MUX	0xff

/* Retimer firmware update operations */
#define USB_RETIMER_FW_UPDATE_SUSPEND_PD 1 /* Suspend PD port */
#define USB_RETIMER_FW_UPDATE_RESUME_PD  2 /* Resume PD port  */
#define USB_RETIMER_FW_UPDATE_GET_MUX    3 /* Read current USB MUX  */
#define USB_RETIMER_FW_UPDATE_SET_USB    4 /* Set MUX to USB mode   */
#define USB_RETIMER_FW_UPDATE_SET_SAFE   5 /* Set MUX to Safe mode  */
#define USB_RETIMER_FW_UPDATE_SET_TBT    6 /* Set MUX to TBT mode   */
#define USB_RETIMER_FW_UPDATE_DISCONNECT 7 /* Set MUX to disconnect */

struct usb4_retimer_dsm_uuid {
	const char *uuid;
	void (**callbacks)(uint8_t port, void *);
	size_t count;
	void *arg;
};

const char *ec_retimer_fw_update_path(void);
void ec_retimer_fw_update(uint8_t data);
/*
 * This function will convert CPU physical port mapping to abstract
 * EC port mapping. For example, board might have enabled TCSS port 1
 * and 3 as per physical port mapping. Since only 2 TCSS ports are enabled
 * EC will name it as port 0 and port 1. So there will be mismatch when
 * coreboot sends index for port 3.
 * Each SoC code using retimer driver needs to implement this function
 * since SoC will have physical port details.
 */
int retimer_get_index_for_typec(uint8_t typec_port);

#endif /* _DRIVERS_INTEL_USB4_RETIMER_H_ */
