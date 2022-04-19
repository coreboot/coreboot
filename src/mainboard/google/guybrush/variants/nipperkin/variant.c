/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <string.h>

void variant_update_dxio_descriptors(fsp_dxio_descriptor *dxio_descriptors)
{
	dxio_descriptors[WLAN].link_aspm_L1_1 = false;
	dxio_descriptors[WLAN].link_aspm_L1_2 = false;

	/* Fix link speed to GEN2 - b/228830362 */
	dxio_descriptors[WLAN].link_speed_capability = GEN2;
	dxio_descriptors[WLAN].port_params[0] = PP_PSPP_AC;
	/* AC_DirectIndirect[11:8], AC_MaxPreferredSpeed[7:4], AC_MinPreferredSpeed[3:0] */
	dxio_descriptors[WLAN].port_params[1] = 0x122;
	dxio_descriptors[WLAN].port_params[2] = PP_PSPP_DC;
	/* DC_DirectIndirect[11:8], DC_MaxPreferredSpeed[7:4], DC_MinPreferredSpeed[3:0] */
	dxio_descriptors[WLAN].port_params[3] = 0x122;
}
