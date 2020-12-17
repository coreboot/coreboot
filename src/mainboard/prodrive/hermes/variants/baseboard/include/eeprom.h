/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

#define I2C_ADDR_EEPROM 0x57

#if ENV_ROMSTAGE
#define FSP_UPD_SIGNATURE FSPM_UPD_SIGNATURE
#define EEPROM_OFFSET_FSP_SIGNATURE 0
#define EEPROM_OFFSET_FSP_CONFIG 0

#define GET_VALUE(x) {.offset = sizeof(FSP_UPD_HEADER) + sizeof(FSPM_ARCH_UPD) \
		+ offsetof(FSP_M_CONFIG, x), .size = member_size(FSP_M_CONFIG, x)}
#else

#define FSP_UPD_SIGNATURE FSPS_UPD_SIGNATURE
#define EEPROM_OFFSET_FSP_SIGNATURE 0x0600
#define EEPROM_OFFSET_FSP_CONFIG (EEPROM_OFFSET_FSP_SIGNATURE + sizeof(FSP_UPD_HEADER))
#define GET_VALUE(x) {.offset = offsetof(FSP_S_CONFIG, x), \
					.size = member_size(FSP_S_CONFIG, x)}
#endif /* ENV_ROMSTAGE */

typedef struct {
	size_t offset;
	size_t size;
} fsp_params;

bool read_write_config(u8 addr, void *blob, size_t read_offset, size_t write_offset,
		size_t size);
int check_signature(u8 addr, const size_t offset, const uint64_t signature);
