/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/tcss.h>

const struct soc_tcss_ops tcss_ops = {
	.configure_aux_bias_pads = tcss_configure_aux_bias_pads_regbar,
	.valid_tbt_auth = tcss_valid_tbt_auth,
};
