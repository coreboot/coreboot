/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <bootmode.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmc_ipc.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tcss.h>
#include <inttypes.h>
#include <security/vboot/vboot_common.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/tcss.h>
#include <drivers/intel/pmc_mux/conn/chip.h>

#define BIAS_CTRL_VW_INDEX_SHIFT		16
#define BIAS_CTRL_BIT_POS_SHIFT			8
#define WAIT_FOR_DISPLAYPORT_TIMEOUT_MS		1000
#define WAIT_FOR_HPD_TIMEOUT_MS			3000

static uint32_t tcss_make_conn_cmd(int u, int u3, int u2, int ufp, int hsl,
					int sbu, int acc)
{
	return TCSS_CD_FIELD(USAGE, u) |
		TCSS_CD_FIELD(USB3, u3) |
		TCSS_CD_FIELD(USB2, u2) |
		TCSS_CD_FIELD(UFP, ufp) |
		TCSS_CD_FIELD(HSL, hsl) |
		TCSS_CD_FIELD(SBU, sbu) |
		TCSS_CD_FIELD(ACC, acc);
}

static uint32_t tcss_make_alt_mode_cmd_buf_0(int u, int u3, int m)
{
	return TCSS_ALT_FIELD(USAGE, u) |
		TCSS_ALT_FIELD(USB3, u3) |
		TCSS_ALT_FIELD(MODE, m);

}

static uint32_t tcss_make_alt_mode_cmd_buf_1(int p, int c, int ufp, int dp)
{
	return TCSS_ALT_FIELD(POLARITY, p) |
		TCSS_ALT_FIELD(CABLE, c) |
		TCSS_ALT_FIELD(UFP, ufp) |
		TCSS_ALT_FIELD(DP_MODE, dp);
}

static uint32_t tcss_make_safe_mode_cmd(int u, int u3)
{
	return TCSS_CD_FIELD(USAGE, u) |
		TCSS_CD_FIELD(USB3, u3);
}


static uint32_t tcss_make_hpd_mode_cmd(int u, int u3, int hpd_lvl, int hpd_irq)
{
	return TCSS_HPD_FIELD(USAGE, u) |
		TCSS_HPD_FIELD(USB3, u3) |
		TCSS_HPD_FIELD(LVL, hpd_lvl) |
		TCSS_HPD_FIELD(IRQ, hpd_irq);

}

static int send_pmc_req(int cmd_type, const struct pmc_ipc_buffer *req,
			struct pmc_ipc_buffer *res, uint32_t size)
{
	uint32_t cmd_reg;
	uint32_t res_reg;
	int tries = 2;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_USBC_CMD_ID, PMC_IPC_USBC_SUBCMD_ID,
				   size);

	printk(BIOS_DEBUG, "Raw Buffer output 0 %08" PRIx32 "\n", req->buf[0]);
	printk(BIOS_DEBUG, "Raw Buffer output 1 %08" PRIx32 "\n", req->buf[1]);

	do {
		r = pmc_send_ipc_cmd(cmd_reg, req, res);
		if (r < 0) {
			printk(BIOS_ERR, "pmc_send_ipc_cmd failed\n");
			return -1;
		}

		res_reg = res->buf[0];
		if (cmd_type == CONNECT_REQ) {
			if (!TCSS_CONN_STATUS_HAS_FAILED(res_reg)) {
				printk(BIOS_DEBUG, "pmc_send_ipc_cmd succeeded\n");
				return 0;
			}

			if (TCSS_CONN_STATUS_IS_FATAL(res_reg)) {
				printk(BIOS_ERR, "pmc_send_ipc_cmd status: fatal\n");
				return -1;
			}
		} else {
			if (!TCSS_STATUS_HAS_FAILED(res_reg)) {
				printk(BIOS_DEBUG, "pmc_send_ipc_cmd succeeded\n");
				return 0;
			}

			if (TCSS_STATUS_IS_FATAL(res_reg)) {
				printk(BIOS_ERR, "pmc_send_ipc_cmd status: fatal\n");
				return -1;
			}
		}
	} while (--tries >= 0);

	printk(BIOS_ERR, "pmc_send_ipc_cmd failed after retries\n");
	return -1;
}

