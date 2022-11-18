/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * All IO necessary to poke VGA registers.
 */
#include <pc80/vga_io.h>

#include <arch/io.h>

#define VGA_CR_INDEX          0x3D4
#define VGA_CR_VALUE          0x3D5

#define VGA_SR_INDEX          0x3C4
#define VGA_SR_VALUE          0x3C5

#define VGA_GR_INDEX          0x3CE
#define VGA_GR_VALUE          0x3CF

#define VGA_AR_INDEX          0x3C0
#define VGA_AR_VALUE_READ     0x3C1
#define VGA_AR_VALUE_WRITE    VGA_AR_INDEX

#define VGA_MISC_WRITE        0x3C2
#define VGA_MISC_READ         0x3CC

#define VGA_ENABLE            0x3C3
#define VGA_STAT1             0x3DA

#define VGA_DAC_MASK          0x3C6
#define VGA_DAC_READ_ADDRESS  0x3C7
#define VGA_DAC_WRITE_ADDRESS 0x3C8
#define VGA_DAC_DATA          0x3C9

/*
 * VGA enable. Poke this to have the PCI IO enabled device accept VGA IO.
 */
unsigned char
vga_enable_read(void)
{
	return inb(VGA_ENABLE);
}

void
vga_enable_write(unsigned char value)
{
	outb(value, VGA_ENABLE);
}

void
vga_enable_mask(unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_enable_read();
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_enable_write(tmp);
}

/*
 * Miscellaneous register.
 */
unsigned char
vga_misc_read(void)
{
	return inb(VGA_MISC_READ);
}

void
vga_misc_write(unsigned char value)
{
	outb(value, VGA_MISC_WRITE);
}

void
vga_misc_mask(unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_misc_read();
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_misc_write(tmp);
}

/*
 * Sequencer registers.
 */
unsigned char
vga_sr_read(unsigned char index)
{
	outb(index, VGA_SR_INDEX);
	return (inb(VGA_SR_VALUE));
}

void
vga_sr_write(unsigned char index, unsigned char value)
{
	outb(index, VGA_SR_INDEX);
	outb(value, VGA_SR_VALUE);
}

void
vga_sr_mask(unsigned char index, unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_sr_read(index);
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_sr_write(index, tmp);
}

/*
 * CRTC registers.
 */
unsigned char
vga_cr_read(unsigned char index)
{
	outb(index, VGA_CR_INDEX);
	return (inb(VGA_CR_VALUE));
}

void
vga_cr_write(unsigned char index, unsigned char value)
{
	outb(index, VGA_CR_INDEX);
	outb(value, VGA_CR_VALUE);
}

void
vga_cr_mask(unsigned char index, unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_cr_read(index);
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_cr_write(index, tmp);
}

/*
 * Attribute registers.
 */
unsigned char
vga_ar_read(unsigned char index)
{
	unsigned char ret;

	(void)inb(VGA_STAT1);
	outb(index, VGA_AR_INDEX);
	ret = inb(VGA_AR_VALUE_READ);
	(void)inb(VGA_STAT1);

	return ret;
}

void
vga_ar_write(unsigned char index, unsigned char value)
{
	(void)inb(VGA_STAT1);
	outb(index, VGA_AR_INDEX);
	outb(value, VGA_AR_VALUE_WRITE);
	(void)inb(VGA_STAT1);
}

void
vga_ar_mask(unsigned char index, unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_ar_read(index);
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_ar_write(index, tmp);
}

/*
 * Graphics registers.
 */
unsigned char
vga_gr_read(unsigned char index)
{
	outb(index, VGA_GR_INDEX);
	return (inb(VGA_GR_VALUE));
}

void
vga_gr_write(unsigned char index, unsigned char value)
{
	outb(index, VGA_GR_INDEX);
	outb(value, VGA_GR_VALUE);
}

void
vga_gr_mask(unsigned char index, unsigned char value, unsigned char mask)
{
	unsigned char tmp;

	tmp = vga_gr_read(index);
	tmp &= ~mask;
	tmp |= (value & mask);
	vga_gr_write(index, tmp);
}

/*
 * DAC functions.
 */
void
vga_palette_enable(void)
{
	(void)inb(VGA_STAT1);
	outb(0x00, VGA_AR_INDEX);
	(void)inb(VGA_STAT1);
}

void
vga_palette_disable(void)
{
	(void)inb(VGA_STAT1);
	outb(0x20, VGA_AR_INDEX);
	(void)inb(VGA_STAT1);
}

unsigned char
vga_dac_mask_read(void)
{
	return inb(VGA_DAC_MASK);
}

void
vga_dac_mask_write(unsigned char mask)
{
	outb(mask, VGA_DAC_MASK);
}

void
vga_dac_read_address(unsigned char address)
{
	outb(address, VGA_DAC_READ_ADDRESS);
}

void
vga_dac_write_address(unsigned char address)
{
	outb(address, VGA_DAC_WRITE_ADDRESS);
}

unsigned char
vga_dac_data_read(void)
{
	return inb(VGA_DAC_DATA);
}

void
vga_dac_data_write(unsigned char data)
{
	outb(data, VGA_DAC_DATA);
}
