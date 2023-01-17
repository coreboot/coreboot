/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <string.h>
#include <console/console.h>
#include <device/mmio.h>
#include <types.h>

#include "hwilib.h"

#define MAX_BLOCK_NUM		4
#define LEN_HIB			0x1fd
#define LEN_SIB			0x121
#define LEN_EIB			0x0b5
#define MIN_LEN_XIB		0x201
#define NEXT_OFFSET_HIB		0x1dc
#define NEXT_OFFSET_SIB		0x104
#define NEXT_OFFSET_EIB		0x0b0
#define NEXT_OFFSET_XIB		0x014
#define LEN_UNIQUEL_NUM		0x010
#define LEN_HW_REV		0x002
#define LEN_MAC_ADDRESS		0x006
#define LEN_SPD			0x080
#define LEN_EDID		0x080
#define LEN_OFFSET		0x00c
#define EIB_FEATRUE_OFFSET	0x00e
#define LEN_MAGIC_NUM		0x007
#define BLOCK_MAGIC		"H1W2M3I"
#define HWI_MAX_NAME_LEN	32

/* Define all supported block types. */
enum {
	BLK_HIB,
	BLK_SIB,
	BLK_EIB,
	BLK_XIB
};

/* This structure holds a valid position for a given field
 * Every field can have multiple positions of which the first available
 * will be taken by the library.
 */
struct param_pos {
	uint8_t blk_type;	/* Valid for a specific block type */
	uint32_t offset;	/* Offset in given block */
	size_t len;		/* Length for the field in this block */
};

/* This structure holds all the needed information for a given field type
 * and a pointer to a function which is able to extract the desired data.
 */
struct param_info {
	struct param_pos pos[MAX_BLOCK_NUM];
	uint64_t mask;
	uint8_t mask_offset;
	uint32_t (*get_field)(const struct param_info *param, uint8_t *dst, size_t dstsize);
};

/* Storage for pointers to the different blocks. The contents will be filled
 * in hwilib_find_blocks().
 */
static uint8_t *all_blocks[MAX_BLOCK_NUM];

/* As the length of extended block is variable, save all length to a global
 * variable so that they can be used later to check boundaries.
 */
static uint16_t all_blk_size[MAX_BLOCK_NUM];

/* Storage for the cbfs file name of the currently open hwi file. */
static char current_hwi[HWI_MAX_NAME_LEN];

static uint32_t hwilib_read_bytes(const struct param_info *param, uint8_t *dst, size_t dstsize);

/* Add all supported fields to this variable. It is important to use the
 * field type of a given field as the array index so that all the information
 * is on the appropriate place inside the array. In this way one do not need
 * to search for fields but can simply use an index into the array.
 */
