// you can't make this stuff common. Things change a lot from superio to superio. sorry.

#include <subr.h>
#include <cpu/p5/io.h>

// just define these here. We may never need them anywhere else
#define FLOPPY_DEVICE 0
#define COM1_DEVICE 2
#define COM2_DEVICE 3

#define FLOPPY_DEFAULT_IOBASE	0x3f0
#define FLOPPY_DEFAULT_IRQ	6
#define FLOPPY_DEFAULT_DRQ	2
#define PARALLEL_DEFAULT_IOBASE	0x378
#define PARALLEL_DEFAULT_IRQ	7
#define PARALLEL_DEFAULT_DRQ	4 /* No dma */
#define COM1_DEFAULT_IOBASE	0x3f8
#define COM1_DEFAULT_IRQ	4
#define COM1_DEFAULT_BAUD	115200
#define COM2_DEFAULT_IOBASE	0x2f8
#define COM2_DEFAULT_IRQ	3
#define COM2_DEFAULT_BAUD	115200


// funny how all these chips are "pnp compatible", and they're all different. 
#define PNPADDR 0x3f0

void
enter_pnp(struct superio *sio)
{
	// unlock it XXX make this a subr at some point 
	outb(0x87, sio->port);
	outb(0x87, sio->port);
}

void
exit_pnp(struct superio *sio)
{
	/* all done. */
	// select configure control
	outb(0xaa, sio->port);
}

static void write_config(struct superio *sio, 
	unsigned char value, unsigned char reg)
{
	outb(reg, sio->port);
	outb(value, sio->port +1);
}

static unsigned char read_config(struct superio *sio, unsigned char reg)
{
	outb(reg, sio->port);
	return inb(sio->port +1);
}
static void set_logical_device(struct superio *sio, int device)
{
	write_config(sio, device, 0x07);
}

static void set_irq0(struct superio *sio, unsigned irq)
{
	write_config(sio, irq, 0x70);
}

static void set_irq1(struct superio *sio, unsigned irq)
{
	write_config(sio, irq, 0x72);
}

static void set_enable(struct superio *sio, int enable)
{
	write_config(sio, enable?0x1:0x0, 0x30);
#if 0
	if (enable) {
		printk_debug("enabled superio device: %d\n", 
			read_config(sio, 0x07));
	}
#endif
}

static void setup_com(struct superio *sio,
	struct com_ports *com, int device)
{
	int divisor = 115200/com->baud;
	printk_debug("Enabling com device: %02x\n", device);
	printk_debug("  iobase = 0x%04x  irq=%d\n", com->base, com->irq);
	/* Select the device */
	set_logical_device(sio, device);
	/* Disable it while it is initialized */
	set_enable(sio, 0);
	if (com->enable) {
		set_iobase0(sio, com->base);
		set_irq0(sio, com->irq);
		/* We are initialized so enable the device */
		set_enable(sio, 1);
		/* Now initialize the com port */
		uart_init(com->base, divisor);
	}
}

static void setup_floppy(struct superio *sio)
{
	/* Remember the default resources */
	unsigned iobase = FLOPPY_DEFAULT_IOBASE;
	unsigned irq = FLOPPY_DEFAULT_IRQ;
	unsigned drq = FLOPPY_DEFAULT_DRQ;
	/* Select the device */
	set_logical_device(sio, FLOPPY_DEVICE);
	/* Disable it while initializing */
	set_enable(sio, 0);
	if (sio->floppy) {
		set_iobase0(sio, iobase);
		set_irq0(sio, irq);
		set_drq(sio, drq);
		set_enable(sio, 1);
	}
}


#if 0
#ifdef MUST_ENABLE_FLOPPY

void setup_floppy(struct superio *sio)
{
	/* now set the LDN to floppy LDN */
	outb(0x7, sio->port);	/* pick reg. 7 */
	outb(0x0, sio->port+1);	/* LDN 0 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, sio->port);
	outb(0x1, sio->port+1);
}
#endif /* MUST_ENABLE_FLOPPY */

void
setup_com(struct superio *sio, int com)
{
    unsigned char b;
    /* now set the LDN to com LDN */
    outb(0x7, sio->port);	/* pick reg. 7 */
    outb(com, sio->port+1);	/* LDN 0 to reg. 7 */

    /* now select register 0x30, and set bit 1 in that register */
    outb(0x30, sio->port);
    outb(0x1, sio->port+1);

}

#endif
static void setup_devices(struct superio *sio)
{
	if (sio->port == 0) {
		sio->port = sio->super->defaultport;
	}
	if (sio->com1.base == 0)	sio->com1.base = COM1_DEFAULT_IOBASE;
	if (sio->com1.irq == 0) 	sio->com1.irq = COM1_DEFAULT_IRQ;
	if (sio->com1.baud == 0)	sio->com1.baud = COM1_DEFAULT_BAUD;
	if (sio->com2.base == 0) 	sio->com2.base = COM2_DEFAULT_IOBASE;
	if (sio->com2.irq == 0) 	sio->com2.irq = COM2_DEFAULT_IRQ;
	if (sio->com2.baud == 0)	sio->com2.baud = COM2_DEFAULT_BAUD;

	enter_pnp(sio);

	/* setup/disable floppy */
	setup_floppy(sio);

	/* enable/disable com1 */
	setup_com(sio, &sio->com1,  COM1_DEVICE);

	/* enable/disable com2 */
	setup_com(sio, &sio->com2,  COM2_DEVICE);

	exit_pnp(sio);
}

struct superio_control superio_winbond_w83977ef_control = {
        pre_pci_init:   (void *) 0,
        init:           setup_devices, 
        finishup:       (void *) 0,
        defaultport:    PNPADDR,
        name:           "WinBond w83977tf"
};

#ifndef USE_NEW_SUPERIO_INTERFACE

// this must die soon. 
void
final_superio_fixup()
{
	static struct superio temp = { &superio_winbond_w83977ef_control, 
				       .com1={1}, .floppy=1};
	
	finishup(&temp); 
/*
    enable_com(PNP_COM1_DEVICE);
    enable_com(PNP_COM2_DEVICE);

    exit_pnp();
*/
}
#endif
