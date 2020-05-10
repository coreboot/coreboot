/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pnp_type.h>
#include <superio/ite/common/ite.h>
#include "it8718f.h"

/*
 * GIGABYTE uses a special Super I/O register to protect its Dual BIOS
 * mechanism. It lives in the GPIO LDN. However, register 0xEF is not
 * mentioned in the IT8718F datasheet so just hardcode it to 0x7E for now.
 */
void it8718f_disable_reboot(pnp_devfn_t dev)
{
	ite_reg_write(dev, 0xEF, 0x7E);
}
