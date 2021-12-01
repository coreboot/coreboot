/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <ec/acpi/ec.h>
#include <swab.h>
#include <timer.h>
#include <types.h>

#include "chip.h"
#include "commands.h"
#include "ec.h"

#define ec_cmd	send_ec_command
#define ec_dat	send_ec_data

static void ec_fcmd(uint8_t fcmd)
{
	write8p(ECRAM + FCMD, fcmd);
	ec_cmd(ECCMD_NOP);

	/* EC sets FCMD = 0x00 on completion (FCMD = 0xfa on some commands) */
	int time = wait_us(50000, read8p(ECRAM + FCMD) == 0x00 || read8p(ECRAM + FCMD) == 0xfa);
	if (time)
		printk(BIOS_DEBUG, "EC: FCMD 0x%02x completed after %d us\n", fcmd, time);
	else
		printk(BIOS_ERR, "EC: FCMD 0x%02x timed out\n", fcmd);
}

static void ec_recv_str(char *buf, size_t size)
{
	while (size--) {
		*buf = recv_ec_data();
		if (*buf == '$') { /* end mark */
			*buf = '\0';
			return;
		}
		buf++;
	}

	/* Truncate and discard the rest */
	*--buf = '\0';
	do {} while (recv_ec_data() != '$');
	printk(BIOS_ERR, "EC: Received string longer than buffer. Data truncated.\n");
}

char *ec_read_model(void)
{
	static char model[10];

	ec_cmd(ECCMD_READ_MODEL);
	ec_recv_str(model, sizeof(model));

	return model;
}

char *ec_read_fw_version(void)
{
	static char version[10] = "1.";

	ec_cmd(ECCMD_READ_FW_VER);
	ec_recv_str(version + 2, sizeof(version) - 2);

	return version;
}

void ec_set_acpi_mode(bool state)
{
	ec_cmd(state ? ECCMD_ENABLE_ACPI_MODE : ECCMD_DISABLE_ACPI_MODE);
	if (state)
		ec_cmd(ECCMD_ENABLE_HOTKEYS);
}

void ec_set_enter_g3_in_s4s5(bool state)
{
	clrsetbits8p(ECRAM + 0x1e6, 1 << G3FG, state << G3FG);
}

void ec_set_aprd(void)
{
	setbits8p(ECRAM + 0x1eb, 1 << APRD);
}

/* To be called by a graphics driver, when detecting a dGPU */
void ec_set_dgpu_present(bool state)
{
	clrsetbits8p(ECRAM + 0x1eb, 1 << DGPT, state << DGPT);
}

void ec_set_fn_win_swap(bool state)
{
	clrsetbits8p(ECRAM + ECKS, 1 << SWFN, state << SWFN);
}

void ec_set_ac_fan_always_on(bool state)
{
	clrsetbits8p(ECRAM + 0x1e6, 1 << FOAC, state << FOAC);
}

void ec_set_kbled_timeout(uint16_t timeout)
{
	printk(BIOS_DEBUG, "EC: set keyboard backlight timeout to %us\n", timeout);

	write8p(ECRAM + FDAT, timeout ? 0xff : 0x00);
	write16p(ECRAM + FBUF, swab16(timeout));
	ec_fcmd(FCMD_SET_KBLED_TIMEOUT);
}

void ec_set_flexicharger(bool state, uint8_t start, uint8_t stop)
{
	printk(BIOS_DEBUG, "EC: set flexicharger: enabled=%d, start=%u%%, stop=%u%%\n",
			   state, start, stop);

	if (!state) {
		start = 0xff;
		stop  = 0xff;

	} else if (start > 100 || stop > 100) {
		printk(BIOS_ERR, "EC: invalid flexicharger settings: start/stop > 100%%\n");
		return;

	} else if (start >= stop) {
		printk(BIOS_ERR, "EC: invalid flexicharger settings: start >= stop\n");
		return;
	}

	write8p(ECRAM + FBF1, state << 1);
	write8p(ECRAM + FBUF, start);
	write8p(ECRAM + FDAT, stop);
	ec_fcmd(FCMD_FLEXICHARGER);
}

void ec_set_camera_boot_state(enum camera_state state)
{
	if (state > CAMERA_STATE_KEEP) {
		printk(BIOS_ERR,
		       "EC: invalid camera boot state %u. Keeping previous state.\n", state);
		state = CAMERA_STATE_KEEP;
	}

	if (state == CAMERA_STATE_KEEP) {
		/*
		 * The EC maintains the camera's state in RAM. However, it doesn't sync the GPIO
		 * on a concurrent boot. Thus, read the previous state from the EC and set the
		 * state and the GPIO by sending the state command even in the keep-case.
		 */
		ec_cmd(ECCMD_GET_DEVICES_STATE);
		state = recv_ec_data() & 1;
	}

	printk(BIOS_DEBUG, "EC: set camera: enabled=%u\n", state);

	ec_dat(DEVICE_CAMERA | DEVICE_STATE(state));
	ec_cmd(ECCMD_SET_INV_DEVICE_STATE);
}

void ec_set_tp_toggle_mode(uint8_t mode)
{
	switch (mode) {
	case 0:	/* CtrlAltF9 */
		setbits8p(ECRAM + RINF, TP_TOGGLE_CTRLALTF9);
		break;
	case 1:	/* KeycodeF7F8*/
		clrbits8p(ECRAM + RINF, TP_TOGGLE_CTRLALTF9);
		break;
	}
}
