#include <superio/generic.h>
#include <superio/w83627hf.h>

void w83627hf_power_after_power_fail(int state)
{
	unsigned char byte;
	w83627hf_enter_pnp(SIO_BASE);
	pnp_set_logical_device(SIO_BASE, ACPI_DEVICE);
	pnp_set_enable(SIO_BASE, 1);

	/* Enable power on after power fail */
	byte = pnp_read_config(SIO_BASE, 0xe4);
	byte &= ~(3 << 5);
	byte |= (state & 3) << 5;
	pnp_write_config(SIO_BASE, byte, 0xe4);
	
	w83627hf_exit_pnp(SIO_BASE);
}


