/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <soc/ramstage.h>
#include <types.h>

union eeprom_dimm_layout {
	struct __packed {
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

_Static_assert(sizeof(union eeprom_dimm_layout) == 0x80,
		"union eeprom_dimm_layout has invalid size!");

struct __packed eeprom_board_layout {
	uint32_t signature;
	union {
		struct __packed {
			char cpu_name[50];
			uint8_t cpu_count;
			uint32_t cpu_max_non_turbo_frequency;
			char pch_name[50];
			union eeprom_dimm_layout dimm[4];
		};
		uint8_t raw_layout[617];
	};
};

_Static_assert(sizeof(struct eeprom_board_layout) == (617 + sizeof(uint32_t)),
		"struct eeprom_board_layout has invalid size!");

struct __packed eeprom_board_settings {
	uint32_t signature;
	union {
		struct __packed {
			uint8_t secureboot;
			uint8_t primary_video;
			uint8_t deep_sx_enabled;
			uint8_t wake_on_usb;
			uint8_t usb_powered_in_s5;
			uint8_t power_state_after_g3;
			uint8_t blue_rear_vref;
			uint8_t front_panel_audio;
			uint8_t pxe_boot_capability;
			uint8_t pink_rear_vref;
			uint8_t vtx_disabled;
			uint8_t boot_menu_disabled;
		};
		uint8_t raw_settings[12];
	};
};

_Static_assert(sizeof(struct eeprom_board_settings) == (12 + sizeof(uint32_t)),
		"struct eeprom_board_settings has invalid size!");

struct __packed eeprom_bmc_settings {
	uint8_t pcie_mux;
	uint8_t hsi;
	uint8_t efp3_displayport;
};

#define HERMES_SN_PN_LENGTH	32

/* The EEPROM on address 0x57 has the following vendor defined layout: */
struct __packed eeprom_layout {
	union {
		uint8_t raw_fspm_upd[0x600];
		FSPM_UPD mupd;
	};
	union {
		uint8_t raw_fsps_upd[0xc00];
		FSPS_UPD supd;
	};
	union {
		uint8_t raw_board_layout[0x400];
		struct eeprom_board_layout board_layout;
	};
	char system_serial_number[HERMES_SN_PN_LENGTH];
	char board_serial_number[HERMES_SN_PN_LENGTH];
	uint8_t boot_order[0x200];
	char board_part_number[HERMES_SN_PN_LENGTH];
	char product_part_number[HERMES_SN_PN_LENGTH];
	uint8_t unused[0x680];
	union {
		uint8_t raw_board_settings[0xf8];
		struct eeprom_board_settings board_settings;
	};
	union {
		uint8_t raw_bmc_settings[0x8];
		struct eeprom_bmc_settings bmc_settings;
	};
};

_Static_assert(sizeof(FSPM_UPD) <= 0x600, "FSPM_UPD too big");
_Static_assert(sizeof(FSPS_UPD) <= 0xc00, "FSPS_UPD too big");
_Static_assert(sizeof(struct eeprom_layout) == 0x2000, "EEPROM layout size mismatch");

bool eeprom_read_buffer(void *blob, size_t read_offset, size_t size);
int check_signature(const size_t offset, const uint64_t signature);
struct eeprom_board_settings *get_board_settings(void);
struct eeprom_bmc_settings *get_bmc_settings(void);
const char *eeprom_read_serial(size_t offset, const char *fallback);
uint8_t get_bmc_hsi(void);
void report_eeprom_error(const size_t off);
bool write_board_settings(const struct eeprom_board_layout *new_layout);

#define READ_EEPROM(section_type, section_name, dest, opt_name)				\
	do {										\
		typeof(dest->opt_name) __tmp;						\
		size_t __off = offsetof(struct eeprom_layout, section_name);		\
		bool ret = eeprom_read_buffer(&__tmp,					\
					__off + offsetof(section_type, opt_name),	\
					sizeof(__tmp));					\
		if (ret) {								\
			report_eeprom_error(__off + offsetof(section_type, opt_name));	\
		} else {								\
			dest->opt_name = __tmp;						\
		}									\
	} while (0)

#define READ_EEPROM_FSP_M(dest, opt_name) READ_EEPROM(FSPM_UPD, raw_fspm_upd, dest, opt_name)
#define READ_EEPROM_FSP_S(dest, opt_name) READ_EEPROM(FSPS_UPD, raw_fsps_upd, dest, opt_name)
