/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <baseboard/variants.h>
#include <boardid.h>
#include <stddef.h>
#include <ec/acpi/ec.h>

#define BOARD_ID_GLK_RVP1_DDR4	0x5 /* RVP1 - DDR4 */
#define BOARD_ID_GLK_RVP2_LP4SD	0x7 /* RVP2 - LP4 Solder Down */
#define BOARD_ID_GLK_RVP2_LP4	0x8 /* RVP2 - LP4 Socket */
#define EC_FAB_ID_CMD	0x0D /* Get the board fab ID in the lower 3 bits */

uint32_t board_id(void)
{
	MAYBE_STATIC_NONZERO int id = -1;
	if (id < 0) {
		if (CONFIG(EC_GOOGLE_CHROMEEC))
			id = variant_board_id();
		else {
			if (send_ec_command(EC_FAB_ID_CMD) == 0)
				id = (recv_ec_data() & 0x07);
		}
	}
	return id;
}
