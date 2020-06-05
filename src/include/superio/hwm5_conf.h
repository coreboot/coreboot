/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DEVICE_PNP_HWM5_CONF_H
#define DEVICE_PNP_HWM5_CONF_H

#include <device/pnp.h>
#include <stdint.h>

/* The address/data register pair for the indirect/indexed IO space of the
 * hardware monitor (HWM) that does temperature and voltage sensing and fan
 * control in ITE, Nuvoton and Winbond super IO chips aren't at offset 0 and 1
 * of the corresponding IO address region, but at offset 5 and 6. */

/*
 * u8 pnp_read_hwm5_index(u16 base, u8 reg)
 * Description:
 *  This routine reads indexed I/O registers. The reg byte is written
 *  to the index register at I/O address = base + 5. The result is then
 *  read from the data register at I/O address = base + 6.
 *
 * Parameters:
 *  @param[in]  u16 base   = The I/O address of the base index register.
 *  @param[in]  u8  reg    = The offset within the indexed space.
 *  @param[out] u8  result = The value read back from the data register.
 */
static inline u8 pnp_read_hwm5_index(u16 base, u8 reg)
{
	return pnp_read_index(base + 5, reg);
}

/*
 * void pnp_write_hwm5_index(u16 base, u8 reg, u8 value)
 * Description:
 *  This routine writes indexed I/O registers. The reg byte is written
 *  to the index register at I/O address = base + 5. The value byte is then
 *  written to the data register at I/O address = base + 6.
 *
 * Parameters:
 *  @param[in] u16 base   = The address of the base index register.
 *  @param[in] u8  reg    = The offset within the indexed space.
 *  @param[in] u8  value  = The value to be written to the data register.
 */
static inline void pnp_write_hwm5_index(u16 base, u8 reg, u8 value)
{
	pnp_write_index(base + 5, reg, value);
}

/*
 * void pnp_unset_and_set_hwm5_index(u16 base, u8 reg, u8 unset, u8 set)
 * Description:
 *  This routine unsets and sets bits from indexed I/O registers. The
 *  reg byte is written to the index register at I/O address = base + 5.
 *  The value byte to update is data register at I/O address = base + 6.
 *
 *  Unlike and-then-or style operations, no bitwise negation is necessary
 *  to specify the bits to unset. Because the bitwise negation implicitly
 *  promotes operands to int before operating, one may have to explicitly
 *  downcast the result if the data width is smaller than that of an int.
 *  Since warnings are errors in coreboot, explicit casting is necessary.
 *
 *  Performing said negation inside this routine alleviates this problem,
 *  while allowing the compiler to warn if the input parameters overflow.
 *  Casting outside this function would silence valid compiler warnings.
 *
 * Parameters:
 *  @param[in] u16 base   = The address of the base index register.
 *  @param[in] u8  reg    = The offset within the indexed space.
 *  @param[in] u8  unset  = Bitmask with ones to the bits to unset from the data register.
 *  @param[in] u8  set    = Bitmask with ones to the bits to set from the data register.
 */
static inline void pnp_unset_and_set_hwm5_index(u16 base, u8 reg, u8 unset, u8 set)
{
	pnp_unset_and_set_index(base + 5, reg, unset, set);
}

#endif /* DEVICE_PNP_HWM5_CONF_H */