static const struct param_info params[] = {
	[HIB_VerID] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x8, .len = 4},
		.get_field = hwilib_read_bytes },
	[SIB_VerID] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0x8, .len = 4},
		.get_field = hwilib_read_bytes },
	[EIB_VerID] = {
		.pos[0] = {.blk_type = BLK_EIB, .offset = 0x8, .len = 4},
		.get_field = hwilib_read_bytes },
	[XIB_VerID] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x8, .len = 4},
		.get_field = hwilib_read_bytes },
	[HIB_HwRev] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xbe, .len = 2},
		.get_field = hwilib_read_bytes },
	[SIB_HwRev] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xc8, .len = 2},
		.get_field = hwilib_read_bytes },
	[HWID] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1a8, .len = 4},
		.pos[1] = {.blk_type = BLK_SIB, .offset = 0xd0, .len = 4},
		.get_field = hwilib_read_bytes },
	[UniqueNum] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xa2, .len = 10},
		.pos[1] = {.blk_type = BLK_SIB, .offset = 0xa2, .len = 10},
		.get_field = hwilib_read_bytes },
	[Mac1] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xc0, .len = 6},
		.get_field = hwilib_read_bytes },
	[Mac1Aux] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xc6, .len = 1},
		.get_field = hwilib_read_bytes },
	[Mac2] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xc8, .len = 6},
		.get_field = hwilib_read_bytes },
	[Mac2Aux] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xce, .len = 1},
		.get_field = hwilib_read_bytes },
	[Mac3] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xd0, .len = 6},
		.get_field = hwilib_read_bytes },
	[Mac3Aux] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xd6, .len = 1},
		.get_field = hwilib_read_bytes },
	[Mac4] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xd8, .len = 6},
		.get_field = hwilib_read_bytes },
	[Mac4Aux] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xde, .len = 1},
		.get_field = hwilib_read_bytes },
	[SPD] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0xe0, .len = 0x80},
		.get_field = hwilib_read_bytes },
	[FF_FreezeDis] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1b8, .len = 1},
		.mask = 0x10,
		.mask_offset = 4,
		.get_field = hwilib_read_bytes },
	[FF_FanReq] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1b9, .len = 1},
		.mask = 0x04,
		.mask_offset = 2,
		.get_field = hwilib_read_bytes },
	[NvramVirtTimeDsaveReset] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1be, .len = 2},
		.get_field = hwilib_read_bytes },
	[BiosFlags] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1c0, .len = 4},
		.get_field = hwilib_read_bytes },
	[MacMapping1] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1cc, .len = 4},
		.get_field = hwilib_read_bytes },
	[MacMapping2] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1d0, .len = 4},
		.get_field = hwilib_read_bytes },
	[MacMapping3] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1d4, .len = 4},
		.get_field = hwilib_read_bytes },
	[MacMapping4] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1d8, .len = 4},
		.get_field = hwilib_read_bytes },
	[RTCType] = {
		.pos[0] = {.blk_type = BLK_HIB, .offset = 0x1e8, .len = 1},
		.get_field = hwilib_read_bytes },
	[BL_Brightness] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xd8, .len = 1},
		.get_field = hwilib_read_bytes },
	[PF_PwmFreq] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xe7, .len = 1},
		.get_field = hwilib_read_bytes },
	[PF_Color_Depth] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xea, .len = 1},
		.mask = 0x03,
		.mask_offset = 0,
		.get_field = hwilib_read_bytes },
	[PF_DisplType] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xe3, .len = 1},
		.get_field = hwilib_read_bytes },
	[PF_DisplCon] = {
		.pos[0] = {.blk_type = BLK_SIB, .offset = 0xf2, .len = 1},
		.get_field = hwilib_read_bytes },
	[Edid] = {
		.pos[0] = {.blk_type = BLK_EIB, .offset = 0x10, .len = 0x80},
		.get_field = hwilib_read_bytes },
	[VddRef] = {
		.pos[0] = {.blk_type = BLK_EIB, .offset = 0x90, .len = 2},
		.get_field = hwilib_read_bytes },
	[XMac1] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0xfc, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac1Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x102, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac2] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x114, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac2Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x11a, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac3] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x12c, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac3Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x132, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac4] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x144, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac4Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x14a, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac5] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x15c, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac5Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x162, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac6] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x174, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac6Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x17a, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac7] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x18c, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac7Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x192, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac8] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1a4, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac8Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1aa, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac9] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1bc, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac9Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1c2, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac10] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1d4, .len = 6},
		.get_field = hwilib_read_bytes },
	[XMac10Aux] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1da, .len = 1},
		.get_field = hwilib_read_bytes },
	[XMac1Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0xec, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac2Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x104, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac3Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x11c, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac4Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x134, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac5Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x14c, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac6Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x164, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac7Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x17c, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac8Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x194, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac9Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1ac, .len = 16},
		.get_field = hwilib_read_bytes },
	[XMac10Mapping] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1c4, .len = 16},
		.get_field = hwilib_read_bytes },
	[netKind1] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x103, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind2] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x11b, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind3] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x133, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind4] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x14b, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind5] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x163, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind6] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x17b, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind7] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x193, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind8] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1ab, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind9] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1c3, .len = 1},
		.get_field = hwilib_read_bytes },
	[netKind10] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1db, .len = 1},
		.get_field = hwilib_read_bytes },
	[T_Warn] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x18, .len = 4},
		.get_field = hwilib_read_bytes },
	[T_Crit] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x1c, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANSamplingTime] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x20, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANSetPoint] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x24, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANKp] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x28, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANKi] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x2c, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANKd] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x30, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANHystVal] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x34, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANHystThreshold] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x38, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANHystCtrl] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x3c, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANMaxSpeed] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x40, .len = 2},
		.get_field = hwilib_read_bytes },
	[FANStartSpeed] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x42, .len = 2},
		.get_field = hwilib_read_bytes },
	[FANSensorDelay] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x44, .len = 4},
		.get_field = hwilib_read_bytes },
	[FANSensorNum] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x48, .len = 1},
		.get_field = hwilib_read_bytes },
	[FANSensorSelect] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x49, .len = 1},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg0] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x4c, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg1] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x60, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg2] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x74, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg3] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x88, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg4] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x9c, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg5] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0xb0, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg6] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0xc4, .len = 20},
		.get_field = hwilib_read_bytes },
	[FANSensorCfg7] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0xd8, .len = 20},
		.get_field = hwilib_read_bytes },
	[LegacyDelay] = {
		.pos[0] = {.blk_type = BLK_XIB, .offset = 0x20c, .len = 4},
		.get_field = hwilib_read_bytes },
};

