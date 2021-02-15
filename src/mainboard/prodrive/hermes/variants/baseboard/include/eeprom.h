/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

__packed union eeprom_dimm_layout {
	struct {
		char name[50];
		char manufacturer[50];
		uint8_t ranks;
		uint8_t controller_id;
		uint8_t data_width_bits;
		uint8_t bus_width_bits;
		uint32_t capacity_mib;
		uint32_t max_tdp_milliwatts;
	};
	uint8_t raw[0x80];
};

__packed struct eeprom_board_layout {
	uint32_t signature;
	union {
		struct {
			char cpu_name[50];
			uint8_t cpu_count;
			uint32_t cpu_max_non_turbo_frequency;
			char pch_name[50];
			union eeprom_dimm_layout dimm[4];
		};
		uint8_t raw_layout[617];
	};
};

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

__packed struct eeprom_bmc_settings {
	uint8_t pcie_mux;
	uint8_t hsi;
};

/* The EEPROM on address 0x57 has the following vendor defined layout: */
__packed struct eeprom_layout {
	union {
		uint8_t RawFSPMUPD[0x600];
		FSPM_UPD mupd;
	};
	union {
		uint8_t RawFSPSUPD[0xC00];
		FSPS_UPD supd;
	};
	union {
		uint8_t RawBoardLayout[0x400];
		struct eeprom_board_layout BoardLayout;
	};
	uint8_t BootOrder[0x900];
	union {
		uint8_t RawBoardSetting[0xF8];
		struct eeprom_board_settings BoardSettings;
	};
	union {
		uint8_t RawBMCSetting[0x8];
		struct eeprom_bmc_settings BMCSettings;
	};
};

_Static_assert(sizeof(FSPM_UPD) <= 0x600, "FSPM_UPD too big");
_Static_assert(sizeof(FSPS_UPD) <= 0xC00, "FSPS_UPD too big");
_Static_assert(sizeof(struct eeprom_layout) == 0x2000, "EEPROM layout size mismatch");

bool read_write_config(void *blob, size_t read_offset, size_t write_offset, size_t size);
int check_signature(const size_t offset, const uint64_t signature);
struct eeprom_board_settings *get_board_settings(void);
struct eeprom_bmc_settings *get_bmc_settings(void);
void report_eeprom_error(const size_t off);
bool write_board_settings(const struct eeprom_board_layout *new_layout);

#define READ_EEPROM(section_type, section_name, dest, opt_name)				\
	do {										\
		typeof(dest->opt_name) __tmp;						\
		size_t __off = offsetof(struct eeprom_layout, section_name);		\
		bool ret = read_write_config(&__tmp,					\
					__off + offsetof(section_type, opt_name),	\
					0,						\
					sizeof(__tmp));					\
		if (ret) {								\
			report_eeprom_error(__off + offsetof(section_type, opt_name));	\
		} else {								\
			dest->opt_name = __tmp;						\
		}									\
	} while (0)

#define READ_EEPROM_FSP_M(dest, opt_name) READ_EEPROM(FSPM_UPD, RawFSPMUPD, dest, opt_name)
#define READ_EEPROM_FSP_S(dest, opt_name) READ_EEPROM(FSPS_UPD, RawFSPSUPD, dest, opt_name)
