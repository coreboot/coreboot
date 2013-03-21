#include <arch/io.h>

static void mch_reset(void)
{
        device_t dev;
        unsigned long value, base;
        dev = pci_locate_device_on_bus(PCI_ID(0x8086, 0x24d0), 0);
        if (dev != PCI_DEV_INVALID) {
                /* I/O space is always enables */

                /* Set gpio base */
                pci_write_config32(dev, 0x58, ICH5_GPIOBASE | 1);
                base = ICH5_GPIOBASE;

                /* Enable GPIO Bar */
                value = pci_read_config32(dev, 0x5c);
                value |= 0x10;
                pci_write_config32(dev, 0x5c, value);

		/* Set GPIO 19 mux to IO usage */
		value = inl(base);
		value |= (1 <<19);
		outl(value, base);

                /* Pull GPIO 19 low */
                value = inl(base + 0x0c);
                value &= ~(1 << 19);
                outl(value, base + 0x0c);
        }
        return;
}

static void mainboard_set_e7520_pll(unsigned bits)
{
	uint16_t gpio_index;
	uint8_t data;
	device_t dev;

	/* currently only handle the Jarrell/PC87427 case */
	dev = PC87427_GPIO_DEV;


	pnp_set_logical_device(dev);
	gpio_index = pnp_read_iobase(dev, 0x60);

	/* select SIO GPIO port 4, pin 2 */
	pnp_write_config(dev, PC87427_GPSEL, ((pnp_read_config(dev, PC87427_GPSEL) & 0x88) | 0x42));
	/* set to push-pull, enable output */
	pnp_write_config(dev, PC87427_GPCFG1, 0x03);

	/* select SIO GPIO port 4, pin 4 */
	pnp_write_config(dev, PC87427_GPSEL, ((pnp_read_config(dev, PC87427_GPSEL) & 0x88) | 0x44));
	/* set to push-pull, enable output */
	pnp_write_config(dev, PC87427_GPCFG1, 0x03);

	/* set gpio 42,44 signal levels */
	data = inb(gpio_index + PC87427_GPDO_4);
	if ((data & 0x14) == (0xff & (((bits&2)?0:1)<<4 | ((bits&1)?0:1)<<2))) {
		print_debug("set_pllsel: correct settings detected!\n");
		return; /* settings already configured */
	} else {
		outb((data & 0xeb) | ((bits&2)?0:1)<<4 | ((bits&1)?0:1)<<2, gpio_index + PC87427_GPDO_4);
		/* reset */
		print_debug("set_pllsel: settings adjusted, now resetting...\n");
		// hard_reset(); /* should activate a PCI_RST, which should reset MCH, but it doesn't seem to work ???? */
		// mch_reset();
		full_reset();
	}
	return;
}

static void mainboard_set_e7520_leds(void)
{
	uint8_t cnt;
	uint8_t data;
	device_t dev;

	/* currently only handle the Jarrell/PC87427 case */
	dev = PC87427_GPIO_DEV;

	pnp_set_logical_device(dev);

	/* enable */
	outb(0x30, 0x2e);
	outb(0x01, 0x2f);
	outb(0x2d, 0x2e);
	outb(0x01, 0x2f);

	/* Set auto mode for dimm leds and post */
	outb(0xf0,0x2e);
	outb(0x70,0x2f);
	outb(0xf4,0x2e);
	outb(0x30,0x2f);
	outb(0xf5,0x2e);
	outb(0x88,0x2f);
	outb(0xf6,0x2e);
	outb(0x00,0x2f);
	outb(0xf7,0x2e);
	outb(0x90,0x2f);
	outb(0xf8,0x2e);
	outb(0x00,0x2f);

	/* Turn the leds off */
	outb(0x00,0x88);
	outb(0x00,0x90);

	/* Disable the ports */
	outb(0xf5,0x2e);
	outb(0x00,0x2f);
	outb(0xf7,0x2e);
	outb(0x00,0x2f);
	outb(0xf4,0x2e);
	outb(0x00,0x2f);

	return;
}

