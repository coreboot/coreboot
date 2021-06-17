/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <types.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0,  0 }, /* P0:, OC 0 */
	{ 1, 1,  1 }, /* P1: (EHCI debug), OC 1 */
	{ 1, 1,  3 }, /* P2:  OC 3 */
	{ 1, 0, -1 }, /* P3: no OC */
	{ 1, 2, -1 }, /* P4: no OC */
	{ 1, 1, -1 }, /* P5: no OC */
	{ 1, 1, -1 }, /* P6: no OC */
	{ 0, 1, -1 }, /* P7: empty, no OC */
	{ 1, 1, -1 }, /* P8: smart card reader, no OC */
	{ 1, 0,  5 }, /* P9:  (EHCI debug), OC 5 */
	{ 1, 0, -1 }, /* P10: fingerprint reader, no OC */
	{ 1, 1, -1 }, /* P11: bluetooth, no OC. */
	{ 0, 0, -1 }, /* P12: wlan, no OC */
	{ 1, 1, -1 }, /* P13: camera, no OC */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}

void mainboard_early_init(int s3resume)
{
	u8 enable_peg = get_uint_option("enable_dual_graphics", 0);

	bool power_en = pmh7_dgpu_power_state();

	if (enable_peg != power_en)
		pmh7_dgpu_power_enable(!power_en);

	if (!enable_peg) {
		// Hide disabled dGPU device
		pci_and_config32(HOST_BRIDGE, DEVEN, ~DEVEN_PEG10);
	}
}
