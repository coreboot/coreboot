/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SANDYBRIDGE_REGISTERS_DMIBAR_H__
#define __SANDYBRIDGE_REGISTERS_DMIBAR_H__

#define DMIVCECH	0x000	/* 32bit */
#define DMIPVCCAP1	0x004	/* 32bit */
#define DMIPVCCAP2	0x008	/* 32bit */
#define DMIPVCCCTL	0x00c	/* 16bit */

#define DMIVC0RCAP	0x010	/* 32bit */
#define DMIVC0RCTL	0x014	/* 32bit */
#define DMIVC0RSTS	0x01a	/* 16bit */
#define  VC0NP		(1 << 1)

#define DMIVC1RCAP	0x01c	/* 32bit */
#define DMIVC1RCTL	0x020	/* 32bit */
#define DMIVC1RSTS	0x026	/* 16bit */
#define  VC1NP		(1 << 1)

#define DMIVCPRCAP	0x028	/* 32bit */
#define DMIVCPRCTL	0x02c	/* 32bit */
#define DMIVCPRSTS	0x032	/* 16bit */
#define  VCPNP		(1 << 1)

#define DMIVCMRCAP	0x034	/* 32bit */
#define DMIVCMRCTL	0x038	/* 32bit */
#define DMIVCMRSTS	0x03e	/* 16bit */
#define  VCMNP		(1 << 1)

#define DMIRCLDECH	0x040	/* 32bit */
#define DMIESD		0x044	/* 32bit */

#define DMILE1D		0x050	/* 32bit */
#define DMILE1A		0x058	/* 64bit */
#define DMILE2D		0x060	/* 32bit */
#define DMILE2A		0x068	/* 64bit */

#define DMILCAP		0x084	/* 32bit */
#define DMILCTL		0x088	/* 16bit */
#define DMILSTS		0x08a	/* 16bit */
#define  TXTRN		(1 << 11)

#define DMILCTL2	0x098	/* 16bit */
#define DMILSTS2	0x09a	/* 16bit */

#define DMIUESTS	0x1c4	/* 32bit */
#define DMICESTS	0x1d0	/* 32bit */

#define DMIL0SLAT	0x22c	/* 32bit */
#define DMILLTC		0x238	/* 32bit */

#endif /* __SANDYBRIDGE_REGISTERS_DMIBAR_H__ */
