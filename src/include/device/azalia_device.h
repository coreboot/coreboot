/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_AZALIA_H
#define DEVICE_AZALIA_H

#include <acpi/acpi.h>
#include <device/device.h>
#include <device/mmio.h>
#include <types.h>

#define HDA_GCAP_REG		0x00
#define HDA_GCTL_REG		0x08
#define   HDA_GCTL_CRST		(1 << 0)
#define HDA_STATESTS_REG	0x0e
#define HDA_IC_REG		0x60
#define HDA_IR_REG		0x64
#define HDA_ICII_REG		0x68
#define   HDA_ICII_BUSY		(1 << 0)
#define   HDA_ICII_VALID	(1 << 1)

#define AZALIA_MAX_CODECS	15

/*
 * The HDA specification refers to the vendor/device ID (similar to PCI IDs)
 * collectively as "vendor ID". While this could be slightly confusing, we will
 * call it "vendor ID" as the specification and most implementations do.
 */
struct azalia_codec {
	const char *name;
	u32 vendor_id;
	u32 subsystem_id;
	u8 address;
	const u32 *verbs;
	size_t verb_count;
};

enum cb_err azalia_enter_reset(u8 *base);
enum cb_err azalia_exit_reset(u8 *base);
u32 azalia_find_verb(const u32 *verb_table, u32 verb_table_bytes, u32 viddid, const u32 **verb);
int azalia_program_verb_table(u8 *base, const u32 *verbs, u32 verb_size);
#if CONFIG(AZALIA_USE_LEGACY_VERB_TABLE)
void azalia_codec_init(u8 *base, int addr, const u32 *verb_table, u32 verb_table_bytes);
#else
void azalia_codec_init(u8 *base, struct azalia_codec *codec);
void azalia_custom_codecs_init(u8 *base, struct azalia_codec *codecs, u16 codec_mask);
#endif
void azalia_codecs_init(u8 *base, u16 codec_mask);
void azalia_audio_init(struct device *dev);
extern struct device_operations default_azalia_audio_ops;

/* Optional hook to program codec settings that are only known at runtime */
void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid);

#if CONFIG(AZALIA_USE_LEGACY_VERB_TABLE)
extern const u32 cim_verb_data[];
extern const u32 cim_verb_data_size;
#else
extern struct azalia_codec mainboard_azalia_codecs[];
#endif
extern const u32 pc_beep_verbs[];
extern const u32 pc_beep_verbs_size;

/* The tables found in this file are derived from the Intel High Definition
   Audio Specification Revision 1.0a, published 17 June 2010 */

/* Reference: 7.3.3 Controls (page 141) */
enum azalia_verb_id {
	AZALIA_GET_PARAMETER               = 0xf00,
	AZALIA_SET_SUBSYSTEM_ID_1          = 0x720,
	AZALIA_SET_SUBSYSTEM_ID_2          = 0x721,
	AZALIA_SET_SUBSYSTEM_ID_3          = 0x722,
	AZALIA_SET_SUBSYSTEM_ID_4          = 0x723,
	AZALIA_SET_CONFIGURATION_DEFAULT_1 = 0x71c,
	AZALIA_SET_CONFIGURATION_DEFAULT_2 = 0x71d,
	AZALIA_SET_CONFIGURATION_DEFAULT_3 = 0x71e,
	AZALIA_SET_CONFIGURATION_DEFAULT_4 = 0x71f,
	AZALIA_FUNCTION_RESET              = 0x7ff,
};

/* Reference: 7.3.4 Parameters (page 198) */
enum azalia_parameter_id {
	AZALIA_PARAMETER_VENDOR_ID = 0x00,
};

/* Reference: 7.3.3.31 Configuration Default (page 177) */
enum azalia_pin_connection {
	AZALIA_JACK                = 0x0,
	AZALIA_NC                  = 0x1,
	AZALIA_INTEGRATED          = 0x2,
	AZALIA_JACK_AND_INTEGRATED = 0x3,
};

