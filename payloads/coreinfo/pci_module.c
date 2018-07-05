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
 */

#include <arch/io.h>
#include <pci.h>
#include <libpayload.h>
#include "coreinfo.h"

#if IS_ENABLED(CONFIG_MODULE_PCI)

struct pci_devices {
	pcidev_t device;
	unsigned int id;
};

#define MAX_PCI_DEVICES 64
static struct pci_devices devices[MAX_PCI_DEVICES];
static int devices_index;

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

static void show_config_space(WINDOW *win, int row, int col, int index)
{
	unsigned char cspace[256];
	pcidev_t dev;
	int i, x, y;

	dev = devices[index].device;

	for (i = 0; i < 256; i ++)
		cspace[i] = pci_read_config8(dev, i);

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++)
			mvwprintw(win, row + y, col + (x * 3), "%2.2X ",
				  cspace[(y * 16) + x]);
	}
}

static int pci_module_redraw(WINDOW *win)
{
	unsigned int bus, slot, func;
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

		bus = PCI_BUS(devices[item].device);
		slot = PCI_SLOT(devices[item].device);
		func = PCI_FUNC(devices[item].device);

		if (item == menu_selected)
			wattrset(win, COLOR_PAIR(3) | A_BOLD);
		else
			wattrset(win, COLOR_PAIR(2));

		mvwprintw(win, 2 + i, 1, "%X:%2.2X.%2.2X %04X:%04X  ",
			  bus, slot, func,
			  devices[item].id & 0xffff,
			  (devices[item].id >> 16) & 0xffff);

		wattrset(win, COLOR_PAIR(2));

		if (i == 0) {
			if (item != 0)
				mvwaddch(win, 2 + i, 19, ACS_UARROW);
		}
		if (i == MENU_VISIBLE - 1) {
			if ((item + 1) < devices_index)
				mvwaddch(win, 2 + i, 19, ACS_DARROW);
		}
	}

	wattrset(win, COLOR_PAIR(2));

	for (i = 0; i < 16; i++)
		mvwprintw(win, 2, 26 + (i * 3), "%2.2X ", i);

	wmove(win, 3, 25);

	for (i = 0; i < 48; i++)
		waddch(win, (i == 0) ? ACS_ULCORNER : ACS_HLINE);

	for (i = 0; i < 16; i++) {
		mvwprintw(win, 4 + i, 23, "%2.2X", i * 16);
		wmove(win, 4 + i, 25);
		waddch(win, ACS_VLINE);
	}

	show_config_space(win, 4, 26, menu_selected);

	return 0;
}

static void pci_scan_bus(int bus)
{
	int slot, func;
	unsigned int val;
	unsigned char hdr;

	for (slot = 0; slot < 0x20; slot++) {
		for (func = 0; func < 8; func++) {
			pcidev_t dev = PCI_DEV(bus, slot, func);

			val = pci_read_config32(dev, REG_VENDOR_ID);

			/* Nobody home. */
			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000) {

				/* If function 0 is not present, no need
				 * to test other functions. */
				if (func == 0)
					func = 8;
				continue;
			}

			/* FIXME: Remove this arbitrary limitation. */
			if (devices_index >= MAX_PCI_DEVICES)
				return;

			devices[devices_index].device =
			    PCI_DEV(bus, slot, func);

			devices[devices_index++].id = val;

			/* If this is a bridge, then follow it. */
			hdr = pci_read_config8(dev, REG_HEADER_TYPE);

			if ((func == 0) && !(hdr & HEADER_TYPE_MULTIFUNCTION))
				func = 8;

			hdr &= ~HEADER_TYPE_MULTIFUNCTION;
			if (hdr == HEADER_TYPE_BRIDGE ||
			    hdr == HEADER_TYPE_CARDBUS) {
				unsigned int busses;

				busses = pci_read_config32(dev, REG_PRIMARY_BUS);

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

#else

struct coreinfo_module pci_module = {
};

#endif
