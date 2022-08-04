/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/tcss.h>
#include <soc/soc_chip.h>

const struct soc_tcss_ops tcss_ops = {
	.configure_aux_bias_pads = ioe_tcss_configure_aux_bias_pads_sbi,
	.valid_tbt_auth = ioe_tcss_valid_tbt_auth,
};

bool ioe_tcss_valid_tbt_auth(void)
{
	const config_t *config = config_of_soc();
	return config->tbt_authentication;
}
