/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/tcss.h>
#include <soc/soc_chip.h>
#include <static.h>

static bool soc_tcss_valid_tbt_auth(void)
{
	const config_t *config = config_of_soc();
	return config->tbt_authentication;
}

const struct soc_tcss_ops tcss_ops = {
	.configure_aux_bias_pads = NULL,
	.valid_tbt_auth = soc_tcss_valid_tbt_auth,
};