static int send_pmc_disconnect_request(int port, const struct tcss_port_map *port_map)
{
	uint32_t cmd;
	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;

	cmd = tcss_make_conn_cmd(PMC_IPC_TCSS_DISC_REQ_RES, port_map->usb3_port,
				port_map->usb2_port, 0, 0, 0, 0);

	req.buf[0] = cmd;

	printk(BIOS_DEBUG, "port C%d DISC req: usage %d usb3 %d usb2 %d\n",
		port,
		GET_TCSS_CD_FIELD(USAGE, cmd),
		GET_TCSS_CD_FIELD(USB3, cmd),
		GET_TCSS_CD_FIELD(USB2, cmd));

	return send_pmc_req(CONNECT_REQ, &req, &rsp, PMC_IPC_DISC_REQ_SIZE);
}

static int send_pmc_connect_request(int port, const struct usbc_mux_info *mux_data,
					const struct tcss_port_map *port_map)
{
	uint32_t cmd;
	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;

	cmd = tcss_make_conn_cmd(
		PMC_IPC_TCSS_CONN_REQ_RES,
		port_map->usb3_port,
		port_map->usb2_port,
		mux_data->ufp,
		mux_data->polarity,
		mux_data->polarity,
		mux_data->dbg_acc);

	req.buf[0] = cmd;

	printk(BIOS_DEBUG, "port C%d CONN req: usage %d usb3 %d usb2 %d "
	      "ufp %d ori_hsl %d ori_sbu %d dbg_acc %d\n",
	      port,
	      GET_TCSS_CD_FIELD(USAGE, cmd),
	      GET_TCSS_CD_FIELD(USB3, cmd),
	      GET_TCSS_CD_FIELD(USB2, cmd),
	      GET_TCSS_CD_FIELD(UFP, cmd),
	      GET_TCSS_CD_FIELD(HSL, cmd),
	      GET_TCSS_CD_FIELD(SBU, cmd),
	      GET_TCSS_CD_FIELD(ACC, cmd));

	return send_pmc_req(CONNECT_REQ, &req, &rsp, PMC_IPC_CONN_REQ_SIZE);
}

static int send_pmc_safe_mode_request(int port, const struct usbc_mux_info *mux_data,
					const struct tcss_port_map *port_map)
{
	uint32_t cmd;
	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;

	cmd = tcss_make_safe_mode_cmd(PMC_IPC_TCSS_SAFE_MODE_REQ_RES, port_map->usb3_port);

	req.buf[0] = cmd;

	printk(BIOS_DEBUG, "port C%d SAFE req: usage %d usb3 %d\n",
		port,
		GET_TCSS_CD_FIELD(USAGE, cmd),
		GET_TCSS_CD_FIELD(USB3, cmd));

	return send_pmc_req(SAFE_REQ, &req, &rsp, PMC_IPC_SAFE_REQ_SIZE);
}

static int send_pmc_dp_hpd_request(int port, const struct usbc_mux_info *mux_data,
					const struct tcss_port_map *port_map)
{
	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;
	uint32_t cmd;

	cmd = tcss_make_hpd_mode_cmd(
		PMC_IPC_TCSS_HPD_REQ_RES,
		port_map->usb3_port,
		mux_data->hpd_lvl,
		mux_data->hpd_irq);

	req.buf[0] = cmd;

	return send_pmc_req(HPD_REQ, &req, &rsp, PMC_IPC_HPD_REQ_SIZE);
}

