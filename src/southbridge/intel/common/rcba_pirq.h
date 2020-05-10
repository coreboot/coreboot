/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_RCBA_PIRQ_H
#define SOUTHBRIDGE_INTEL_COMMON_RCBA_PIRQ_H

/*
 * The DnnIR registers use common RCBA offsets across these chipsets:
 * bd82x6x, i82801, i89xx, ibexpeak, lynxpoint
 *
 * However not all registers are in use on all of these.
 */

#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D23IR		0x3158	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D21IR		0x3164	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define D19IR		0x3168	/* 16bit */

#endif /* SOUTHBRIDGE_INTEL_COMMON_RCBA_PIRQ_H */
