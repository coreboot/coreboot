/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

#define READ_OP					0
#define WRITE_OP				1

#define EC_HWM_LDN				1
#define EC_GPIO_LDN				2

/* EC GPIO configuration */
#define EC_GPIO_PP				(0 << 0)
#define EC_GPIO_OD				(1 << 0)
#define EC_GPIO_FUNC0				(0 << 4)
#define EC_GPIO_FUNC1				(1 << 4)
#define EC_GPIO_FUNC2				(2 << 4)
#define EC_GPIO_FUNC3				(3 << 4)

struct ec_val_reg {
	uint8_t val;
	uint16_t reg;
};

uint16_t sch5545_get_ec_fw_version(void);
void sch5545_update_ec_firmware(uint16_t ec_version);
void sch5545_ec_early_init(void);
void sch5545_ec_hwm_early_init(void);
void sch5545_ec_hwm_init(void *unused);
