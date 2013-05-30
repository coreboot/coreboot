/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <console/console.h>
#include <arch/io.h>

#include "fw_cfg.h"
#include "fw_cfg_if.h"

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511

static unsigned char fw_cfg_detected = 0xff;

static int fw_cfg_present(void)
{
	static const char qsig[] = "QEMU";
	unsigned char sig[4];

	if (fw_cfg_detected == 0xff) {
		fw_cfg_get(FW_CFG_SIGNATURE, sig, sizeof(sig));
		fw_cfg_detected = (memcmp(sig, qsig, 4) == 0) ? 1 : 0;
		printk(BIOS_INFO, "QEMU: firmware config interface %s\n",
		       fw_cfg_detected ? "detected" : "not found");
	}
	return fw_cfg_detected;
}

void fw_cfg_get(int entry, void *dst, int dstlen)
{
	outw(entry, FW_CFG_PORT_CTL);
	insb(FW_CFG_PORT_DATA, dst, dstlen);
}

int fw_cfg_max_cpus(void)
{
	unsigned short max_cpus;

	if (!fw_cfg_present())
		return -1;

	fw_cfg_get(FW_CFG_MAX_CPUS, &max_cpus, sizeof(max_cpus));
	return max_cpus;
}
