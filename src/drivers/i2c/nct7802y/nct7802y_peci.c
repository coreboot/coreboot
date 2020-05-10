/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>

#include "nct7802y.h"
#include "chip.h"

void nct7802y_init_peci(struct device *const dev)
{
	const struct drivers_i2c_nct7802y_config *const config = dev->chip_info;
	unsigned int i, all_off = 1;

	/* Bank 1 can only be written to if PECI reading is enabled */
	if (nct7802y_select_bank(dev, 0) != CB_SUCCESS)
		return;
	nct7802y_update(dev, PECI_ENABLE, 0, PECI_ENABLE_AGENTx(0));

	if (nct7802y_select_bank(dev, 1) != CB_SUCCESS)
		return;

	for (i = 0; i < NCT7802Y_PECI_CNT; ++i) {
		if (config->peci[i].mode != PECI_DISABLED) {
			u8 ctrl3 = 0, style = 0;
			switch (config->peci[i].mode) {
			case PECI_DOMAIN_0:
				ctrl3 = PECI_CTRL_3_EN_AGENTx(i);
				style = PECI_TEMP_STYLE_DOM0_AGENTx(i) |
					PECI_TEMP_STYLE_SINGLE;
				break;
			case PECI_DOMAIN_1:
				ctrl3 = PECI_CTRL_3_EN_AGENTx(i) |
					PECI_CTRL_3_HAS_DOM1_AGENTx(i);
				style = PECI_TEMP_STYLE_DOM1_AGENTx(i) |
					PECI_TEMP_STYLE_SINGLE;
				break;
			case PECI_HIGHEST:
				ctrl3 = PECI_CTRL_3_EN_AGENTx(i) |
					PECI_CTRL_3_HAS_DOM1_AGENTx(i);
				style = PECI_TEMP_STYLE_HIGHEST;
				break;
			default:
				break;
			}
			nct7802y_update(dev, PECI_CTRL_1,
					PECI_CTRL_1_MANUAL_EN,
					PECI_CTRL_1_EN |
						PECI_CTRL_1_ROUTINE_EN);
			nct7802y_update(dev, PECI_CTRL_3,
					PECI_CTRL_3_HAS_DOM1_AGENTx(i), ctrl3);
			nct7802y_update(dev, PECI_REPORT_TEMP_STYLE,
					PECI_TEMP_STYLE_DOM1_AGENTx(i) |
						PECI_TEMP_STYLE_HIGHEST,
					style);
			nct7802y_write(dev, PECI_BASE_TEMP_AGENT(i),
				       config->peci[i].base_temp);
			all_off = 0;
		} else {
			nct7802y_update(dev, PECI_CTRL_3,
					PECI_CTRL_3_EN_AGENTx(i), 0);
		}
	}

	if (all_off)
		nct7802y_update(dev, PECI_CTRL_1, PECI_CTRL_1_EN, 0);

	/* Disable PECI #0 reading if we only enabled it to access bank 1 */
	if (config->peci[0].mode == PECI_DISABLED) {
		if (nct7802y_select_bank(dev, 0) != CB_SUCCESS)
			return;

		nct7802y_update(dev, PECI_ENABLE, PECI_ENABLE_AGENTx(0), 0);
	}
}
