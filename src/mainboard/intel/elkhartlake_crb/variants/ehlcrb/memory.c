/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

/* ToDo : Fill EHL related memory configs */

const struct mb_cfg *variant_memcfg_config(uint8_t board_id)
{
	/* ToDo : Fill EHL related memory configs */

	die("unsupported board id : 0x%x\n", board_id);
}
