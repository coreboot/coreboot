/*
 * This file is part of the coreinfo project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <arch/io.h>
#include "coreinfo.h"

struct pci_devices {
	unsigned short device;
	unsigned int id;
};

static struct pci_devices devices[64];
static int devices_index;

#define REG_VENDOR_ID   0x00
#define REG_HEADER_TYPE 0x0e
#define REG_PRIMARY_BUS 0x18

#define HEADER_TYPE_NORMAL  0
#define HEADER_TYPE_BRIDGE  1
#define HEADER_TYPE_CARDBUS 2

#define PCI_ADDR(_bus, _dev, _reg) \
	(0x80000000 | (_bus << 16) | (_dev << 8) | (_reg & ~3))

/* Number of entries to show in the list */
#define MENU_VISIBLE 16

static int menu_selected = 0;
static int menu_first = 0;

static void swap(struct pci_devices *a, struct pci_devices *b)
{
	struct pci_devices tmp;

	tmp.device = a->device;
	tmp.id = a->id;

	a->device = b->device;
	a->id = b->id;

	b->device = tmp.device;
	b->id = tmp.id;
}

static int partition(struct pci_devices *list, int len)
{
	int val = list[len / 2].device;
	int index = 0;
	int i;

	swap(&list[len / 2], &list[len - 1]);

	for (i = 0; i < len - 1; i++) {
		if (list[i].device < val) {
			swap(&list[i], &list[index]);
			index++;
		}
	}

	swap(&list[index], &list[len - 1]);

	return index;
}

static void quicksort(struct pci_devices *list, int len)
{
	int index;

	if (len <= 1)
		return;

	index = partition(list, len);

	quicksort(list, index);
	quicksort(&(list[index]), len - index);
}

static void pci_read_dword(unsigned int bus, unsigned int devfn,
			   unsigned int reg, unsigned int *val)
{
	outl(PCI_ADDR(bus, devfn, reg), 0xcf8);
	*val = inl(0xcfc);
}

static void pci_read_byte(unsigned int bus, unsigned int devfn,
			  unsigned int reg, unsigned char *val)
{
	outl(PCI_ADDR(bus, devfn, reg), 0xcf8);
	*val = inb(0xcfc + (reg & 3));
}

static void show_config_space(WINDOW *win, int row, int col, int index)
{
	unsigned char cspace[64];
	int bus, devfn;
	int i, x, y;

	bus = (devices[index].device >> 8) & 0xff;
	devfn = devices[index].device & 0xff;

	for (i = 0; i < 64; i += 4)
		pci_read_dword(bus, devfn, i, ((unsigned int *)&cspace[i]));

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 16; x++)
			mvwprintw(win, row + y, col + (x * 3), "%2.2X ",
				  cspace[(y * 16) + x]);
	}
}

static int pci_module_redraw(WINDOW *win)
{
	unsigned int bus, devfn, func;
	int i, last;

	print_module_title(win, "PCI Device List");

	last = menu_first + MENU_VISIBLE;

	if (last > devices_index)
		last = devices_index;

	for (i = 0; i < MENU_VISIBLE; i++) {
		int item = menu_first + i;

		/* Draw a blank space. */
		if (item >= devices_index) {
			wattrset(win, COLOR_PAIR(2));
			mvwprintw(win, 2 + i, 1, "                 ");
			continue;
		}

		bus = (devices[item].device >> 8) & 0xff;
		devfn = (devices[item].device & 0xff) / 8;
		func = (devices[item].device & 0xff) % 8;

		if (item == menu_selected)
			wattrset(win, COLOR_PAIR(3) | A_BOLD);
		else
			wattrset(win, COLOR_PAIR(2));

		mvwprintw(win, 2 + i, 1, "%X:%2.2X.%2.2X %X:%X  ",
			  bus, devfn, func,
			  devices[item].id & 0xffff,
			  (devices[item].id >> 16) & 0xffff);

		wattrset(win, COLOR_PAIR(2));

		if (i == 0) {
			if (item != 0)
				mvwprintw(win, 2 + i, 19, "\30");
		}
		if (i == MENU_VISIBLE - 1) {
			if ((item + 1) < devices_index)
				mvwprintw(win, 2 + i, 19, "\31");
		}
	}

	wattrset(win, COLOR_PAIR(2));

	for (i = 0; i < 16; i++)
		mvwprintw(win, 2, 26 + (i * 3), "%2.2X ", i);

	wmove(win, 3, 25);

	for (i = 0; i < 48; i++)
		waddch(win, (i == 0) ? '\332' : '\304');

	for (i = 0; i < 4; i++) {
		mvwprintw(win, 4 + i, 23, "%2.2X", i * 16);
		wmove(win, 4 + i, 25);
		waddch(win, '\263');
	}

	show_config_space(win, 4, 26, menu_selected);

	return 0;
}

static void pci_scan_bus(int bus)
{
	int devfn, func;
	unsigned int val;
	unsigned char hdr;

	for (devfn = 0; devfn < 0x100;) {
		for (func = 0; func < 8; func++, devfn++) {
			pci_read_dword(bus, devfn, REG_VENDOR_ID, &val);

			/* Nobody home. */
			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* FIXME: Remove this arbitrary limitation. */
			if (devices_index >= 64)
				return;

			devices[devices_index].device =
			    ((bus & 0xff) << 8) | (devfn & 0xff);

			devices[devices_index++].id = val;

			/* If this is a bridge, then follow it. */
			pci_read_byte(bus, devfn, REG_HEADER_TYPE, &hdr);
			hdr &= 0x7f;
			if (hdr == HEADER_TYPE_BRIDGE ||
			    hdr == HEADER_TYPE_CARDBUS) {
				unsigned int busses;

				pci_read_dword(bus, devfn, REG_PRIMARY_BUS,
					       &busses);

				pci_scan_bus((busses >> 8) & 0xff);

			}
		}
	}

	quicksort(devices, devices_index);
}

static int pci_module_handle(int key)
{
	int ret = 0;

	switch (key) {
	case KEY_DOWN:
		if (menu_selected + 1 < devices_index) {
			menu_selected++;
			ret = 1;
		}
		break;
	case KEY_UP:
		if (menu_selected > 0) {
			menu_selected--;
			ret = 1;
		}
		break;
	}

	if (!ret)
		return ret;

	if (menu_selected < menu_first)
		menu_first = menu_selected;
	else if (menu_selected >= menu_first + MENU_VISIBLE) {
		menu_first = menu_selected - (MENU_VISIBLE - 1);
		if (menu_first < 0)
			menu_first = 0;
	}

	return ret;
}

static int pci_module_init(void)
{
	pci_scan_bus(0);
	return 0;
}

struct coreinfo_module pci_module = {
	.name = "PCI",
	.init = pci_module_init,
	.redraw = pci_module_redraw,
	.handle = pci_module_handle,
};
