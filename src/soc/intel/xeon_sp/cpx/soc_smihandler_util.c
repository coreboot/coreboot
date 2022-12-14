/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pci_devs.h>

pci_devfn_t soc_get_ubox_pmon_dev(void)
{
	return UBOX_DEV_PMON;
}
