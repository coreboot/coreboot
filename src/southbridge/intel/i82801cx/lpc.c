/*
 * (C) 2003 Linux Networx, SuSE Linux AG
 * (C) 2004 Tyan Computer
 * (c) 2005 Digital Design Corporation
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include "i82801cx.h"

#define NMI_OFF 0

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1


/**
 * Set miscellanous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void i82801cx_enable_ioapic(struct device *dev)
{
	u32 reg32;

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (1 << 13);	/* Coprocessor error enable (COPR_ERR_EN) */
	reg32 |= (3 << 7);	/* IOAPIC enable (APIC_EN) */
	reg32 |= (1 << 2);	/* DMA collection buffer enable (DCB_EN) */
	reg32 |= (1 << 1);	/* Delayed transaction enable (DTE) */
	pci_write_config32(dev, GEN_CNTL, reg32);
	printk(BIOS_DEBUG, "IOAPIC Southbridge enabled %x\n", reg32);

	set_ioapic_id(IO_APIC_ADDR, 0x02);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(IO_APIC_ADDR, 0x03, 0x01);
}

// This is how interrupts are received from the Super I/O chip
static void i82801cx_enable_serial_irqs( struct device *dev)
{
	// Recognize serial IRQs, continuous mode, frame size 21, 4 clock start frame pulse width
    pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0<< 0));
}

/**
 * Route all DMA channels to either PCI or LPC.
 *
 * @param dev TODO
 * @param mask Identifies whether each channel should be used for PCI DMA
 *             (bit = 0) or LPC DMA (bit = 1). The LSb controls channel 0.
 *             Channel 4 is not used (reserved).
 */
static void i82801cx_lpc_route_dma( struct device *dev, uint8_t mask)
{
    uint16_t dmaConfig;
    int channelIndex;

    dmaConfig = pci_read_config16(dev, PCI_DMA_CFG);
    dmaConfig &= 0x300;				// Preserve reserved bits
    for(channelIndex = 0; channelIndex < 8; channelIndex++) {
    	if (channelIndex == 4)
        	continue;		// Register doesn't support channel 4
        dmaConfig |= ((mask & (1 << channelIndex))? 3:1) << (channelIndex*2);
    }
    pci_write_config16(dev, PCI_DMA_CFG, dmaConfig);
}

static void i82801cx_rtc_init(struct device *dev)
{
    uint32_t dword;
    int rtc_failed;
	int pwr_on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
    uint8_t pmcon3 = pci_read_config8(dev, GEN_PMCON_3);

    rtc_failed = pmcon3 & RTC_BATTERY_DEAD;
    if (rtc_failed) {
    	// Clear the RTC_BATTERY_DEAD bit, but preserve
        // the RTC_POWER_FAILED, G3 state, and reserved bits
		// NOTE: RTC_BATTERY_DEAD and RTC_POWER_FAILED are "write-1-clear" bits
        pmcon3 &= ~RTC_POWER_FAILED;
    }

    get_option(&pwr_on, "power_on_after_fail");
	pmcon3 &= ~SLEEP_AFTER_POWER_FAIL;
	if (!pwr_on) {
		pmcon3 |= SLEEP_AFTER_POWER_FAIL;
	}
	pci_write_config8(dev, GEN_PMCON_3, pmcon3);
	printk(BIOS_INFO, "set power %s after power fail\n",
				 pwr_on ? "on" : "off");

    // See if the Safe Mode jumper is set
    dword = pci_read_config32(dev, GEN_STS);
    rtc_failed |= dword & (1 << 2);

    rtc_init(rtc_failed);
}


static void i82801cx_1f0_misc(struct device *dev)
{
	// Prevent LPC disabling, enable parity errors, and SERR# (System Error)
    pci_write_config16(dev, PCI_COMMAND, 0x014f);

    // Set ACPI base address to 0x1100 (I/O space)
    pci_write_config32(dev, PMBASE, 0x00001101);

    // Enable ACPI I/O and power management
    pci_write_config8(dev, ACPI_CNTL, 0x10);

    // Set GPIO base address to 0x1180 (I/O space)
    pci_write_config32(dev, GPIO_BASE, 0x00001181);

    // Enable GPIO
    pci_write_config8(dev, GPIO_CNTL, 0x10);

    // Route PIRQA to IRQ11, PIRQB to IRQ3, PIRQC to IRQ5, PIRQD to IRQ10
    pci_write_config32(dev, PIRQA_ROUT, 0x0A05030B);

    // Route PIRQE to IRQ7. Leave PIRQF - PIRQH unrouted.
    pci_write_config8(dev, PIRQE_ROUT, 0x07);

    // Enable access to the upper 128 byte bank of CMOS RAM
    pci_write_config8(dev, RTC_CONF, 0x04);

    // Decode 0x3F8-0x3FF (COM1) for COMA port,
	//		  0x2F8-0x2FF (COM2) for COMB
    pci_write_config8(dev, COM_DEC, 0x10);

	// LPT decode defaults to 0x378-0x37F and 0x778-0x77F
	// Floppy decode defaults to 0x3F0-0x3F5, 0x3F7

    // Enable COMA, COMB, LPT, floppy;
	// disable microcontroller, Super I/O, sound, gameport
    pci_write_config16(dev, LPC_EN, 0x000F);
}

static void lpc_init(struct device *dev)
{
	uint8_t byte;
	int pwr_on=-1;
	int nmi_option;

	/* IO APIC initialization */
	i82801cx_enable_ioapic(dev);

	i82801cx_enable_serial_irqs(dev);

	/* power after power fail */
	        /* FIXME this doesn't work! */
        /* Which state do we want to goto after g3 (power restored)?
         * 0 == S0 Full On
         * 1 == S5 Soft Off
         */
    byte = pci_read_config8(dev, GEN_PMCON_3);
    if (pwr_on)
    	byte &= ~1;		// Return to S0 (boot) after power is re-applied
    else
    	byte |= 1;		// Return to S5
    pci_write_config8(dev, GEN_PMCON_3, byte);
    printk(BIOS_INFO, "set power %s after power fail\n", pwr_on?"on":"off");

    /* Set up NMI on errors */
    byte = inb(0x61);
    byte &= ~(1 << 3); /* IOCHK# NMI Enable */
    byte &= ~(1 << 2); /* PCI SERR# Enable */
    outb(byte, 0x61);
    byte = inb(0x70);
    nmi_option = NMI_OFF;
    get_option(&nmi_option, "nmi");
    if (nmi_option) {
        byte &= ~(1 << 7); /* set NMI */
        outb(byte, 0x70);
    }

	/* Initialize the real time clock */
	i82801cx_rtc_init(dev);

	i82801cx_lpc_route_dma(dev, 0xff);

	/* Initialize isa dma */
	isa_dma_init();

	i82801cx_1f0_misc(dev);
}

static void i82801cx_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations lpc_ops  = {
	.read_resources   = i82801cx_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_static_bus,
	.enable           = 0,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801CA_LPC,
};
