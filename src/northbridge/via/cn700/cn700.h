/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if !defined (__PRE_RAM__)
// HACK
static inline void cn700_noop(device_t dev)
{
}
#endif

/* VGA stuff */
#define SR_INDEX		0x3c4
#define SR_DATA			0x3c5
#define CRTM_INDEX		0x3b4
#define CRTM_DATA		0x3b5
#define CRTC_INDEX		0x3d4
#define CRTC_DATA		0x3d5

/* Memory controller registers */
#define RANK0_END		0x40
#define RANK1_END		0x41
#define RANK2_END		0x42
#define RANK3_END		0x43
#define RANK0_START		0x48
#define RANK1_START		0x49
#define RANK2_START		0x4a
#define RANK3_START		0x4b
#define DDR_PAGE_CTL		0x69
#define DRAM_REFRESH_COUNTER	0x6a
#define DRAM_MISC_CTL		0x6b
#define CH_A_DQS_OUTPUT_DELAY	0x70
#define CH_A_MD_OUTPUT_DELAY	0x71

/* RAM init commands */
#define RAM_COMMAND_NORMAL	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_CBR		0x4