/** \brief This functions reads the given field from the first valid hwinfo
 *         block
 * @param  *param	Parameter to read from hwinfo
 * @param  *dst		Pointer to memory where the data will be stored in
 * @param  dstsize	Size of the memory passed in via the *dst pointer
 * @return		Number of copied bytes on success, 0 on error
 */
static uint32_t hwilib_read_bytes(const struct param_info *param, uint8_t *dst, size_t dstsize)
{
	uint8_t i = 0, *blk = NULL;

	if (!param || !dst)
		return 0;
	/* Take the first valid block to get the parameter from */
	do {
		if ((param->pos[i].len) && (param->pos[i].offset) &&
		     (all_blocks[param->pos[i].blk_type])) {
			blk =  all_blocks[param->pos[i].blk_type];
			break;
		}
		i++;
	} while (i < MAX_BLOCK_NUM);

	/* Ensure there is a valid block available for this parameter and
	 * the length of the parameter do not exceed dstsize or block len.
	 */
	if ((!blk) || (param->pos[i].len > dstsize) ||
	    (param->pos[i].len + param->pos[i].offset >
	     all_blk_size[param->pos[i].blk_type]))
		return 0;
	/* We can now copy the wanted data. */
	memcpy(dst, (blk + param->pos[i].offset), param->pos[i].len);
	/* If there is a mask given, apply it only for parameters with a
	 * length of 1, 2, 4 or 8 bytes.
	 */
	if (param->mask) {
		switch (param->pos[i].len) {
		case 1:
			/* Apply a mask on a 8 bit value */
			*dst &= (param->mask & 0xff);
			*dst >>= (param->mask_offset);
			break;
		case 2:
			/* Apply mask on a 16 bit value */
			*((uint16_t *)(dst)) &= (param->mask & 0xffff);
			*((uint16_t *)(dst)) >>= (param->mask_offset);
			break;
		case 4:
			/* Apply mask on a 32 bit value */
			*((uint32_t *)(dst)) &= (param->mask & 0xffffffff);
			*((uint32_t *)(dst)) >>= (param->mask_offset);
			break;
		case 8:
			/* Apply mask on a 64 bit value */
			*((uint64_t *)(dst)) &= (param->mask);
			*((uint64_t *)(dst)) >>= (param->mask_offset);
			break;
		default:
			/* Warn if there is a mask for an invalid length. */
			printk(BIOS_WARNING,
			      "HWILIB: Invalid field length for given mask.\n");
			break;
		}
	}
	return param->pos[i].len;
}

/** \brief This function finds all available block types in a given cbfs file.
 * @param  *hwi_filename	Name of the cbfs-file to use.
 * @return			CB_SUCCESS when no error, otherwise error code
 */
enum cb_err hwilib_find_blocks(const char *hwi_filename)
{
	uint8_t *ptr = NULL, *base = NULL;
	uint32_t next_offset = 1;
	size_t filesize = 0;

