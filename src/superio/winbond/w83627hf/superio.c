#include <pci.h>
#include <cpu/p5/io.h>
#include <serial_subr.h>
#include <printk.h>
#include <pc80/keyboard.h>
#include <superio/generic.h>
#include <superio/w83627hf.h>

#if defined(SERIAL_CONSOLE)
#  if !defined(TTYS0_BASE)
#    define TTYS0_BASE 0x3f8
#  endif
#else
#undef TTYS0_BASE
#endif

void w83627hf_enter_pnp(unsigned char port)
{
	outb(0x87, port);
	outb(0x87, port);
}

void w83627hf_exit_pnp(unsigned char port)
{
	outb(0xaa, port);

}

static void setup_com(struct superio *sio,
	struct com_ports *com, int device)
{
	int divisor = 115200/com->baud;
	if ((com->base == TTYS0_BASE) && (!!pnp_read_enable(sio->port) == !!com->enable)) {
		/* Don't reinitialize the console serial port,
		 * This is especially nasty in SMP. 
		 */
		return;
	}
	printk_debug("Enabling com device: %02x\n", device);
	printk_debug("  iobase = 0x%04x  irq=%d\n", com->base, com->irq);
	/* Select the device */
	pnp_set_logical_device(sio->port, device);
	/* Disable it while it is initialized */
	pnp_set_enable(sio->port, 0);
	if (com->enable) {
		pnp_set_iobase0(sio->port, com->base);
		pnp_set_irq0(sio->port, com->irq);
		/* We are initialized so enable the device */
		pnp_set_enable(sio->port, 1);
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
	pnp_set_logical_device(sio->port, FLOPPY_DEVICE);
	/* Disable it while initializing */
	pnp_set_enable(sio->port, 0);
	if (sio->floppy) {
		pnp_set_iobase0(sio->port, iobase);
		pnp_set_irq0(sio->port, irq);
		pnp_set_drq(sio->port, drq);
		pnp_set_enable(sio->port, 1);
	}
}

static void setup_parallel(struct superio *sio)
{
	/* Remember the default resources */
	unsigned iobase = PARALLEL_DEFAULT_IOBASE;
	unsigned irq = PARALLEL_DEFAULT_IRQ;
	unsigned drq = PARALLEL_DEFAULT_DRQ;
	/* Select the device */
	pnp_set_logical_device(sio->port, PARALLEL_DEVICE);
	/* Disable it while initializing */
	pnp_set_enable(sio->port, 0);
	if (sio->lpt) {
		pnp_set_iobase0(sio->port, iobase);
		pnp_set_irq0(sio->port, irq);
		pnp_set_drq(sio->port, drq);
		pnp_set_enable(sio->port, 1);
	}
}

static void setup_keyboard(struct superio *sio)
{
	/* Remember the default resources */
	unsigned iobase0 = KBC_DEFAULT_IOBASE0;
	unsigned iobase1 = KBC_DEFAULT_IOBASE1;
	unsigned irq0 = KBC_DEFAULT_IRQ0;
	unsigned irq1 = KBC_DEFAULT_IRQ1;
	/* Select the device */
	pnp_set_logical_device(sio->port, KBC_DEVICE);
	/* Disable it while initializing */
	pnp_set_enable(sio->port, 0);
	if (sio->keyboard) {
		pnp_set_iobase0(sio->port, iobase0);
		pnp_set_iobase1(sio->port, iobase1);
		pnp_set_irq0(sio->port, irq0);
		pnp_set_irq1(sio->port, irq1);
		pnp_set_enable(sio->port, 1);
		/* Initialize the keyboard */
		pc_keyboard_init();
	}
}


#if 0
static void setup_acpi_registers(struct superio *sio)
{
	pnp_set_logical_device(sio->port, ACPI_DEVICE);
	/* Enable power on after power fail */
	pnp_write_config(sio->port, (1 << 7)|(0 <<5), 0xe4);
	pnp_set_enable(sio->port, 1);
}
#endif

static void enable_devices(struct superio *sio)
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

	w83627hf_enter_pnp(sio->port);

#if defined(SIO_SYSTEM_CLK_INPUT)
	/* Setup the clock input */
	pnp_write_config(sio->port, (0x84 | SIO_SYSTEM_CLK_INPUT), 0x24);
#endif

	/* enable/disable floppy */
	setup_floppy(sio);

	/* enable or disable parallel */
	setup_parallel(sio);

	/* enable/disable com1 */
	setup_com(sio, &sio->com1,  COM1_DEVICE);

	/* enable/disable com2 */
	setup_com(sio, &sio->com2,  COM2_DEVICE);

	/* enable/disable keyboard */
	setup_keyboard(sio);

	/* enable/disable cir */
	pnp_set_logical_device(sio->port, CIR_DEVICE);
	pnp_set_enable(sio->port, sio->cir);

	/*  game */
	pnp_set_logical_device(sio->port, GAME_PORT_DEVICE);
	pnp_set_enable(sio->port, sio->game);

	/*  gpio_port2 */
	pnp_set_logical_device(sio->port, GPIO_PORT2_DEVICE);
	pnp_set_enable(sio->port, sio->gpio2);

	/*  gpio_port3  */
	pnp_set_logical_device(sio->port, GPIO_PORT3_DEVICE);
	pnp_set_enable(sio->port, sio->gpio3);

	/* enable/disable acpi  */
	pnp_set_logical_device(sio->port, ACPI_DEVICE);
	pnp_set_enable(sio->port, sio->acpi);

	/* enable/disable hw monitor */
	pnp_set_logical_device(sio->port, HW_MONITOR_DEVICE);
	pnp_set_enable(sio->port, sio->hwmonitor);

#if 0
	/* setup acpi registers so I am certain to get
	 * power on after power fail.
	 */
	setup_acpi_registers(sio);
#endif

	w83627hf_exit_pnp(sio->port);
}

/* The base address is either 0x2e or 0x4e */
struct superio_control superio_winbond_w83627hf_control = {
	(void *)0, enable_devices, (void *)0, 0x2e, "w83627hf"
};
