/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

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

__packed struct eeprom_board_settings {
	uint32_t signature;
	union {
		struct {
			uint8_t secureboot;
			uint8_t primary_video;
			uint8_t deep_sx_enabled;
			uint8_t wake_on_usb;
			uint8_t usb_powered_in_s5;
			uint8_t power_state_after_g3;
			uint8_t blue_rear_vref;
			uint8_t internal_audio_connection;
			uint8_t pxe_boot_capability;
		};
		uint8_t raw_settings[9];
	};
};

typedef struct {
	size_t offset;
	size_t size;
} fsp_params;

bool read_write_config(void *blob, size_t read_offset, size_t write_offset, size_t size);
int check_signature(const size_t offset, const uint64_t signature);
struct eeprom_board_settings *get_board_settings(void);
