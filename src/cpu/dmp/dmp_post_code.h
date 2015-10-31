/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef DMP_POST_CODE_H
#define DMP_POST_CODE_H

/* DMP Vortex86EX specific POST codes */

#define POST_DMP_KBD_FW_UPLOAD			0x06
#define POST_DMP_KBD_CHK_READY			0x07
#define POST_DMP_KBD_IS_READY			0x08
#define POST_DMP_KBD_IS_BAD			0x09
#define POST_DMP_KBD_FW_VERIFY_ERR		0x82
#define POST_DMP_ID_ERR				0x85
#define POST_DMP_DRAM_TEST_ERR			0x86
#define POST_DMP_DRAM_SIZING_ERR		0x77

#endif /* DMP_POST_CODE_H*/