static int send_pmc_dp_mode_request(int port, const struct usbc_mux_info *mux_data,
					const struct tcss_port_map *port_map)
{
	uint32_t cmd;
	uint8_t dp_mode;
	int ret;

	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;

	cmd = tcss_make_alt_mode_cmd_buf_0(
		PMC_IPC_TCSS_ALTMODE_REQ_RES,
		port_map->usb3_port,
		PMC_IPC_DP_MODE);

	req.buf[0] = cmd;

	printk(BIOS_DEBUG, "port C%d ALT_1 req: usage %d usb3 %d dp_mode %d\n",
		port,
		GET_TCSS_ALT_FIELD(USAGE, cmd),
		GET_TCSS_ALT_FIELD(USB3, cmd),
		GET_TCSS_ALT_FIELD(MODE, cmd));

	switch (mux_data->dp_pin_mode) {
	case MODE_DP_PIN_A:
		dp_mode = 1;
		break;
	case MODE_DP_PIN_B:
		dp_mode = 2;
		break;
	case MODE_DP_PIN_C:
		dp_mode = 3;
		break;
	case MODE_DP_PIN_D:
		dp_mode = 4;
		break;
	case MODE_DP_PIN_E:
		dp_mode = 5;
		break;
	case MODE_DP_PIN_F:
		dp_mode = 6;
		break;
	default:
		dp_mode = 0;
		break;
	}

	cmd = tcss_make_alt_mode_cmd_buf_1(
		mux_data->polarity,
		mux_data->cable,
		0, /* ufp is not supported in DP ALT Mode request */
		dp_mode);

	printk(BIOS_DEBUG, "port C%d ALT_2 req: polarity %d cable %d ufp %d "
				"dp_mode %d\n",
		port,
		GET_TCSS_ALT_FIELD(POLARITY, cmd),
		GET_TCSS_ALT_FIELD(CABLE, cmd),
		GET_TCSS_ALT_FIELD(UFP, cmd),
		GET_TCSS_ALT_FIELD(DP_MODE, cmd));

	req.buf[1] = cmd;

	ret = send_pmc_req(DP_REQ, &req, &rsp, PMC_IPC_ALT_REQ_SIZE);
	if (ret)
		return ret;

	send_pmc_dp_hpd_request(port, mux_data, port_map);
	return 0;
}

static void tcss_init_mux(int port, const struct tcss_port_map *port_map)
{
	int ret;

	ret = send_pmc_disconnect_request(port, port_map);
	if (ret)
		printk(BIOS_ERR, "Failed to setup port:%d to initial state\n", port);
}

static void tcss_configure_dp_mode(const struct tcss_port_map *port_map, size_t num_ports)
{
	int ret;
	size_t i;
	const struct usbc_ops *ops;
	struct usbc_mux_info mux_info;
	const struct tcss_port_map *port_info;

	if (!display_init_required())
		return;

	ops = usbc_get_ops();
	if (ops == NULL)
		return;

	for (i = 0; i < num_ports; i++) {
		ret = ops->mux_ops.get_mux_info(i, &mux_info);
		if ((ret < 0) || (!mux_info.dp))
			continue;

		port_info = &port_map[i];

		ret = send_pmc_connect_request(i, &mux_info, port_info);
		if (ret) {
			printk(BIOS_ERR, "Port %zd connect request failed\n", i);
			continue;
		}
		ret = send_pmc_safe_mode_request(i, &mux_info, port_info);
		if (ret) {
			printk(BIOS_ERR, "Port %zd safe mode request failed\n", i);
			continue;
		}

		ret = send_pmc_dp_mode_request(i, &mux_info, port_info);
		if (ret) {
			printk(BIOS_ERR, "Port C%zd mux set failed with error %d\n", i, ret);
		} else {
			printk(BIOS_INFO, "Port C%zd is configured to DP mode!\n", i);
			return;
		}
	}
}

