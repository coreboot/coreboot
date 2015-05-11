/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ARM_TF_TEMP_H__
#define __ARM_TF_TEMP_H__

#include <types.h>

/*
 * Code temporarily copied from arm-trusted-firmware/include/common/bl_common.h,
 * since it tries to pull in a few too many standard C headers and needs to be
 * cleaned up a bit before we can include it directly.
 */

#define PARAM_EP_SECURE		0x0
#define PARAM_EP_NON_SECURE	0x1
#define PARAM_EP_SECURITY_MASK	0x1

#define PARAM_EP_EE_MASK	0x2
#define PARAM_EP_EE_LITTLE	0x0
#define PARAM_EP_EE_BIG		0x2

#define PARAM_EP_ST_MASK	0x4
#define PARAM_EP_ST_DISABLE	0x0
#define PARAM_EP_ST_ENABLE	0x4

#define PARAM_EP		0x01
#define PARAM_IMAGE_BINARY	0x02
#define PARAM_BL31		0x03

#define VERSION_1		0x01

#define SET_PARAM_HEAD(_p, _type, _ver, _attr) do { \
	(_p)->h.type = (uint8_t)(_type); \
	(_p)->h.version = (uint8_t)(_ver); \
	(_p)->h.size = (uint16_t)sizeof(*_p); \
	(_p)->h.attr = (uint32_t)(_attr) ; \
	} while (0)

typedef struct aapcs64_params {
	unsigned long arg0;
	unsigned long arg1;
	unsigned long arg2;
	unsigned long arg3;
	unsigned long arg4;
	unsigned long arg5;
	unsigned long arg6;
	unsigned long arg7;
} aapcs64_params_t;

typedef struct param_header {
	uint8_t type;		/* type of the structure */
	uint8_t version;    /* version of this structure */
	uint16_t size;      /* size of this structure in bytes */
	uint32_t attr;      /* attributes: unused bits SBZ */
} param_header_t;

typedef struct entry_point_info {
	param_header_t h;
	uintptr_t pc;
	uint32_t spsr;
	aapcs64_params_t args;
} entry_point_info_t;

typedef struct image_info {
	param_header_t h;
	uintptr_t image_base;   /* physical address of base of image */
	uint32_t image_size;    /* bytes read from image file */
} image_info_t;

typedef struct bl31_params {
	param_header_t h;
	image_info_t *bl31_image_info;
	entry_point_info_t *bl32_ep_info;
	image_info_t *bl32_image_info;
	entry_point_info_t *bl33_ep_info;
	image_info_t *bl33_image_info;
} bl31_params_t;

#endif /* __ARM_TF_H__ */