enum azalia_pin_location_gross {
	AZALIA_EXTERNAL_PRIMARY_CHASSIS = 0x00,
	AZALIA_INTERNAL                 = 0x10,
	AZALIA_SEPARATE_CHASSIS         = 0x20,
	AZALIA_LOCATION_OTHER           = 0x30,
};

enum azalia_pin_location_geometric {
	AZALIA_GEOLOCATION_NA = 0x0,
	AZALIA_REAR           = 0x1,
	AZALIA_FRONT          = 0x2,
	AZALIA_LEFT           = 0x3,
	AZALIA_RIGHT          = 0x4,
	AZALIA_TOP            = 0x5,
	AZALIA_BOTTOM         = 0x6,
	AZALIA_SPECIAL7       = 0x7,
	AZALIA_SPECIAL8       = 0x8,
	AZALIA_SPECIAL9       = 0x9,
};

enum azalia_pin_location_special {
	AZALIA_REAR_PANEL         = AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_SPECIAL7,
	AZALIA_DRIVE_BAY          = AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_SPECIAL8,
	AZALIA_RISER              = AZALIA_INTERNAL                 | AZALIA_SPECIAL7,
	AZALIA_DIGITAL_DISPLAY    = AZALIA_INTERNAL                 | AZALIA_SPECIAL8,
	AZALIA_ATAPI              = AZALIA_INTERNAL                 | AZALIA_SPECIAL9,
	AZALIA_MOBILE_LID_INSIDE  = AZALIA_LOCATION_OTHER           | AZALIA_SPECIAL7,
	AZALIA_MOBILE_LID_OUTSIDE = AZALIA_LOCATION_OTHER           | AZALIA_SPECIAL8,
};

enum azalia_pin_device {
	AZALIA_LINE_OUT           = 0x0,
	AZALIA_SPEAKER            = 0x1,
	AZALIA_HP_OUT             = 0x2,
	AZALIA_CD                 = 0x3,
	AZALIA_SPDIF_OUT          = 0x4,
	AZALIA_DIGITAL_OTHER_OUT  = 0x5,
	AZALIA_MODEM_LINE_SIDE    = 0x6,
	AZALIA_MODEM_HANDSET_SIDE = 0x7,
	AZALIA_LINE_IN            = 0x8,
	AZALIA_AUX                = 0x9,
	AZALIA_MIC_IN             = 0xa,
	AZALIA_TELEPHONY          = 0xb,
	AZALIA_SPDIF_IN           = 0xc,
	AZALIA_DIGITAL_OTHER_IN   = 0xd,
	AZALIA_DEVICE_OTHER       = 0xf,
};

enum azalia_pin_type {
	AZALIA_TYPE_UNKNOWN        = 0x0,
	AZALIA_STEREO_MONO_1_8     = 0x1,
	AZALIA_STEREO_MONO_1_4     = 0x2,
	AZALIA_ATAPI_INTERNAL      = 0x3,
	AZALIA_RCA                 = 0x4,
	AZALIA_OPTICAL             = 0x5,
	AZALIA_OTHER_DIGITAL       = 0x6,
	AZALIA_OTHER_ANALOG        = 0x7,
	AZALIA_MULTICHANNEL_ANALOG = 0x8,
	AZALIA_XLR                 = 0x9,
	AZALIA_RJ_11               = 0xa,
	AZALIA_COMBINATION         = 0xb,
	AZALIA_TYPE_OTHER          = 0xf,
};

enum azalia_pin_color {
	AZALIA_COLOR_UNKNOWN = 0x0,
	AZALIA_BLACK         = 0x1,
	AZALIA_GREY          = 0x2,
	AZALIA_BLUE          = 0x3,
	AZALIA_GREEN         = 0x4,
	AZALIA_RED           = 0x5,
	AZALIA_ORANGE        = 0x6,
	AZALIA_YELLOW        = 0x7,
	AZALIA_PURPLE        = 0x8,
	AZALIA_PINK          = 0x9,
	AZALIA_WHITE         = 0xe,
	AZALIA_COLOR_OTHER   = 0xf,
};

