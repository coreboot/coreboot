/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
   EN29F512 has 1C,21
   EN29F010 has 1C,20
   EN29F040A has 1C,04
   EN29LV010 has 1C,6E and uses short F0 reset sequence
   EN29LV040(A) has 1C,4F and uses short F0 reset sequence
 */
int probe_en29f512(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

	*(volatile uint8_t *)(bios + 0x555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AA) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0x90;

	myusec_delay(10);

	id1 = *(volatile uint8_t *)(bios + 0x100);
	id2 = *(volatile uint8_t *)(bios + 0x101);

	/* exit by writing F0 anywhere? or the code below */
	*(volatile uint8_t *)(bios + 0x555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AA) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0xF0;

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);

	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

/*
   EN29F002AT has 1C,92
   EN29F002AB has 1C,97
 */
int probe_en29f002a(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t id1, id2;

	*(volatile uint8_t *)(bios + 0x555) = 0xAA;
	*(volatile uint8_t *)(bios + 0xAAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0x90;

	myusec_delay(10);

	id1 = *(volatile uint8_t *)(bios + 0x100);
	id2 = *(volatile uint8_t *)(bios + 0x101);

	/* exit by writing F0 anywhere? or the code below */
	*(volatile uint8_t *)(bios + 0x555) = 0xAA;
	*(volatile uint8_t *)(bios + 0xAAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x555) = 0xF0;

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);

	if (id1 == flash->manufacture_id && id2 == flash->model_id)
		return 1;

	return 0;
}

