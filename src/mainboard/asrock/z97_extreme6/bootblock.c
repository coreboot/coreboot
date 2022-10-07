/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>

#define GLOBAL_DEV	PNP_DEV(0x2e, 0)
#define SERIAL_DEV	PNP_DEV(0x2e, NCT6791D_SP1)
#define ACPI_DEV	PNP_DEV(0x2e, NCT6791D_ACPI)
#define GPIO_PP_OD_DEV	PNP_DEV(0x2e, NCT6791D_GPIO_PP_OD)

/*
 * Asrock Z97 Extreme6 Super I/O GPIOs
 *
 * +------+-----+---------------------------+
 * | GPIO | Pin | Description               |
 * +------+-----+---------------------------+
 * | GP00 | 121 | N/C                       |
 * | GP01 | 122 | CHA_FAN2 PWM output       |
 * | GP02 | 123 | CHA_FAN3 PWM output       |
 * | GP03 |   2 | N/C                       |
 * | GP04 |   3 | CHA_FAN3 tach input       |
 * | GP05 |   4 | CHA_FAN2 tach input       |
 * | GP06 |   5 | PWR_FAN  tach input       |
 * | GP07 |   6 | N/C            (SE_IFDET) |
 * +------+-----+---------------------------+
 * | GP10 |  14 | HDD Saver power switch    |
 * | GP11 |  13 | Assert HDA_SDO (SIO_GP11) |
 * | GP12 |  12 | CPU_FAN2 FON#             |
 * | GP13 |  11 | SATA_SEL (for eSATA)      |
 * | GP14 |  10 | N/C                       |
 * | GP15 |   9 | N/C          (UARTP80_EN) |
 * | GP16 |   8 | OTP for VCORE (OTE_GATE1) |
 * | GP17 |   7 | LED_EN#                   |
 * +------+-----+---------------------------+
 * | GP20 |  59 | KDAT                      |
 * | GP21 |  58 | KCLK                      |
 * | GP22 |  57 | MDAT                      |
 * | GP23 |  56 | MCLK                      |
 * | GP24 |  95 | SE_DEVSLP (SATA Express)  |
 * | GP25 |  96 | N/C            (SIO_GP25) |
 * | GP26 |  53 | N/C                       |
 * | GP27 |  98 | M2_2_SE_IFDET             |
 * +------+-----+---------------------------+
 * | GP30 |  83 | N/C           (RESETCON#) |
 * | GP31 |  76 | BIOS_A (or SML1DAT)       |
 * | GP32 |  75 | BIOS_B (or SML1CLK)       |
 * | GP33 |  71 | 3VSBSW#                   |
 * | GP34 |  55 | VCORE_OFFSET#             |
 * | GP35 |  54 | N/C                       |
 * | GP36 |  53 | N/C                       |
 * | GP37 |   7 | LED_EN#                   |
 * +------+-----+---------------------------+
 * | GP40 |  62 | N/C             (TEST_EN) |
 * | GP41 |  52 | N/C                       |
 * | GP42 |  51 | WLAN1_ON/OFF#             |
 * | GP43 |  41 | Port 80 display - DGL_0#  |
 * | GP44 |  40 | PWR_LED gate              |
 * | GP45 |  39 | HDD_LED gate              |
 * | GP46 |  38 | CHA_FAN3 FON#             |
 * | GP47 |  37 | CHA_FAN2 FON#             |
 * +------+-----+---------------------------+
 * | GP50 |  93 | N/C            (SUSWARN#) |
 * | GP51 |  92 | CPU_FAN2 tach input       |
 * | GP52 |  91 | N/C             (SUSACK#) |
 * | GP53 |  90 | SUSWARN_5VDUAL            |
 * | GP54 |  89 | SLP_SUS#                  |
 * | GP55 |  88 | SLP_SUS_FET               |
 * | GP56 |  87 | PEG12V_DET (Molex conn)   |
 * | GP57 |  86 | PCIE4_SEL (PCIE3 / mPCIe) |
 * +------+-----+---------------------------+
 * | GP70 |  69 | N/C              (DSW_EN) |
 * | GP71 |  68 | N/C                       |
 * | GP72 |  67 | N/C                       |
 * | GP73 |  66 | M.2 / SATA Express select |
 * | GP74 |  79 | RESET# of long PCIe ports |
 * | GP75 |  78 | RESET# for on-board chips |
 * | GP76 |  77 | RESET# SATA Express / M.2 |
 * | GP77 |  86 | HDD_LED gate              |
 * +------+-----+---------------------------+
 *
 * HWM voltage inputs
 *
 * +------+-----+---------------------------+
 * | Name | Pin | Voltage (resistor values) |
 * +------+-----+---------------------------+
 * | VIN0 | 104 | +12V         (110K / 10K) |
 * | VIN1 | 105 | +5V           (20K / 10K) |
 * | VIN2 | 106 | CPU_VRING                 |
 * | VIN3 | 107 | CPU_VSA                   |
 * | VIN4 | 111 | CPU_VCORE0                |
 * | VIN5 | 114 | CPU_VGFX                  |
 * | VIN6 | 115 | V_VCCIOA_LOAD             |
 * | VIN7 | 116 | N/C                       |
 * | VIN8 | 103 | CPU_VIO                   |
 * +------+-----+---------------------------+
 */

void mainboard_config_superio(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin mux states */
	pnp_write_config(GLOBAL_DEV, 0x1b, 0xe6);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x10);
	pnp_write_config(GLOBAL_DEV, 0x24, 0xfc);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x40);
	pnp_write_config(GLOBAL_DEV, 0x2b, 0x20);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);
	pnp_write_config(GLOBAL_DEV, 0x2d, 0x02);

	/* Select push-pull vs. open-drain output */
	pnp_set_logical_device(GPIO_PP_OD_DEV);
	pnp_write_config(GPIO_PP_OD_DEV, 0xe0, 0xfe);
	pnp_write_config(GPIO_PP_OD_DEV, 0xe2, 0x79);
	pnp_write_config(GPIO_PP_OD_DEV, 0xe6, 0x6f);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