static uint32_t calc_bias_ctrl_reg_value(gpio_t pad)
{
	unsigned int vw_index, vw_bit;
	const unsigned int cpu_pid = gpio_get_pad_cpu_portid(pad);
	if (!gpio_get_vw_info(pad, &vw_index, &vw_bit) || !cpu_pid)
		return 0;

	return vw_index << BIAS_CTRL_VW_INDEX_SHIFT |
		vw_bit << BIAS_CTRL_BIT_POS_SHIFT |
		cpu_pid;
}

void tcss_configure_aux_bias_pads_regbar(
	const struct typec_aux_bias_pads *pads)
{
	for (size_t i = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (pads[i].pad_auxn_dc && pads[i].pad_auxp_dc) {
			REGBAR32(PID_IOM, IOM_AUX_BIAS_CTRL_PULLUP_OFFSET(i)) =
				calc_bias_ctrl_reg_value(pads[i].pad_auxp_dc);
			REGBAR32(PID_IOM, IOM_AUX_BIAS_CTRL_PULLDOWN_OFFSET(i)) =
				calc_bias_ctrl_reg_value(pads[i].pad_auxn_dc);
		}
	}
}

void ioe_tcss_configure_aux_bias_pads_sbi(
	const struct typec_aux_bias_pads *pads)
{
	for (size_t i = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (pads[i].pad_auxn_dc && pads[i].pad_auxp_dc) {
			ioe_p2sb_sbi_write(PID_IOM, IOM_AUX_BIAS_CTRL_PULLUP_OFFSET(i),
				calc_bias_ctrl_reg_value(pads[i].pad_auxp_dc));
			ioe_p2sb_sbi_write(PID_IOM, IOM_AUX_BIAS_CTRL_PULLDOWN_OFFSET(i),
				calc_bias_ctrl_reg_value(pads[i].pad_auxn_dc));
		}
	}
}

const struct tcss_port_map *tcss_get_port_info(size_t *num_ports)
{
	static struct tcss_port_map port_map[MAX_TYPE_C_PORTS];
	size_t active_ports = 0;
	size_t port;

	for (port = 0; port < MAX_TYPE_C_PORTS; port++) {
		const struct device_path conn_path[] = {
			{.type = DEVICE_PATH_PCI, .pci.devfn = PCH_DEVFN_PMC},
			{.type = DEVICE_PATH_GENERIC, .generic.id = 0, .generic.subid = 0},
			{.type = DEVICE_PATH_GENERIC, .generic.id = port},
		};
		const struct device *conn = find_dev_nested_path(pci_root_bus(), conn_path,
								ARRAY_SIZE(conn_path));
		unsigned int usb2_port, usb3_port;

		if (!is_dev_enabled(conn))
			continue;

		if (CONFIG(DRIVERS_INTEL_PMC) &&
			intel_pmc_mux_conn_get_ports(conn, &usb2_port, &usb3_port)) {
			port_map[active_ports].usb2_port = usb2_port;
			port_map[active_ports].usb3_port = usb3_port;
			++active_ports;
		}
	}

	*num_ports = active_ports;
	return port_map;
}

void tcss_configure(const struct typec_aux_bias_pads aux_bias_pads[MAX_TYPE_C_PORTS])
{
	const struct tcss_port_map *port_map;
	size_t num_ports;
	size_t i;

	port_map = tcss_get_port_info(&num_ports);
	if (port_map == NULL)
		return;

	if (!platform_is_resuming()) {
		for (i = 0; i < num_ports; i++)
			tcss_init_mux(i, &port_map[i]);
	}

	/* This should be performed before alternate modes are entered */
	if (tcss_ops.configure_aux_bias_pads)
		tcss_ops.configure_aux_bias_pads(aux_bias_pads);

	if (CONFIG(ENABLE_TCSS_DISPLAY_DETECTION))
		tcss_configure_dp_mode(port_map, num_ports);
}

bool tcss_valid_tbt_auth(void)
{
	return REGBAR32(PID_IOM, IOM_CSME_IMR_TBT_STATUS) & TBT_VALID_AUTHENTICATION;
}
