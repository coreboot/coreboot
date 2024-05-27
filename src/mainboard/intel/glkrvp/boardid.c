/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <ec/acpi/ec.h>

#define BOARD_ID_GLK_RVP1_DDR4	0x5 /* RVP1 - DDR4 */
#define BOARD_ID_GLK_RVP2_LP4SD	0x7 /* RVP2 - LP4 Solder Down */
#define BOARD_ID_GLK_RVP2_LP4	0x8 /* RVP2 - LP4 Socket */
#define EC_FAB_ID_CMD	0x0D /* Get the board fab ID in the lower 3 bits */

uint32_t board_id(void)
{
	static int id = -1;
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
