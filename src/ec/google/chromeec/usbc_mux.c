/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>

static const struct usbc_ops google_chromeec_usbc_ops = {
	.mux_ops = {
		.get_mux_info = google_chromeec_get_usbc_mux_info,
	},
	.dp_ops = {
		.wait_for_connection = google_chromeec_wait_for_displayport,
		.enter_dp_mode = google_chromeec_typec_control_enter_dp_mode,
		.wait_for_hpd = google_chromeec_wait_for_dp_hpd,
	},
};

const struct usbc_ops *usbc_get_ops(void)
{
	return &google_chromeec_usbc_ops;
}
