/*
 * viatool - dump all registers on a VIA CPU + chipset based system.
 *
 * Copyright (C) 2013 Alexandru Gagniuc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * a long with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quirks.h"
#include <viatool.h>
#include <stdio.h>
#include <stddef.h>

extern struct quirk_list vx900_sb_quirk_list;

struct quirk_list *sb_quirks[] = {
	&vx900_sb_quirk_list,
	0,
};

struct quirk_list *nb_quirks[] = {
	0,
};

int print_quirks(struct pci_dev *sb, struct pci_access *pacc,
		 struct quirk_list **qlists);

int print_quirks_north(struct pci_dev *nb, struct pci_access *pacc)
{
	printf("\n====== Northbridge Quirks =======\n\n");
	return print_quirks(nb, pacc, nb_quirks);
}

int print_quirks_south(struct pci_dev *sb, struct pci_access *pacc)
{
	printf("\n====== Southbridge Quirks =======\n\n");
	return print_quirks(sb, pacc, sb_quirks);
}

int print_quirks(struct pci_dev *sb, struct pci_access *pacc,
		 struct quirk_list **qlists)
{
	size_t i, j;
	struct quirk *q;
	struct quirk_list *qlist;
	struct pci_dev *dev;

	for (i = 0; ; i++)
	{
		qlist = qlists[i];

		if (qlist == NULL) {
			/* OOPS. We've tried all we know, but no quirk */
			printf("No quirks supported.\n");
			break;
		}

		/* Is this the right device ? */
		if ( (qlist->pci_vendor_id != sb->vendor_id) ||
			qlist->pci_device_id != sb->device_id)
			continue;

		for (j = 0; ; j++)
		{
			q = &qlist->dev_quirks[j];

			if(q->pci_device_id == 0)
				break;

			printf("Probing PCI device %i:%.2x.%i\n",
			       q->pci_bus, q->pci_dev, q->pci_func);

			dev = pci_get_dev(pacc, q->pci_domain, q->pci_bus,
					  q->pci_dev, q->pci_func);

			if (!dev) {
				perror("Error: no device found\n");
				continue;
			}

			pci_fill_info(dev, PCI_FILL_IDENT |
					   PCI_FILL_BASES |
					   PCI_FILL_SIZES |
					   PCI_FILL_CLASS );

			if (dev->device_id != q->pci_device_id) {
				printf("Expected %.4x:%.4x, got %.4x:%.4x\n",
				       q->pci_vendor_id, q->pci_device_id,
				       dev->vendor_id, dev->device_id);
				continue;
			}

			if (!q->quirk_func) {
				perror("BUG: Quirk missing.\n");
				continue;
			}

			q->quirk_func(dev);
			/* On to next quirk */
		}

		/* Done. No need to go through the remainder of the list */
		break;
	}

	return 0;
}
