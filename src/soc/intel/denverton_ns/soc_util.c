/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/device.h>
#include <string.h>
#include <soc/iomap.h>
#include <soc/soc_util.h>
#include <soc/pmc.h>
#include <soc/smbus.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

#ifdef __SIMPLE_DEVICE__
pci_devfn_t get_hostbridge_dev(void)
{
	return PCI_DEV(0, SA_DEV, SA_FUNC);
}
#else
struct device *get_hostbridge_dev(void)
{
	return pcidev_on_root(SA_DEV, SA_FUNC);
}
#endif

#ifdef __SIMPLE_DEVICE__
pci_devfn_t get_lpc_dev(void)
{
	return PCI_DEV(0, LPC_DEV, LPC_FUNC);
}
#else
struct device *get_lpc_dev(void)
{
	return pcidev_on_root(LPC_DEV, LPC_FUNC);
}
#endif

#ifdef __SIMPLE_DEVICE__
pci_devfn_t get_pmc_dev(void)
{
	return PCI_DEV(0, PMC_DEV, PMC_FUNC);
}
#else
struct device *get_pmc_dev(void)
{
	return pcidev_on_root(PMC_DEV, PMC_FUNC);
}
#endif

#ifdef __SIMPLE_DEVICE__
pci_devfn_t get_smbus_dev(void)
{
	return PCI_DEV(0, SMBUS_DEV, SMBUS_FUNC);
}
#else
struct device *get_smbus_dev(void)
{
	return pcidev_on_root(SMBUS_DEV, SMBUS_FUNC);
}
#endif

uint32_t get_pciebase(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	u32 pciexbar_reg;

	dev = get_hostbridge_dev();
	if (!dev)
		return 0;

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch (pciexbar_reg & MASK_PCIEXBAR_LENGTH) {
	case MASK_PCIEXBAR_LENGTH_256M:
		pciexbar_reg &= MASK_PCIEXBAR_256M;
		break;
	case MASK_PCIEXBAR_LENGTH_128M:
		pciexbar_reg &= MASK_PCIEXBAR_128M;
		break;
	case MASK_PCIEXBAR_LENGTH_64M:
		pciexbar_reg &= MASK_PCIEXBAR_64M;
		break;
	default:
		pciexbar_reg &= MASK_PCIEXBAR_256M;
		break;
	}

	return pciexbar_reg;
}

uint32_t get_pcielength(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	u32 pciexbar_reg;

	dev = get_hostbridge_dev();
	if (!dev)
		return 0;

	pciexbar_reg = pci_read_config32(dev, PCIEXBAR);

	if (!(pciexbar_reg & (1 << 0)))
		return 0;

	switch (pciexbar_reg & MASK_PCIEXBAR_LENGTH) {
	case MASK_PCIEXBAR_LENGTH_256M:
		pciexbar_reg = 256;
		break;
	case MASK_PCIEXBAR_LENGTH_128M:
		pciexbar_reg = 128;
		break;
	case MASK_PCIEXBAR_LENGTH_64M:
		pciexbar_reg = 64;
		break;
	default:
		pciexbar_reg = 64;
		break;
	}

	return pciexbar_reg;
}

uint32_t get_tseg_memory(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_hostbridge_dev();

	if (!dev)
		return 0;

	return pci_read_config32(dev, TSEGMB) & MASK_TSEGMB;
}

uint32_t get_top_of_low_memory(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_hostbridge_dev();

	if (!dev)
		return 0;

	return pci_read_config32(dev, TOLUD) & MASK_TOLUD;
}

uint64_t get_top_of_upper_memory(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_hostbridge_dev();

	if (!dev)
		return 0;

	return ((uint64_t)(pci_read_config32(dev, TOUUD_HI) & MASK_TOUUD_HI)
		<< 32) +
	       (uint64_t)(pci_read_config32(dev, TOUUD_LO) & MASK_TOUUD_LO);
}

uint16_t get_pmbase(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_pmc_dev();

	if (!dev)
		return 0;

	return pci_read_config16(dev, PMC_ACPI_BASE) & 0xfff8;
}

uint16_t get_tcobase(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_smbus_dev();

	if (!dev)
		return 0;

	return pci_read_config16(dev, TCOBASE) & MASK_TCOBASE;
}

void mmio_andthenor32(void *addr, uint32_t val2and, uint32_t val2or)
{
	uint32_t reg32;

	reg32 = read32(addr);
	reg32 &= (uint32_t)val2and;
	reg32 |= (uint32_t)val2or;
	write32(addr, reg32);
}

uint8_t silicon_stepping(void)
{
	uint8_t revision_id;
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	dev = get_lpc_dev();

	if (!dev)
		return 0;

	revision_id = pci_read_config8(dev, PCI_REVISION_ID);

	return revision_id;
}

void *memcpy_s(void *dest, const void *src, size_t n)
{
	uint8_t *dp;
	const uint8_t *sp;

	dp = (uint8_t *)dest;
	sp = (uint8_t *)src;

	if (!n)
		return dest;

	if (n > UINT32_MAX)
		return dest;

	if (!dp)
		return dest;

	if (!sp)
		return dest;

	/*
	 * overlap is undefined behavior, do not allow
	 */
	if (((dp > sp) && (dp < (sp + n))) || ((sp > dp) && (sp < (dp + n))))
		return dest;

	/*
	 * now perform the copy
	 */

	/* Original memcpy() function */
	unsigned long d0, d1, d2;

	asm volatile(
#if ENV_X86_64
		"rep ; movsd\n\t"
		"mov %4,%%rcx\n\t"
#else
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
#endif
		"rep ; movsb\n\t"
		: "=&c"(d0), "=&D"(d1), "=&S"(d2)
		: "0"(n >> 2), "g"(n & 3), "1"(dest), "2"(src)
		: "memory");

	return dest;
}
