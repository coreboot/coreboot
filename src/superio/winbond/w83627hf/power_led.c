#include <superio/generic.h>
#include <superio/w83627hf.h>

void power_led(int state)
{
	unsigned char byte;

        w83627hf_enter_pnp(SIO_BASE);
        pnp_set_logical_device(SIO_BASE, GPIO_PORT3_DEVICE);
        pnp_set_enable(SIO_BASE, 1);

        /* Enable power LED */
        byte = pnp_read_config(SIO_BASE, 0xf3);
	byte &= ~(3 << 6);
        byte |= state;
        pnp_write_config(SIO_BASE, byte, 0xf3);

        w83627hf_exit_pnp(SIO_BASE);

}


