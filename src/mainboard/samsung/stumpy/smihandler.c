/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>

/* Include romstage serial for SIO helper functions */
#include <superio/ite/it8772f/early_serial.c>

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1;
}

/*
 * Change LED_POWER# (SIO GPIO 45) state based on sleep type.
 * The IO address is hardcoded as we don't have device path in SMM.
 */
#define SIO_GPIO_BASE_SET4	(0x730 + 3)
#define SIO_GPIO_BLINK_GPIO45	0x25
void mainboard_smi_sleep(u8 slp_typ)
{
	u8 reg8;

	switch (slp_typ) {
	case 3:
	case 4:
		/* Blink LED */
		it8772f_enter_conf();
		it8772f_sio_write(IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
		/* Enable blink pin map */
		it8772f_sio_write(IT8772F_GPIO_LED_BLINK1_PINMAP,
				  SIO_GPIO_BLINK_GPIO45);
		/* Enable 4HZ blink */
		it8772f_sio_write(IT8772F_GPIO_LED_BLINK1_CONTROL, 0x02);
		/* Set GPIO to alternate function */
		reg8 = it8772f_sio_read(GPIO_REG_ENABLE(3));
		reg8 &= ~(1 << 5);
		it8772f_sio_write(GPIO_REG_ENABLE(3), reg8);
		it8772f_exit_conf();
		break;

	case 5:
		/* Turn off LED */
		reg8 = inb(SIO_GPIO_BASE_SET4);
		reg8 |= (1 << 5);
		outb(reg8, SIO_GPIO_BASE_SET4);
		break;
	}
}

#define APMC_FINALIZE 0xcb

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APMC_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;
	}
	return 0;
}
