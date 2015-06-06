/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SOC_BROADCOM_CYGNUS_REG_UTILS__
#define __SOC_BROADCOM_CYGNUS_REG_UTILS__

/* ---- Include Files ---------------------------------------------------- */

#include <stdint.h>

/* ---- Public Constants and Types --------------------------------------- */

#define __REG32(x)      (*((volatile uint32_t *)(x)))
#define __REG16(x)      (*((volatile uint16_t *)(x)))
#define __REG8(x)       (*((volatile uint8_t *) (x)))

/* ---- Public Variable Externs ------------------------------------------ */
/* ---- Public Function Prototypes --------------------------------------- */

/****************************************************************************/
/*
 *   32-bit register access functions
 */
/****************************************************************************/

extern uint32_t reg_debug;
#define	REG_DEBUG(val) (reg_debug = val)

static inline void
reg32_clear_bits(volatile uint32_t *reg, uint32_t value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __FUNCTION__, (uint32_t)reg, *reg, (*reg & ~(value)));
#endif
	*reg &= ~(value);
}

static inline void
reg32_set_bits(volatile uint32_t *reg, uint32_t value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __FUNCTION__, (uint32_t)reg, *reg, (*reg | value));
#endif
	*reg |= value;
}

static inline void
reg32_toggle_bits(volatile uint32_t *reg, uint32_t value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __FUNCTION__, (uint32_t)reg, *reg, (*reg ^ value));
#endif
	*reg ^= value;
}

static inline void
reg32_write_masked(volatile uint32_t *reg, uint32_t mask, uint32_t value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __FUNCTION__, (uint32_t)reg, *reg, (*reg & (~mask)) | (value & mask));
#endif
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg32_write(volatile uint32_t *reg, uint32_t value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x, 0x%x)\n", __FUNCTION__, (uint32_t)reg, value);
#endif
	*reg = value;
}

static inline uint32_t
reg32_read(volatile uint32_t *reg)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x\n", __FUNCTION__, (uint32_t)reg, *reg);
#endif
	return *reg;
}

/****************************************************************************/
/*
 *   16-bit register access functions
 */
/****************************************************************************/

static inline void
reg16_clear_bits(volatile uint16_t *reg, uint16_t value)
{
	*reg &= ~(value);
}

static inline void
reg16_set_bits(volatile uint16_t *reg, uint16_t value)
{
	*reg |= value;
}

static inline void
reg16_toggle_bits(volatile uint16_t *reg, uint16_t value)
{
	*reg ^= value;
}

static inline void
reg16_write_masked(volatile uint16_t *reg, uint16_t mask, uint16_t value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void
reg16_write(volatile uint16_t *reg, uint16_t value)
{
	*reg = value;
}

static inline uint16_t
reg16_read(volatile uint16_t *reg)
{
	return *reg;
}

/****************************************************************************/
/*
 *   8-bit register access functions
 */
/****************************************************************************/

static inline void
reg8_clear_bits(volatile uint8_t *reg, uint8_t value)
{
	*reg &= ~(value);
}

static inline void
reg8_set_bits(volatile uint8_t *reg, uint8_t value)
{
	*reg |= value;
}

static inline void
reg8_toggle_bits(volatile uint8_t *reg, uint8_t value)
{
	*reg ^= value;
}

static inline void
reg8_write_masked(volatile uint8_t *reg, uint8_t mask, uint8_t value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void
reg8_write(volatile uint8_t *reg, uint8_t value)
{
	*reg = value;
}

static inline uint8_t
reg8_read(volatile uint8_t *reg)
{
	return *reg;
}
#endif /* __SOC_BROADCOM_CYGNUS_REG_UTILS__ */
