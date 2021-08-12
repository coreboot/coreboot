/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mipi_panel.h>

cb_err_t mipi_panel_parse_init_commands(const void *buf, mipi_cmd_func_t cmd_func)
{
	const struct panel_init_command *init = buf;

	/*
	 * The given commands should be in a buffer containing a packed array of
	 * panel_init_command and each element may be in variable size so we have
	 * to parse and scan.
	 */

	for (; init->cmd != PANEL_CMD_END; init = (const void *)buf) {
		/*
		 * For some commands like DELAY, the init->len should not be
		 * counted for buf.
		 */
		buf += sizeof(*init);

		u32 cmd = init->cmd, len = init->len;

		switch (cmd) {
		case PANEL_CMD_DELAY:
			mdelay(len);
			break;

		case PANEL_CMD_DCS:
		case PANEL_CMD_GENERIC:
			buf += len;

			cb_err_t ret = cmd_func(cmd, init->data, len);
			if (ret != CB_SUCCESS)
				return ret;
			break;

		default:
			printk(BIOS_ERR, "%s: Unknown command code: %d, "
			       "abort panel initialization.\n", __func__, cmd);
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}
