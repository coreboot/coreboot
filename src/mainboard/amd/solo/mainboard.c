#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>


unsigned long initial_apicid[CONFIG_MAX_CPUS] =
{
	0
};
