/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>

void itss_irq_init(const uint8_t pch_interrupt_routing[PIRQ_COUNT])
{
	uint32_t regs[PIRQ_COUNT/sizeof(uint32_t)] = {0};
	uint8_t index, byte;

	/* Fill in all the PIRx routes into one array. */
	for (index = 0; index < ARRAY_SIZE(regs); index++) {
		for (byte = 0; byte < sizeof(uint32_t); byte++) {
			uint8_t val = pch_interrupt_routing[index *
						    sizeof(uint32_t) + byte];
			uint8_t irq = val & 0xf;

			if ((irq <= 2) || (irq == 8) || (irq == 13))
				regs[index] |= (0x80 << (8 * byte));
			else
				regs[index] |= (val << (8 * byte));
		}
		/* Access the routing register in 32 bit mode to make this
		   function suitable for both IOSF 1.0 (where only 32 bit access
		   is supported) and later versions of the interface. */
		pcr_write32(PID_ITSS,
			    PCR_ITSS_PIRQA_ROUT + (index * sizeof(uint32_t)),
			    regs[index]);
	}
}

void itss_clock_gate_8254(void)
{
	const uint32_t cge8254_mask = (1 << 2);

	pcr_rmw32(PID_ITSS, PCR_ITSS_ITSSPRC, ~cge8254_mask, cge8254_mask);
}

void itss_set_irq_polarity(int irq, int active_low)
{
	uint32_t mask;
	uint16_t reg;
	const uint16_t port = PID_ITSS;

	if (irq < 0 || irq > ITSS_MAX_IRQ)
		return;

	reg = PCR_ITSS_IPC0_CONF + sizeof(uint32_t) * (irq / IRQS_PER_IPC);
	mask = 1 << (irq % IRQS_PER_IPC);

	pcr_rmw32(port, reg, ~mask, (active_low ? mask : 0));
}

static uint32_t irq_snapshot[NUM_IPC_REGS];

void itss_snapshot_irq_polarities(int start, int end)
{
	int i;
	int reg_start;
	int reg_end;
	const uint16_t port = PID_ITSS;

	if (start < 0 || start > ITSS_MAX_IRQ ||
	    end < 0 || end > ITSS_MAX_IRQ || end < start)
		return;

	reg_start = start / IRQS_PER_IPC;
	reg_end = (end + IRQS_PER_IPC - 1) / IRQS_PER_IPC;

	for (i = reg_start; i < reg_end; i++) {
		uint16_t reg = PCR_ITSS_IPC0_CONF + sizeof(uint32_t) * i;
		irq_snapshot[i] = pcr_read32(port, reg);
	}
}

static void show_irq_polarities(const char *msg)
{
	int i;
	const uint16_t port = PID_ITSS;

	printk(BIOS_INFO, "ITSS IRQ Polarities %s:\n", msg);
	for (i = 0; i < NUM_IPC_REGS; i++) {
		uint16_t reg = PCR_ITSS_IPC0_CONF + sizeof(uint32_t) * i;
		printk(BIOS_INFO, "IPC%d: 0x%08x\n", i, pcr_read32(port, reg));
	}
}

void itss_restore_irq_polarities(int start, int end)
{
	int i;
	int reg_start;
	int reg_end;
	const uint16_t port = PID_ITSS;

	if (start < 0 || start > ITSS_MAX_IRQ ||
	    end < 0 || end > ITSS_MAX_IRQ || end < start)
		return;

	show_irq_polarities("Before");

	reg_start = start / IRQS_PER_IPC;
	reg_end = (end + IRQS_PER_IPC - 1) / IRQS_PER_IPC;

	for (i = reg_start; i < reg_end; i++) {
		uint32_t mask;
		uint16_t reg;
		int irq_start;
		int irq_end;

		irq_start = i * IRQS_PER_IPC;
		irq_end = MIN(irq_start + IRQS_PER_IPC - 1, ITSS_MAX_IRQ);

		if (start > irq_end)
			continue;
		if (end < irq_start)
			break;

		/* Track bits within the bounds of the register. */
		irq_start = MAX(start, irq_start) % IRQS_PER_IPC;
		irq_end = MIN(end, irq_end) % IRQS_PER_IPC;

		/* Create bitmask of the inclusive range of start and end. */
		mask = (((1U << irq_end) - 1) | (1U << irq_end));
		mask &= ~((1U << irq_start) - 1);

		reg = PCR_ITSS_IPC0_CONF + sizeof(uint32_t) * i;
		pcr_rmw32(port, reg, ~mask, (mask & irq_snapshot[i]));
	}

	show_irq_polarities("After");
}

enum pirq itss_get_on_chip_dev_pirq(struct device *dev, enum pci_pin pin)
{
	/* Check if device is on chip. */
	if (dev->upstream->dev->path.type != DEVICE_PATH_DOMAIN)
		return PIRQ_INVALID;

	uint16_t pir = pcr_read16(PID_ITSS, itss_soc_get_on_chip_dev_pir(dev));
	if (pir < PCI_ITSS_PIR(0))
		return PIRQ_INVALID;

	/* The lower 3 bits of every 4 bits indicates which PIRQ is connect to INT. */
	unsigned int pir_shift = (pin - PCI_INT_A) * 4;
	unsigned int pir_mask = 0x07;
	return ((pir >> pir_shift) & pir_mask) + PIRQ_A;
}
