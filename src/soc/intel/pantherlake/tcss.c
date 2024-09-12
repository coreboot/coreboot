/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/tcss.h>
#include <soc/soc_chip.h>

const struct soc_tcss_ops tcss_ops = {
/* TODO: Implement AUX BIAS PAD Programming if required */
	.configure_aux_bias_pads = NULL,
	.valid_tbt_auth = NULL,
};
