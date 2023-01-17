/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SIEMENS_HWI_LIB_H_
#define SIEMENS_HWI_LIB_H_

#include <types.h>

/* Declare all supported fields which can be get with hwilib. */
typedef enum {
	HIB_VerID = 0,
	SIB_VerID,
	EIB_VerID,
	XIB_VerID,
	HIB_HwRev,
	SIB_HwRev,
	HWID,
	UniqueNum,
	Mac1,
	Mac2,
	Mac3,
	Mac4,
	Mac1Aux,
	Mac2Aux,
	Mac3Aux,
	Mac4Aux,
	SPD,
	FF_FreezeDis,
	FF_FanReq,
	NvramVirtTimeDsaveReset,
	BiosFlags,
	MacMapping1,
	MacMapping2,
	MacMapping3,
	MacMapping4,
	RTCType,
	BL_Brightness,
	PF_PwmFreq,
	PF_Color_Depth,
	PF_DisplType,
	PF_DisplCon,
	Edid,
	VddRef,
	T_Warn,
	T_Crit,
	FANSamplingTime,
	FANSetPoint,
	FANKp,
	FANKi,
	FANKd,
	FANHystVal,
	FANHystThreshold,
	FANHystCtrl,
	FANMaxSpeed,
	FANStartSpeed,
	FANSensorDelay,
	FANSensorNum,
	FANSensorSelect,
	FANSensorCfg0,
	FANSensorCfg1,
	FANSensorCfg2,
	FANSensorCfg3,
	FANSensorCfg4,
	FANSensorCfg5,
	FANSensorCfg6,
	FANSensorCfg7,
	XMac1,
	XMac2,
	XMac3,
	XMac4,
	XMac5,
	XMac6,
	XMac7,
	XMac8,
	XMac9,
	XMac10,
	XMac1Aux,
	XMac2Aux,
	XMac3Aux,
	XMac4Aux,
	XMac5Aux,
	XMac6Aux,
	XMac7Aux,
	XMac8Aux,
	XMac9Aux,
	XMac10Aux,
	XMac1Mapping,
	XMac2Mapping,
	XMac3Mapping,
	XMac4Mapping,
	XMac5Mapping,
	XMac6Mapping,
	XMac7Mapping,
	XMac8Mapping,
	XMac9Mapping,
	XMac10Mapping,
	netKind1,
	netKind2,
	netKind3,
	netKind4,
	netKind5,
	netKind6,
	netKind7,
	netKind8,
	netKind9,
	netKind10,
	LegacyDelay
} hwinfo_field_t;

/* Define used values in supported fields */
#define PF_COLOR_DEPTH_6BIT	0x00
#define PF_COLOR_DEPTH_8BIT	0x01
#define PF_COLOR_DEPTH_10BIT	0x02
#define PF_DISPLCON_LVDS_SINGLE	0x00
#define PF_DISPLCON_LVDS_DUAL	0x05
#define FF_FREEZE_DISABLE	0x01
#define FF_FAN_NEEDED		0x01

/* Use this function to find all supported blocks in cbfs. It must be called
 * once with a valid cbfs file name before hwilib_get_field() can be used.
 */
enum cb_err hwilib_find_blocks(const char *hwi_filename);

/* Use this function to get fields out of supported info blocks
 * This function returns the number of copied bytes or 0 on error.
 */
uint32_t hwilib_get_field(hwinfo_field_t field, uint8_t *data, uint32_t maxlen);

#endif /* SIEMENS_HWI_LIB_H_ */
