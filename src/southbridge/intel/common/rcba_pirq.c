#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <southbridge/intel/common/rcba.h>

#define MAX_SLOT 31
#define MIN_SLOT 19

static const u32 pirq_dir_route_reg[MAX_SLOT - MIN_SLOT + 1] = {
	D19IR, D20IR, D21IR, D22IR, D23IR, 0, D25IR,
	D26IR, D27IR, D28IR, D29IR, D30IR, D31IR,
};

enum pirq intel_common_map_pirq(const device_t dev, const pci_pin_t pci_pin)
{
	u8 slot = PCI_SLOT(dev->path.pci.devfn);
	u8 shift = 4 * (pci_pin - PCI_INT_A);
	u8 pirq;
	u16 reg;

	if (pci_pin < 1 || pci_pin > 4) {
		printk(BIOS_ERR, "Slot %d PCI pin %d out of bounds\n",
			slot, pci_pin);
		return PIRQ_NONE;
	}

	if (slot < MIN_SLOT || slot > MAX_SLOT) {
		/* non-PCH devices use 1:1 mapping. */
		return pci_pin;
	}

	reg = pirq_dir_route_reg[slot - MIN_SLOT];

	pirq = ((RCBA16(reg) >> shift) & 0xf);
	if (pirq > 8) {
		printk(BIOS_ERR, "Reg 0x%04x PIRQ %c out of bounds\n",
			reg, 'A' + pirq);
		return PIRQ_NONE;
	}
	return PIRQ_A + pirq;
}