	/* Check for a valid parameter */
	if (!hwi_filename)
		return CB_ERR_ARG;
	/* Check if this file is already open. If yes, just leave as there is
	   nothing left to do here. */
	if (!strncmp(current_hwi, hwi_filename, HWI_MAX_NAME_LEN)) {
		printk(BIOS_SPEW, "HWILIB: File \"%s\" already open.\n",
			hwi_filename);
		return CB_SUCCESS;
	}

	ptr = cbfs_map(hwi_filename, &filesize);
	if (!ptr) {
		printk(BIOS_ERR,"HWILIB: Missing file \"%s\" in cbfs.\n",
			hwi_filename);
		return CB_ERR;
	}
	/* Ensure the block has the right magic */
	if (strncmp((char*)ptr, BLOCK_MAGIC, LEN_MAGIC_NUM)) {
		printk(BIOS_ERR, "HWILIB: Bad magic at start of block!\n");
		return CB_ERR;
	}
	/* Reset all static pointers to blocks as they might have been set
	 *  in prior calls to this function.
	 * This way the caller do not need to "close" already opened blocks.
	 */
	memset(all_blocks, 0, (MAX_BLOCK_NUM * sizeof (uint8_t *)));
	/* Check which blocks are available by examining the length field. */
	base = ptr;
	/* Fill in sizes of all fixed length blocks. */
	all_blk_size[BLK_HIB] = LEN_HIB;
	all_blk_size[BLK_SIB] = LEN_SIB;
	all_blk_size[BLK_EIB] = LEN_EIB;
	/* Length of BLK_XIB is variable and will be filled if block is found */
	all_blk_size[BLK_XIB] = 0;
	while(!(strncmp((char *)ptr, BLOCK_MAGIC, LEN_MAGIC_NUM)) &&
		next_offset) {
		uint16_t len = read16(ptr + LEN_OFFSET);
		/* Ensure file size boundaries for a given block. */
		if ((ptr - base + len) > filesize)
			break;
		if (len == LEN_HIB) {
			all_blocks[BLK_HIB] = ptr;
			next_offset = read32(ptr + NEXT_OFFSET_HIB);
			if (next_offset)
				ptr = base + next_offset;
		} else if (len == LEN_SIB) {
			all_blocks[BLK_SIB] = ptr;
			next_offset = read32(ptr + NEXT_OFFSET_SIB);
			if (next_offset)
				ptr = base + next_offset;
		} else if (len == LEN_EIB) {
			/* Skip preliminary blocks */
			if (!(read16(ptr + EIB_FEATRUE_OFFSET) & 0x01))
				all_blocks[BLK_EIB] = ptr;
			next_offset = read32(ptr + NEXT_OFFSET_EIB);
			if (next_offset)
				ptr = base + next_offset;
		} else if (len >= MIN_LEN_XIB) {
			all_blocks[BLK_XIB] = ptr;
			next_offset = read32(ptr + NEXT_OFFSET_XIB);
			all_blk_size[BLK_XIB] = len;
			if (next_offset)
				ptr = base + next_offset;
		} else {
			next_offset = 0;
		}
	}
	/* We should have found at least one valid block */
	if (all_blocks[BLK_HIB] || all_blocks[BLK_SIB] || all_blocks[BLK_EIB] ||
	    all_blocks[BLK_XIB]) {
		/* Save currently opened hwi filename. */
		strncpy(current_hwi, hwi_filename, HWI_MAX_NAME_LEN);
		return CB_SUCCESS;
	}
	else
		return CB_ERR;
}

/** \brief This functions is used from caller to get a specific field from
 *         hwinfo block.
 * @param  field	Field type to read from hwinfo
 * @param  *dst		Pointer to memory where the data will be stored in
 * @param  dstsize	Size of the memory passed in via the *dst pointer
 * @return		Number of copied bytes on success, 0 on error
 */
uint32_t hwilib_get_field(hwinfo_field_t field, uint8_t *dst, size_t dstsize)
{
	/* Check the boundaries of params-variable */
	if ((uint32_t)field < ARRAY_SIZE(params))
		return params[field].get_field(&params[field], dst, dstsize);
	else
		return 0;
}
