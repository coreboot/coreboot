// THIS FILE DOES NOT REALLY WORK! We need to fix it WRT the docs. RGM

#include <pci.h>
#include <cpu/p5/io.h>
#include <serial_subr.h>
#include <printk.h>

#define FLOPPY_DEVICE 0
#define PARALLEL_DEVICE 1
#define COM1_DEVICE 2
#define COM2_DEVICE 3
#define KBC_DEVICE  5
#define CIR_DEVICE  6
#define GAME_PORT_DEVICE 7
#define GPIO_PORT2_DEVICE 8
#define GPIO_PORT3_DEVICE 9
#define ACPI_DEVICE 0xa
#define HW_MONITOR_DEVICE 0xb


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
#define KBC_DEFAULT_IOBASE0	0x60
#define KBC_DEFAULT_IOBASE1	0x64
#define KBC_DEFAULT_IRQ0	0x1
#define KBC_DEFAULT_IRQ1	0xc


static void enter_pnp(struct superio *sio)
{
	outb(0x87, sio->port);
	outb(0x87, sio->port);
}

static void exit_pnp(struct superio *sio)
{
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

static void set_enable(struct superio *sio, int enable)
{
	write_config(sio, enable?0x1:0x0, 0x30);
#if 1
	if (enable) {
		printk_debug("enabled superio device: %d\n", 
			read_config(sio, 0x07));
	}
#endif
}

static void set_iobase0(struct superio *sio, unsigned iobase)
{
	write_config(sio, (iobase >> 8) & 0xff, 0x60);
	write_config(sio, iobase & 0xff, 0x61);
}

static void set_iobase1(struct superio *sio, unsigned iobase)
{
	write_config(sio, (iobase >> 8) & 0xff, 0x62);
	write_config(sio, iobase & 0xff, 0x63);
}

static void set_irq0(struct superio *sio, unsigned irq)
{
	write_config(sio, irq, 0x70);
}

static void set_irq1(struct superio *sio, unsigned irq)
{
	write_config(sio, irq, 0x72);
}

static void set_drq(struct superio *sio, unsigned drq)
{
	write_config(sio, drq & 0xff, 0x74);
}

static void setup_com(struct superio *sio,
	struct com_ports *com, int device)
{
	// set baud, default to 115200 if not set.
	int divisor = 115200/(com->baud ? com->baud : 1);
	printk_debug("%s com device: %02x\n", 
			com->enable? "Enabling" : "Disabling", device);
	/* Select the device */
	set_logical_device(sio, device);
	/* Disable it while it is initialized */
	set_enable(sio, 0);
	if (com->enable) {
		printk_debug("  iobase = 0x%04x  irq=%d\n", 
				com->base, com->irq);
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

static void setup_parallel(struct superio *sio)
{
	/* Remember the default resources */
	unsigned iobase = PARALLEL_DEFAULT_IOBASE;
	unsigned irq = PARALLEL_DEFAULT_IRQ;
	unsigned drq = PARALLEL_DEFAULT_DRQ;
	/* Select the device */
	set_logical_device(sio, PARALLEL_DEVICE);
	/* Disable it while initializing */
	set_enable(sio, 0);
	if (sio->lpt) {
		set_iobase0(sio, iobase);
		set_irq0(sio, irq);
		set_drq(sio, drq);
		set_enable(sio, 1);
	}
}
//do we have a keyboard on this chip? 
#if 0
static void setup_keyboard(struct superio *sio)
{
	/* Remember the default resources */
	unsigned iobase0 = KBC_DEFAULT_IOBASE0;
	unsigned iobase1 = KBC_DEFAULT_IOBASE1;
	unsigned irq0 = KBC_DEFAULT_IRQ0;
	unsigned irq1 = KBC_DEFAULT_IRQ1;
	/* Select the device */
	set_logical_device(sio, KBC_DEVICE);
	/* Disable it while initializing */
	set_enable(sio, 0);
	if (sio->keyboard) {
		set_iobase0(sio, iobase0);
		set_iobase1(sio, iobase1);
		set_irq0(sio, irq0);
		set_irq1(sio, irq1);
		set_enable(sio, 1);
		/* Initialize the keyboard */
		pc_keyboard_init();
	}
}
#endif

#if 0
static void setup_acpi_registers(struct superio *sio)
{
	set_logical_device(sio, ACPI_DEVICE);
	/* Enable power on after power fail */
	write_config(sio, (1 << 7)|(0 <<5), 0xe4);
	set_enable(sio, 1);
}
#endif

static void enable_devices(struct superio *sio)
{
	printk_info("Setting up %s\n", sio->super->name);
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

	/* enable/disable floppy */
	setup_floppy(sio);

	/* enable or disable parallel */
	setup_parallel(sio);

	/* enable/disable com1 */
	setup_com(sio, &sio->com1,  COM1_DEVICE);

	/* enable/disable com2 */
	setup_com(sio, &sio->com2,  COM2_DEVICE);

	/* enable/disable keyboard */
	// keyboard present? setup_keyboard(sio);

	/* enable/disable cir */
	set_logical_device(sio, CIR_DEVICE);
	set_enable(sio, sio->cir);

	/*  game */
	set_logical_device(sio, GAME_PORT_DEVICE);
	set_enable(sio, sio->game);

	/*  gpio_port2 */
	set_logical_device(sio, GPIO_PORT2_DEVICE);
	set_enable(sio, sio->gpio2);

	/*  gpio_port3  */
	set_logical_device(sio, GPIO_PORT3_DEVICE);
	set_enable(sio, sio->gpio3);

	/* enable/disable acpi  */
	set_logical_device(sio, ACPI_DEVICE);
	set_enable(sio, sio->acpi);

	/* enable/disable hw monitor */
	set_logical_device(sio, HW_MONITOR_DEVICE);
	set_enable(sio, sio->hwmonitor);

#if 0
	/* setup acpi registers so I am certain to get
	 * power on after power fail.
	 */
	setup_acpi_registers(sio);
#endif

	// what's this.
	write_config(sio, 1, 0x30);
	exit_pnp(sio);
}

/* The base address is either 0x2e or 0x4e */
struct superio_control superio_winbond_w83877tf_control = {
	pre_pci_init : (void *)0, 
	init:          enable_devices, 
        finishup:      (void *)0, 
        defaultport:   0x2e, 
        name:          "w83877tf"
};