enum azalia_pin_misc {
	AZALIA_JACK_PRESENCE_DETECT    = 0x0,
	AZALIA_NO_JACK_PRESENCE_DETECT = 0x1,
};

#define AZALIA_PIN_DESC(conn, location, dev, type, color, misc, association, sequence)	\
	((((conn)        << 30) & 0xc0000000) |						\
	 (((location)    << 24) & 0x3f000000) |						\
	 (((dev)         << 20) & 0x00f00000) |						\
	 (((type)        << 16) & 0x000f0000) |						\
	 (((color)       << 12) & 0x0000f000) |						\
	 (((misc)        <<  8) & 0x00000f00) |						\
	 (((association) <<  4) & 0x000000f0) |						\
	 (((sequence)    <<  0) & 0x0000000f))

#if CONFIG(AZALIA_USE_LEGACY_VERB_TABLE)
#define AZALIA_ARRAY_SIZES const u32 pc_beep_verbs_size =	\
	ARRAY_SIZE(pc_beep_verbs);				\
	const u32 cim_verb_data_size = sizeof(cim_verb_data)
#else
#define AZALIA_ARRAY_SIZES const u32 pc_beep_verbs_size =	\
	ARRAY_SIZE(pc_beep_verbs)
#endif

#define AZALIA_VERB_12B(codec, pin, verb, val)		\
	((codec) << 28 | (pin) << 20 | (verb) << 8 | (val))

#define AZALIA_VERB_GET_VENDOR_ID(codec) \
	AZALIA_VERB_12B(codec, 0, AZALIA_GET_PARAMETER, AZALIA_PARAMETER_VENDOR_ID)

#define AZALIA_PIN_CFG(codec, pin, val)								\
	AZALIA_VERB_12B(codec, pin, AZALIA_SET_CONFIGURATION_DEFAULT_1, ((val) >>  0) & 0xff),	\
	AZALIA_VERB_12B(codec, pin, AZALIA_SET_CONFIGURATION_DEFAULT_2, ((val) >>  8) & 0xff),	\
	AZALIA_VERB_12B(codec, pin, AZALIA_SET_CONFIGURATION_DEFAULT_3, ((val) >> 16) & 0xff),	\
	AZALIA_VERB_12B(codec, pin, AZALIA_SET_CONFIGURATION_DEFAULT_4, ((val) >> 24) & 0xff)

#define AZALIA_PIN_CFG_NC(n)   (0x411111f0 | ((n) & 0xf))

#define AZALIA_RESET(pin)					\
	AZALIA_VERB_12B(0, pin, AZALIA_FUNCTION_RESET, 0),	\
	AZALIA_VERB_12B(0, pin, AZALIA_FUNCTION_RESET, 0),	\
	AZALIA_VERB_12B(0, pin, AZALIA_FUNCTION_RESET, 0),	\
	AZALIA_VERB_12B(0, pin, AZALIA_FUNCTION_RESET, 0)

#define AZALIA_SUBVENDOR(codec, val)							\
	AZALIA_VERB_12B(codec, 1, AZALIA_SET_SUBSYSTEM_ID_1, ((val) >>  0) & 0xff),	\
	AZALIA_VERB_12B(codec, 1, AZALIA_SET_SUBSYSTEM_ID_2, ((val) >>  8) & 0xff),	\
	AZALIA_VERB_12B(codec, 1, AZALIA_SET_SUBSYSTEM_ID_3, ((val) >> 16) & 0xff),	\
	AZALIA_VERB_12B(codec, 1, AZALIA_SET_SUBSYSTEM_ID_4, ((val) >> 24) & 0xff)

#endif /* DEVICE_AZALIA_H */
