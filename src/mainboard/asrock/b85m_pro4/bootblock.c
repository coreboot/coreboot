/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <southbridge/intel/lynxpoint/pch.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)
#define ACPI_DEV   PNP_DEV(0x2e, NCT6776_ACPI)

/*
 * Asrock B85M Pro4 Super I/O GPIOs
 *
 * +------+-----+---------------------------+
 * | GPIO | Pin | Description               |
 * +------+-----+---------------------------+
 * | GP00 |   2 | N/C                       |
 * | GP01 |   4 | CPU Fan 4-pin tach enable |
 * | GP02 |   5 | CPU Fan 3-pin tach enable |
 * | GP03 |   8 | CPU Fan 3-pin FON# signal |
 * | GP04 |   9 | N/C                       |
 * | GP05 |  11 | N/C     (+1.05V_PCH_GPIO) |
 * | GP06 |  12 | N/C      (+1.5V_PCH_GPIO) |
 * | GP07 |  13 | N/C                       |
 * +------+-----+---------------------------+
 * | GP10 | 123 | N/C            (VCCM_OV1) |
 * | GP11 | 122 | N/C            (VCCM_OV2) |
 * | GP12 | 121 | N/C            (VCCM_OV3) |
 * | GP13 | 120 | N/C           (VCCM_STEP) |
 * | GP14 | 119 | Assert HDA_SDO (SIO_GP14) |
 * | GP15 | 118 | N/C        (PWM_THROTTLE) |
 * | GP16 | 117 | OTP for VCORE (OTE_GATE1) |
 * | GP17 | 116 | N/C           (IMON_GPIO) |
 * +------+-----+---------------------------+
 * | GP70 |  93 | PWR_FANIN                 |
 * | GP71 |  92 | N/C                       |
 * | GP72 |  91 | N/C           (SIO_PIN91) |
 * | GP73 |  90 | CHA2_FANIN                |
 * | GP74 |  89 | N/C           (SIO_PIN89) |
 * | GP75 |  88 | N/C           (SIO_PIN88) |
 * | GP76 |  87 | HDA reset gate     (GP76) |
 * | GP77 |  86 | HDD_LED gate              |
 * +------+-----+---------------------------+
 */

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x68);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x80);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x1c);
	pnp_write_config(GLOBAL_DEV, 0x27, 0xd0);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x62);
	pnp_write_config(GLOBAL_DEV, 0x2f, 0x03);

	/* Power RAM in S3 and let the PCH handle power failure actions */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x70);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
