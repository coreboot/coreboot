/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CHIPINFO_PARTNUM_*
 *
 * Definitions of part number/JTAG-ID fields.
 */
#define CHIPINFO_PARTNUM_SM_KODIAK		0x192
#define CHIPINFO_PARTNUM_SC_KODIAK_CHROME	0x193
#define CHIPINFO_PARTNUM_SC_KODIAK_WINDOWS	0x194
#define CHIPINFO_PARTNUM_QCM_KODIAK		0x197
#define CHIPINFO_PARTNUM_QCS_KODIAK		0x198
#define CHIPINFO_PARTNUM_SMP_KODIAK		0x1A1
#define CHIPINFO_PARTNUM_SM_KODIAK_LTE_ONLY	0x1B5
#define CHIPINFO_PARTNUM_SCP_KODIAK		0x1EB
#define CHIPINFO_PARTNUM_SC_8CGEN3		0x1E3
#define CHIPINFO_PARTNUM_SCP_8CGEN3		0x20A
#define CHIPINFO_PARTNUM_KODIAK_SCP_7CGEN3	0x215
#define CHIPINFO_PARTNUM_QCS_KODIAK_LITE	0x20F
#define CHIPINFO_PARTNUM_QCM_KODIAK_LITE	0x20E


#define DEVICE_ID                       0xFFFF

struct chipinfo {
	uint16_t jtagid : 14;
	uint16_t modem  : 1;
	uint16_t pro    : 1;
};

uint16_t socinfo_modem_supported(void);
uint16_t socinfo_pro_part(void);
